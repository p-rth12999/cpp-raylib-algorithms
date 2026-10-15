#ifndef FILL_H
#define FILL_H

#include "raylib.h"
#include "transform2d.h"
#include <vector>
#include <climits>

// ============================================================
// FILL ALGORITHMS - Assignment 4
// All of them work on the canvas in BUFFERED mode (canvasEnable),
// because they have to read pixels back. No recursion is used, so
// large regions cannot overflow the stack.
// ============================================================

// FLOOD FILL: replaces every connected pixel of oldColor with newColor.
// Returns the number of pixels changed.
int floodFill4(int x, int y, Color oldColor, Color newColor);   // 4-connected (N,S,E,W)
int floodFill8(int x, int y, Color oldColor, Color newColor);   // 8-connected (+ diagonals)

// BOUNDARY FILL: spreads until it meets boundaryColor.
// maxPixels > 0 is a safety net: if the outline leaks and the fill grows
// bigger than maxPixels, everything is undone and -1 is returned.
// Otherwise returns the number of pixels changed.
int boundaryFill4(int x, int y, Color fillColor, Color boundaryColor, int maxPixels = 0);
int boundaryFill8(int x, int y, Color fillColor, Color boundaryColor, int maxPixels = 0);

// SCAN-LINE FILL: x positions where the polygon edges cross row y (sorted),
// and the fill itself. Works for concave polygons. Only rows y < yLimit are
// filled, which lets a demo animate the scan line.
std::vector<float> scanLineCrossings(const std::vector<Vec2>& poly, int y);
void scanLineFill(const std::vector<Vec2>& poly, Color color, int yLimit = INT_MAX);

#endif
