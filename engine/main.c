/*
 * Shared wiki engine for the TI-84 Plus CE.
 *
 * Browse by category, list every article at once, or search titles by typing
 * on the ALPHA key labels. Articles are stored as newline separated, style
 * prefixed lines and indexed on open.
 *
 * This file is content agnostic: everything it displays comes from the
 * generated content.c / content.h of whichever wiki is being built (see
 * engine/tools/build_content.py). Each wiki's makefile pulls this in with
 * EXTRA_CSOURCES.
 */

#include <graphx.h>
#include <keypadc.h>
#include <ti/screen.h>
#include <sys/timers.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "content.h"

/* ------------------------------------------------------------------ */
/* Palette                                                             */
/* ------------------------------------------------------------------ */

enum {
    C_NONE = 0,   /* transparent marker: never drawn */
    C_BG,
    C_TEXT,
    C_HEADER,
    C_HEADERTXT,
    C_ACCENT,
    C_FORMULA,
    C_BAND,
    C_GRAY,
    C_SELBG,
    C_SELTXT,
    C_FOOTBG,
    C_RULE,
    C_BAR,
    C_SHADOW,
    C_COUNT
};

static void init_palette(void)
{
    gfx_palette[C_NONE]      = gfx_RGBTo1555(255,   0, 255);
    gfx_palette[C_BG]        = gfx_RGBTo1555(250, 250, 247);
    gfx_palette[C_TEXT]      = gfx_RGBTo1555( 24,  24,  28);
    gfx_palette[C_HEADER]    = gfx_RGBTo1555( 28,  42,  84);
    gfx_palette[C_HEADERTXT] = gfx_RGBTo1555(255, 255, 255);
    gfx_palette[C_ACCENT]    = gfx_RGBTo1555( 18,  96, 190);
    gfx_palette[C_FORMULA]   = gfx_RGBTo1555(150,  55,  10);
    gfx_palette[C_BAND]      = gfx_RGBTo1555(235, 235, 232);
    gfx_palette[C_GRAY]      = gfx_RGBTo1555(122, 122, 130);
    gfx_palette[C_SELBG]     = gfx_RGBTo1555( 18,  96, 190);
    gfx_palette[C_SELTXT]    = gfx_RGBTo1555(255, 255, 255);
    gfx_palette[C_FOOTBG]    = gfx_RGBTo1555(238, 238, 242);
    gfx_palette[C_RULE]      = gfx_RGBTo1555(205, 205, 212);
    gfx_palette[C_BAR]       = gfx_RGBTo1555(150, 150, 160);
    gfx_palette[C_SHADOW]    = gfx_RGBTo1555(180, 180, 188);
}

/* ------------------------------------------------------------------ */
/* Layout                                                              */
/* ------------------------------------------------------------------ */

#define SCR_W        320
#define SCR_H        240

#define HDR_H         18
#define FTR_H         16
#define FTR_Y        (SCR_H - FTR_H)

#define BODY_Y       (HDR_H + 3)
#define BODY_H       (FTR_Y - BODY_Y - 2)

#define TEXT_LH       10                    /* article line height */
#define TEXT_ROWS    (BODY_H / TEXT_LH)     /* 20 lines of article text */

#define LIST_LH       14                    /* menu row height */
#define LIST_ROWS    (BODY_H / LIST_LH)     /* 13 menu rows */

#define SB_X         311                    /* scrollbar */
#define SB_W           6

/* ------------------------------------------------------------------ */
/* Keys                                                                */
/* ------------------------------------------------------------------ */

typedef enum {
    K_NONE = 0,
    K_UP, K_DOWN, K_LEFT, K_RIGHT,
    K_ENTER, K_2ND, K_CLEAR, K_DEL, K_MODE,
    K_YEQU, K_WINDOW, K_ZOOM, K_TRACE, K_GRAPH,
    K_CHAR
} key_t;

typedef struct {
    uint8_t group;
    uint8_t mask;
    uint8_t code;
    char    ch;
} keyent_t;

/* Letters follow the ALPHA labels printed on the keypad, so typing feels
 * normal without actually toggling ALPHA mode. */
static const keyent_t keytab[] = {
    { 7, kb_Up,     K_UP,     0 },
    { 7, kb_Down,   K_DOWN,   0 },
    { 7, kb_Left,   K_LEFT,   0 },
    { 7, kb_Right,  K_RIGHT,  0 },
    { 6, kb_Enter,  K_ENTER,  0 },
    { 1, kb_2nd,    K_2ND,    0 },
    { 6, kb_Clear,  K_CLEAR,  0 },
    { 1, kb_Del,    K_DEL,    0 },
    { 1, kb_Mode,   K_MODE,   0 },
    { 1, kb_Yequ,   K_YEQU,   0 },
    { 1, kb_Window, K_WINDOW, 0 },
    { 1, kb_Zoom,   K_ZOOM,   0 },
    { 1, kb_Trace,  K_TRACE,  0 },
    { 1, kb_Graph,  K_GRAPH,  0 },

    { 2, kb_Math,   K_CHAR, 'A' },
    { 3, kb_Apps,   K_CHAR, 'B' },
    { 4, kb_Prgm,   K_CHAR, 'C' },
    { 2, kb_Recip,  K_CHAR, 'D' },
    { 3, kb_Sin,    K_CHAR, 'E' },
    { 4, kb_Cos,    K_CHAR, 'F' },
    { 5, kb_Tan,    K_CHAR, 'G' },
    { 6, kb_Power,  K_CHAR, 'H' },
    { 2, kb_Square, K_CHAR, 'I' },
    { 3, kb_Comma,  K_CHAR, 'J' },
    { 4, kb_LParen, K_CHAR, 'K' },
    { 5, kb_RParen, K_CHAR, 'L' },
    { 6, kb_Div,    K_CHAR, 'M' },
    { 2, kb_Log,    K_CHAR, 'N' },
    { 3, kb_7,      K_CHAR, 'O' },
    { 4, kb_8,      K_CHAR, 'P' },
    { 5, kb_9,      K_CHAR, 'Q' },
    { 6, kb_Mul,    K_CHAR, 'R' },
    { 2, kb_Ln,     K_CHAR, 'S' },
    { 3, kb_4,      K_CHAR, 'T' },
    { 4, kb_5,      K_CHAR, 'U' },
    { 5, kb_6,      K_CHAR, 'V' },
    { 6, kb_Sub,    K_CHAR, 'W' },
    { 2, kb_Sto,    K_CHAR, 'X' },
    { 3, kb_1,      K_CHAR, 'Y' },
    { 4, kb_2,      K_CHAR, 'Z' },
    { 3, kb_0,      K_CHAR, ' ' },
};

#define KEYTAB_LEN (sizeof(keytab) / sizeof(keytab[0]))

static char key_char;

/* Blocks until a key is pressed. Arrows auto-repeat when held. */
static key_t get_key(void)
{
    static uint8_t held = 0xFF;
    static uint8_t frames;

    for (;;) {
        uint8_t i, found = 0xFF;

        kb_Scan();
        for (i = 0; i < KEYTAB_LEN; i++) {
            if (kb_Data[keytab[i].group] & keytab[i].mask) {
                found = i;
                break;
            }
        }

        if (found == 0xFF) {
            held = 0xFF;
            frames = 0;
        } else if (found != held) {
            held = found;
            frames = 0;
            key_char = keytab[found].ch;
            return (key_t)keytab[found].code;
        } else {
            uint8_t code = keytab[found].code;
            bool arrow = (code == K_UP || code == K_DOWN ||
                          code == K_LEFT || code == K_RIGHT);
            if (frames < 255) {
                frames++;
            }
            if (arrow && frames > 16 && (frames % 3) == 0) {
                key_char = keytab[found].ch;
                return (key_t)code;
            }
        }
        delay(10);
    }
}

/* ------------------------------------------------------------------ */
/* Small helpers                                                       */
/* ------------------------------------------------------------------ */

static char linebuf[80];
static char tmpbuf[64];

/* Copy at most max characters, marking truncation with a trailing '.'. */
static const char *fit(const char *s, uint8_t max)
{
    uint8_t len = (uint8_t)strlen(s);

    if (max > sizeof(tmpbuf) - 1) {
        max = sizeof(tmpbuf) - 1;
    }
    if (len <= max) {
        return s;
    }
    memcpy(tmpbuf, s, max - 1);
    tmpbuf[max - 1] = '.';
    tmpbuf[max] = '\0';
    return tmpbuf;
}

static void print_right(const char *s, int x_right, int y)
{
    gfx_PrintStringXY(s, x_right - (int)(strlen(s) * 8), y);
}

static char upper(char c)
{
    return (c >= 'a' && c <= 'z') ? (char)(c - 32) : c;
}

/* Case insensitive substring test. */
static bool contains(const char *hay, const char *needle)
{
    if (!*needle) {
        return true;
    }
    for (; *hay; hay++) {
        const char *h = hay, *n = needle;
        while (*n && *h && upper(*h) == upper(*n)) {
            h++;
            n++;
        }
        if (!*n) {
            return true;
        }
    }
    return false;
}

/* ------------------------------------------------------------------ */
/* Chrome                                                              */
/* ------------------------------------------------------------------ */

static void draw_header(const char *title, const char *right)
{
    gfx_SetColor(C_HEADER);
    gfx_FillRectangle_NoClip(0, 0, SCR_W, HDR_H);
    gfx_SetColor(C_ACCENT);
    gfx_FillRectangle_NoClip(0, HDR_H, SCR_W, 2);

    gfx_SetTextFGColor(C_HEADERTXT);
    gfx_PrintStringXY(fit(title, right ? 30 : 38), 6, 5);
    if (right) {
        print_right(right, SCR_W - 6, 5);
    }
}

static void draw_footer(const char *hint)
{
    gfx_SetColor(C_FOOTBG);
    gfx_FillRectangle_NoClip(0, FTR_Y, SCR_W, FTR_H);
    gfx_SetColor(C_RULE);
    gfx_FillRectangle_NoClip(0, FTR_Y, SCR_W, 1);

    gfx_SetTextFGColor(C_GRAY);
    gfx_PrintStringXY(fit(hint, 39), 5, FTR_Y + 4);
}

static void draw_scrollbar(unsigned int top, unsigned int visible,
                           unsigned int total)
{
    unsigned int track = BODY_H;
    unsigned int h, y;

    if (total <= visible) {
        return;
    }

    gfx_SetColor(C_BAND);
    gfx_FillRectangle_NoClip(SB_X, BODY_Y, SB_W, track);

    h = track * visible / total;
    if (h < 12) {
        h = 12;
    }
    y = (track - h) * top / (total - visible);

    gfx_SetColor(C_BAR);
    gfx_FillRectangle_NoClip(SB_X, BODY_Y + (int)y, SB_W, (int)h);
}

static void frame_begin(void)
{
    gfx_FillScreen(C_BG);
}

static void frame_end(void)
{
    gfx_SwapDraw();
}

/* ------------------------------------------------------------------ */
/* Application state                                                   */
/* ------------------------------------------------------------------ */

typedef enum {
    S_CATS = 0,   /* category menu */
    S_ARTS,       /* articles inside one category */
    S_ALL,        /* every article, A-Z */
    S_SEARCH,     /* incremental title search */
    S_VIEW        /* reading an article */
} screen_t;

static screen_t screen = S_CATS;
static screen_t view_from = S_ARTS;

static unsigned int cat_sel, cat_top;
static unsigned int art_sel, art_top;
static unsigned int all_sel, all_top;

static char         query[20];
static uint8_t      query_len;
static uint16_t     results[256];
static unsigned int nresults, res_sel, res_top;

static uint8_t      cur_cat, cur_art;
static const char  *lines[WIKI_MAX_LINES];
static unsigned int nlines, view_top;

static bool running = true;

/* ------------------------------------------------------------------ */
/* Article handling                                                    */
/* ------------------------------------------------------------------ */

static const article_t *article_at(uint8_t cat, uint8_t art)
{
    return &wiki_categories[cat].articles[art];
}

static void open_article(uint8_t cat, uint8_t art, screen_t from)
{
    const article_t *a = article_at(cat, art);
    const char *p = a->body;

    cur_cat = cat;
    cur_art = art;
    view_from = from;
    view_top = 0;
    nlines = 0;

    while (nlines < a->num_lines && nlines < WIKI_MAX_LINES) {
        lines[nlines++] = p;
        p = strchr(p, '\n');
        if (!p) {
            break;
        }
        p++;
    }

    screen = S_VIEW;
}

/* Copy display line n (without its style byte) into linebuf. */
static char line_text(unsigned int n)
{
    const char *p = lines[n];
    char style = *p++;
    unsigned int i = 0;

    while (*p && *p != '\n' && i < sizeof(linebuf) - 1) {
        linebuf[i++] = *p++;
    }
    linebuf[i] = '\0';
    return style;
}

static void draw_view(void)
{
    const article_t *a = article_at(cur_cat, cur_art);
    unsigned int i;
    int y = BODY_Y + 2;

    frame_begin();
    snprintf(tmpbuf, sizeof(tmpbuf), "%u/%u",
             (unsigned)(view_top + 1), (unsigned)nlines);
    /* tmpbuf is reused by fit(), so print the header in two steps. */
    {
        char pos[16];
        strcpy(pos, tmpbuf);
        draw_header(a->title, pos);
    }

    for (i = 0; i < TEXT_ROWS && view_top + i < nlines; i++) {
        char style = line_text(view_top + i);
        const char *text = linebuf;

        switch (style) {
        case STYLE_HEAD:
            gfx_SetColor(C_ACCENT);
            gfx_FillRectangle_NoClip(4, y + 8, 4 + (int)strlen(text) * 8, 1);
            gfx_SetTextFGColor(C_ACCENT);
            gfx_PrintStringXY(text, 4, y);
            break;

        case STYLE_FORM:
            gfx_SetColor(C_BAND);
            gfx_FillRectangle_NoClip(4, y - 1, SB_X - 8, TEXT_LH);
            gfx_SetColor(C_FORMULA);
            gfx_FillRectangle_NoClip(4, y - 1, 2, TEXT_LH);
            gfx_SetTextFGColor(C_FORMULA);
            gfx_PrintStringXY(text, 12, y);
            break;

        case STYLE_BULLET:
            if (*text == BULLET_CONT) {
                text++;
            } else {
                gfx_SetColor(C_ACCENT);
                gfx_FillRectangle_NoClip(6, y + 3, 3, 3);
            }
            gfx_SetTextFGColor(C_TEXT);
            gfx_PrintStringXY(text, 14, y);
            break;

        default:
            gfx_SetTextFGColor(C_TEXT);
            gfx_PrintStringXY(text, 6, y);
            break;
        }
        y += TEXT_LH;
    }

    draw_scrollbar(view_top, TEXT_ROWS, nlines);
    if (view_top + TEXT_ROWS >= nlines &&
        cur_art + 1 < wiki_categories[cur_cat].num_articles) {
        draw_footer("END  TRACE next article  CLEAR back");
    } else {
        draw_footer("UP/DN scroll  L/R page  CLEAR back");
    }
    frame_end();
}

/* ------------------------------------------------------------------ */
/* Generic list screen                                                 */
/* ------------------------------------------------------------------ */

typedef const char *(*item_fn)(unsigned int index);

static void draw_list(const char *title, unsigned int count,
                      unsigned int sel, unsigned int top,
                      item_fn item, const char *hint)
{
    unsigned int i;
    int y = BODY_Y;

    frame_begin();
    if (count) {
        snprintf(tmpbuf, sizeof(tmpbuf), "%u/%u",
                 (unsigned)(sel + 1), (unsigned)count);
    } else {
        strcpy(tmpbuf, "0");
    }
    {
        char pos[16];
        strcpy(pos, tmpbuf);
        draw_header(title, pos);
    }

    for (i = 0; i < LIST_ROWS && top + i < count; i++) {
        bool on = (top + i == sel);

        if (on) {
            gfx_SetColor(C_SELBG);
            gfx_FillRectangle_NoClip(0, y, SB_X - 2, LIST_LH);
            gfx_SetTextFGColor(C_SELTXT);
            gfx_PrintStringXY(">", 4, y + 3);
        } else {
            gfx_SetTextFGColor(C_TEXT);
        }
        gfx_PrintStringXY(fit(item(top + i), 36), 14, y + 3);
        y += LIST_LH;
    }

    if (!count) {
        gfx_SetTextFGColor(C_GRAY);
        gfx_PrintStringXY("No matches.", 14, BODY_Y + 6);
    }

    draw_scrollbar(top, LIST_ROWS, count);
    draw_footer(hint);
    frame_end();
}

/* Keep sel on screen after a move. */
static void clamp_window(unsigned int sel, unsigned int *top,
                         unsigned int rows, unsigned int count)
{
    if (count == 0) {
        *top = 0;
        return;
    }
    if (sel < *top) {
        *top = sel;
    } else if (sel >= *top + rows) {
        *top = sel - rows + 1;
    }
    if (*top + rows > count) {
        *top = (count > rows) ? count - rows : 0;
    }
}

/* Shared up/down/page handling for a list. Returns true if it consumed. */
static bool list_nav(key_t key, unsigned int *sel, unsigned int *top,
                     unsigned int count)
{
    if (count == 0) {
        return key == K_UP || key == K_DOWN || key == K_LEFT || key == K_RIGHT;
    }

    switch (key) {
    case K_UP:
        *sel = (*sel == 0) ? count - 1 : *sel - 1;
        break;
    case K_DOWN:
        *sel = (*sel + 1 >= count) ? 0 : *sel + 1;
        break;
    case K_LEFT:
        *sel = (*sel > LIST_ROWS) ? *sel - LIST_ROWS : 0;
        break;
    case K_RIGHT:
        *sel += LIST_ROWS;
        if (*sel >= count) {
            *sel = count - 1;
        }
        break;
    case K_WINDOW:
        *sel = 0;
        break;
    case K_GRAPH:
        *sel = count - 1;
        break;
    default:
        return false;
    }
    clamp_window(*sel, top, LIST_ROWS, count);
    return true;
}

/* Jump to the first item starting with c. */
static void jump_to_letter(char c, unsigned int *sel, unsigned int *top,
                           unsigned int count, item_fn item)
{
    unsigned int i;

    for (i = 0; i < count; i++) {
        if (upper(item(i)[0]) == upper(c)) {
            *sel = i;
            clamp_window(*sel, top, LIST_ROWS, count);
            return;
        }
    }
}

/* ------------------------------------------------------------------ */
/* Item accessors                                                      */
/* ------------------------------------------------------------------ */

static char itembuf[48];

static const char *cat_item(unsigned int i)
{
    snprintf(itembuf, sizeof(itembuf), "%-28s %2u",
             wiki_categories[i].title, wiki_categories[i].num_articles);
    return itembuf;
}

static const char *art_item(unsigned int i)
{
    return wiki_categories[cat_sel].articles[i].title;
}

static const char *all_item(unsigned int i)
{
    const artref_t *r = &wiki_index[i];
    return article_at(r->cat, r->art)->title;
}

static const char *res_item(unsigned int i)
{
    const artref_t *r = &wiki_index[results[i]];
    return article_at(r->cat, r->art)->title;
}

/* ------------------------------------------------------------------ */
/* Search                                                              */
/* ------------------------------------------------------------------ */

static void run_search(void)
{
    unsigned int i;

    nresults = 0;
    for (i = 0; i < wiki_index_size; i++) {
        const artref_t *r = &wiki_index[i];
        if (contains(article_at(r->cat, r->art)->title, query) ||
            contains(wiki_categories[r->cat].title, query)) {
            if (nresults < sizeof(results) / sizeof(results[0])) {
                results[nresults++] = (uint16_t)i;
            }
        }
    }
    res_sel = 0;
    res_top = 0;
}

static void draw_search(void)
{
    unsigned int i;
    int y = BODY_Y + 22;

    frame_begin();
    draw_header("Search", NULL);

    /* query box */
    gfx_SetColor(C_RULE);
    gfx_FillRectangle_NoClip(6, BODY_Y + 1, SCR_W - 12, 18);
    gfx_SetColor(C_BG);
    gfx_FillRectangle_NoClip(7, BODY_Y + 2, SCR_W - 14, 16);

    gfx_SetTextFGColor(C_TEXT);
    snprintf(itembuf, sizeof(itembuf), "%s_", query);
    gfx_PrintStringXY(itembuf, 12, BODY_Y + 6);

    gfx_SetTextFGColor(C_GRAY);
    snprintf(itembuf, sizeof(itembuf), "%u found", (unsigned)nresults);
    print_right(itembuf, SCR_W - 12, BODY_Y + 6);

    for (i = 0; i < 12 && res_top + i < nresults; i++) {
        bool on = (res_top + i == res_sel);

        if (on) {
            gfx_SetColor(C_SELBG);
            gfx_FillRectangle_NoClip(0, y, SB_X - 2, LIST_LH);
            gfx_SetTextFGColor(C_SELTXT);
            gfx_PrintStringXY(">", 4, y + 3);
        } else {
            gfx_SetTextFGColor(C_TEXT);
        }
        gfx_PrintStringXY(fit(res_item(res_top + i), 36), 14, y + 3);
        y += LIST_LH;
    }

    if (!nresults) {
        gfx_SetTextFGColor(C_GRAY);
        gfx_PrintStringXY(query_len ? "No matching articles."
                                    : "Type a word using the ALPHA labels.",
                          14, y + 3);
    }

    draw_footer("Type to filter  DEL erase  CLEAR exit");
    frame_end();
}

/* ------------------------------------------------------------------ */
/* Screen handlers                                                     */
/* ------------------------------------------------------------------ */

static void handle_cats(key_t key)
{
    if (list_nav(key, &cat_sel, &cat_top, wiki_num_categories)) {
        return;
    }
    switch (key) {
    case K_ENTER:
    case K_2ND:
        art_sel = 0;
        art_top = 0;
        screen = S_ARTS;
        break;
    case K_YEQU:
        query[0] = '\0';
        query_len = 0;
        run_search();
        screen = S_SEARCH;
        break;
    case K_ZOOM:
        screen = S_ALL;
        break;
    case K_CLEAR:
    case K_MODE:
        running = false;
        break;
    case K_CHAR:
        jump_to_letter(key_char, &cat_sel, &cat_top,
                       wiki_num_categories, cat_item);
        break;
    default:
        break;
    }
}

static void handle_arts(key_t key)
{
    unsigned int count = wiki_categories[cat_sel].num_articles;

    if (list_nav(key, &art_sel, &art_top, count)) {
        return;
    }
    switch (key) {
    case K_ENTER:
    case K_2ND:
        open_article((uint8_t)cat_sel, (uint8_t)art_sel, S_ARTS);
        break;
    case K_CLEAR:
    case K_DEL:
        screen = S_CATS;
        break;
    case K_YEQU:
        query[0] = '\0';
        query_len = 0;
        run_search();
        screen = S_SEARCH;
        break;
    case K_ZOOM:
        screen = S_ALL;
        break;
    case K_MODE:
        running = false;
        break;
    case K_CHAR:
        jump_to_letter(key_char, &art_sel, &art_top, count, art_item);
        break;
    default:
        break;
    }
}

static void handle_all(key_t key)
{
    if (list_nav(key, &all_sel, &all_top, wiki_index_size)) {
        return;
    }
    switch (key) {
    case K_ENTER:
    case K_2ND:
        open_article(wiki_index[all_sel].cat, wiki_index[all_sel].art, S_ALL);
        break;
    case K_CLEAR:
    case K_DEL:
    case K_ZOOM:
        screen = S_CATS;
        break;
    case K_YEQU:
        query[0] = '\0';
        query_len = 0;
        run_search();
        screen = S_SEARCH;
        break;
    case K_MODE:
        running = false;
        break;
    case K_CHAR:
        jump_to_letter(key_char, &all_sel, &all_top,
                       wiki_index_size, all_item);
        break;
    default:
        break;
    }
}

static void handle_search(key_t key)
{
    switch (key) {
    case K_CHAR:
        if (query_len < sizeof(query) - 1) {
            query[query_len++] = key_char;
            query[query_len] = '\0';
            run_search();
        }
        break;
    case K_DEL:
        if (query_len) {
            query[--query_len] = '\0';
            run_search();
        }
        break;
    case K_UP:
        if (nresults) {
            res_sel = (res_sel == 0) ? nresults - 1 : res_sel - 1;
            clamp_window(res_sel, &res_top, 12, nresults);
        }
        break;
    case K_DOWN:
        if (nresults) {
            res_sel = (res_sel + 1 >= nresults) ? 0 : res_sel + 1;
            clamp_window(res_sel, &res_top, 12, nresults);
        }
        break;
    case K_ENTER:
    case K_2ND:
        if (nresults) {
            const artref_t *r = &wiki_index[results[res_sel]];
            open_article(r->cat, r->art, S_SEARCH);
        }
        break;
    case K_CLEAR:
        screen = S_CATS;
        break;
    case K_MODE:
        running = false;
        break;
    default:
        break;
    }
}

static void handle_view(key_t key)
{
    unsigned int max_top = (nlines > TEXT_ROWS) ? nlines - TEXT_ROWS : 0;

    switch (key) {
    case K_UP:
        if (view_top) {
            view_top--;
        }
        break;
    case K_DOWN:
        if (view_top < max_top) {
            view_top++;
        }
        break;
    case K_LEFT:
        view_top = (view_top > TEXT_ROWS) ? view_top - TEXT_ROWS : 0;
        break;
    case K_RIGHT:
        view_top += TEXT_ROWS;
        if (view_top > max_top) {
            view_top = max_top;
        }
        break;
    case K_WINDOW:
        view_top = 0;
        break;
    case K_GRAPH:
        view_top = max_top;
        break;
    case K_TRACE:  /* next article in the same category */
        if (cur_art + 1 < wiki_categories[cur_cat].num_articles) {
            open_article(cur_cat, cur_art + 1, view_from);
        }
        break;
    case K_CLEAR:
    case K_DEL:
        screen = view_from;
        break;
    case K_YEQU:
        query[0] = '\0';
        query_len = 0;
        run_search();
        screen = S_SEARCH;
        break;
    case K_MODE:
        running = false;
        break;
    default:
        break;
    }
}

/* ------------------------------------------------------------------ */
/* Main                                                                */
/* ------------------------------------------------------------------ */

static void render(void)
{
    switch (screen) {
    case S_CATS:
        draw_list(WIKI_TITLE, wiki_num_categories, cat_sel, cat_top,
                  cat_item, "2ND open  Y= search  ZOOM all  CLEAR");
        break;

    case S_ARTS:
        draw_list(wiki_categories[cat_sel].title,
                  wiki_categories[cat_sel].num_articles,
                  art_sel, art_top, art_item,
                  "2ND open  CLEAR back  Y= search");
        break;

    case S_ALL:
        draw_list("All Articles A-Z", wiki_index_size, all_sel, all_top,
                  all_item, "2ND open  A-Z jump  CLEAR back");
        break;

    case S_SEARCH:
        draw_search();
        break;

    case S_VIEW:
        draw_view();
        break;
    }
}

int main(void)
{
    gfx_Begin();
    init_palette();
    gfx_SetDrawBuffer();
    gfx_SetMonospaceFont(8);
    gfx_SetTextTransparentColor(C_NONE);
    gfx_SetTextBGColor(C_NONE);

    while (running) {
        key_t key;

        render();
        key = get_key();

        switch (screen) {
        case S_CATS:   handle_cats(key);   break;
        case S_ARTS:   handle_arts(key);   break;
        case S_ALL:    handle_all(key);    break;
        case S_SEARCH: handle_search(key); break;
        case S_VIEW:   handle_view(key);   break;
        }
    }

    gfx_End();
    os_ClrHome();
    return 0;
}
