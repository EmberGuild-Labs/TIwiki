/*
 * Host-side simulator for MATHWIKI.
 *
 * Compiles the real src/main.c against stub graphx/keypadc headers so the
 * navigation, search, wrapping and scrolling logic can be exercised on a
 * desktop. Text drawn by the app lands in an ASCII grid (col = x/8,
 * row = y/4) which is printed once per frame.
 *
 *   ./sim "down down enter enter down down"
 *
 * Tokens: up down left right enter 2nd clear del mode y= window zoom trace
 * graph, a single letter a-z, or _ for a space.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graphx.h"
#include "keypadc.h"

char sim_grid[SIM_ROWS][SIM_COLS + 1];
uint16_t sim_palette[256];
uint8_t sim_kb[8];

int wiki_main(void);

static int frame_no;
static int frames_wanted = -1;   /* -1 = print every frame */

/* ---------------- graphics stubs ---------------- */

static void grid_clear(void)
{
    int r, c;
    for (r = 0; r < SIM_ROWS; r++) {
        for (c = 0; c < SIM_COLS; c++) {
            sim_grid[r][c] = ' ';
        }
        sim_grid[r][SIM_COLS] = '\0';
    }
}

void gfx_Begin(void) { grid_clear(); }
void gfx_End(void) {}
void gfx_SetDrawBuffer(void) {}
void gfx_SetColor(uint8_t c) { (void)c; }
void gfx_FillRectangle_NoClip(int x, int y, int w, int h)
{
    (void)x; (void)y; (void)w; (void)h;
}
uint8_t gfx_SetTextFGColor(uint8_t c) { return c; }
uint8_t gfx_SetTextBGColor(uint8_t c) { return c; }
uint8_t gfx_SetTextTransparentColor(uint8_t c) { return c; }
void gfx_SetMonospaceFont(uint8_t s) { (void)s; }
void gfx_FillScreen(uint8_t c) { (void)c; grid_clear(); }
void os_ClrHome(void) {}
void delay(unsigned short ms) { (void)ms; }

void gfx_PrintStringXY(const char *s, int x, int y)
{
    int row = y / 4;
    int col = x / 8;

    if (row < 0 || row >= SIM_ROWS) {
        return;
    }
    for (; *s && col < SIM_COLS; s++, col++) {
        unsigned char ch = (unsigned char)*s;
        if (col < 0) {
            continue;
        }
        switch (ch) {              /* TI glyphs -> ASCII */
        case 0x10: ch = '>'; break;
        case 0x18: ch = '^'; break;
        case 0x19: ch = 'v'; break;
        case 0x1A: ch = '<'; break;
        default:
            if (ch < 32 || ch > 126) {
                ch = '?';
            }
            break;
        }
        sim_grid[row][col] = (char)ch;
    }
}

void gfx_SwapDraw(void)
{
    int r, c, last = -1;

    frame_no++;
    if (frames_wanted >= 0 && frame_no != frames_wanted) {
        return;
    }

    for (r = SIM_ROWS - 1; r >= 0; r--) {
        for (c = 0; c < SIM_COLS; c++) {
            if (sim_grid[r][c] != ' ') {
                break;
            }
        }
        if (c < SIM_COLS) {
            last = r;
            break;
        }
    }

    printf("\n+--- frame %d %s\n", frame_no,
           "--------------------------------------");
    for (r = 0; r <= last; r++) {
        int blank = 1;
        for (c = 0; c < SIM_COLS; c++) {
            if (sim_grid[r][c] != ' ') {
                blank = 0;
                break;
            }
        }
        if (blank) {
            continue;
        }
        for (c = SIM_COLS - 1; c >= 0 && sim_grid[r][c] == ' '; c--) {
            sim_grid[r][c] = '\0';
        }
        printf("|%s\n", sim_grid[r]);
    }
}

/* ---------------- keypad script ---------------- */

typedef struct {
    const char *name;
    uint8_t group;
    uint8_t mask;
} named_key_t;

static const named_key_t named[] = {
    { "up",     7, kb_Up },
    { "down",   7, kb_Down },
    { "left",   7, kb_Left },
    { "right",  7, kb_Right },
    { "enter",  6, kb_Enter },
    { "2nd",    1, kb_2nd },
    { "clear",  6, kb_Clear },
    { "del",    1, kb_Del },
    { "mode",   1, kb_Mode },
    { "y=",     1, kb_Yequ },
    { "window", 1, kb_Window },
    { "zoom",   1, kb_Zoom },
    { "trace",  1, kb_Trace },
    { "graph",  1, kb_Graph },
};

/* ALPHA labels, same mapping as the app's keytab. */
static const named_key_t letters[] = {
    { "a", 2, kb_Math },   { "b", 3, kb_Apps },   { "c", 4, kb_Prgm },
    { "d", 2, kb_Recip },  { "e", 3, kb_Sin },    { "f", 4, kb_Cos },
    { "g", 5, kb_Tan },    { "h", 6, kb_Power },  { "i", 2, kb_Square },
    { "j", 3, kb_Comma },  { "k", 4, kb_LParen }, { "l", 5, kb_RParen },
    { "m", 6, kb_Div },    { "n", 2, kb_Log },    { "o", 3, kb_7 },
    { "p", 4, kb_8 },      { "q", 5, kb_9 },      { "r", 6, kb_Mul },
    { "s", 2, kb_Ln },     { "t", 3, kb_4 },      { "u", 4, kb_5 },
    { "v", 5, kb_6 },      { "w", 6, kb_Sub },    { "x", 2, kb_Sto },
    { "y", 3, kb_1 },      { "z", 4, kb_2 },      { "_", 3, kb_0 },
};

#define MAX_SCRIPT 8192
static uint8_t script_group[MAX_SCRIPT];
static uint8_t script_mask[MAX_SCRIPT];
static int script_len, script_pos, releasing;

static void script_add(const char *tok)
{
    size_t i;

    for (i = 0; i < sizeof(named) / sizeof(named[0]); i++) {
        if (!strcmp(tok, named[i].name)) {
            script_group[script_len] = named[i].group;
            script_mask[script_len++] = named[i].mask;
            return;
        }
    }
    for (i = 0; i < sizeof(letters) / sizeof(letters[0]); i++) {
        if (!strcmp(tok, letters[i].name)) {
            script_group[script_len] = letters[i].group;
            script_mask[script_len++] = letters[i].mask;
            return;
        }
    }
    fprintf(stderr, "sim: unknown key token '%s'\n", tok);
    exit(2);
}

void kb_Scan(void)
{
    memset(sim_kb, 0, sizeof(sim_kb));

    if (releasing) {
        releasing = 0;
        return;
    }
    if (script_pos >= script_len) {
        sim_kb[1] = kb_Mode;          /* end of script: quit the app */
        return;
    }
    sim_kb[script_group[script_pos]] = script_mask[script_pos];
    script_pos++;
    releasing = 1;
}

/* ---------------- driver ---------------- */

int main(int argc, char **argv)
{
    int i;

    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--frame") && i + 1 < argc) {
            frames_wanted = atoi(argv[++i]);
            continue;
        }
        {
            char *copy = strdup(argv[i]);
            char *tok = strtok(copy, " \t\n");
            while (tok) {
                if (script_len >= MAX_SCRIPT) {
                    fprintf(stderr, "sim: script too long\n");
                    return 2;
                }
                script_add(tok);
                tok = strtok(NULL, " \t\n");
            }
            free(copy);
        }
    }

    wiki_main();
    printf("\n(%d frames, %d/%d script keys consumed)\n",
           frame_no, script_pos, script_len);
    return 0;
}
