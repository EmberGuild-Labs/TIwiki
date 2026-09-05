/* Host stub for graphx.h: renders the app into an ASCII grid. See sim.c. */
#ifndef HOSTSIM_GRAPHX_H
#define HOSTSIM_GRAPHX_H

#include <stdint.h>

#define SIM_COLS 40
#define SIM_ROWS 60

extern char sim_grid[SIM_ROWS][SIM_COLS + 1];
extern uint16_t sim_palette[256];

#define gfx_palette sim_palette
#define gfx_RGBTo1555(r, g, b) \
    ((uint16_t)(((((r) & 255) >> 3) << 10) | ((((g) & 255) >> 3) << 5) | (((b) & 255) >> 3)))

void gfx_Begin(void);
void gfx_End(void);
void gfx_SetDrawBuffer(void);
void gfx_SwapDraw(void);
void gfx_FillScreen(uint8_t color);
void gfx_SetColor(uint8_t color);
void gfx_FillRectangle_NoClip(int x, int y, int w, int h);
uint8_t gfx_SetTextFGColor(uint8_t color);
uint8_t gfx_SetTextBGColor(uint8_t color);
uint8_t gfx_SetTextTransparentColor(uint8_t color);
void gfx_SetMonospaceFont(uint8_t spacing);
void gfx_PrintStringXY(const char *string, int x, int y);

#endif
