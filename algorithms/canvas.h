#ifndef CANVAS_H
#define CANVAS_H

#include "raylib.h"
#include <vector>

// ============================================================
// CANVAS - the pixel surface every drawing algorithm plots to
//
//   Immediate mode (default) : plot() calls DrawPixel.
//   Buffered mode            : plot() writes into a software pixel
//                              buffer that can be READ back. Fill
//                              algorithms need this, because raylib
//                              cannot read pixels from the screen.
// ============================================================

void   canvasEnable(int width, int height);   // switch to buffered mode (call after InitWindow)
void   canvasDisable();                       // back to immediate mode
void   canvasShutdown();                      // free the texture (call before CloseWindow)

bool   canvasEnabled();
int    canvasWidth();
int    canvasHeight();

void   canvasClear(Color color);
void   plot(int x, int y, Color color);       // used by all line / circle / fill algorithms
Color  canvasGet(int x, int y);               // read a pixel (buffered mode only)
Color* canvasPixels();                        // raw pixel array, row-major

void   canvasPresent(int x, int y);           // draw the buffer (between BeginDrawing / EndDrawing)

std::vector<Color> canvasSnapshot();                          // copy the buffer (e.g. a static background)
void               canvasRestore(const std::vector<Color>&);  // paste it back

bool   sameColor(Color a, Color b);           // compares r, g, b

#endif
