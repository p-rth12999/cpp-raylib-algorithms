#include "raylib.h"
#include "canvas.h"
#include "dda.h"
#include "bresenham_circle.h"
#include "fill.h"
#include <vector>
using namespace std;

// ============================================================
// ASSIGNMENT 4A - FLOOD FILL
// Replaces every connected pixel that has the SAME COLOUR as the
// clicked pixel. It stops where the colour changes, not at an outline.
//
//   LEFT CLICK : fill the region under the mouse
//   C : next colour     T : 4-connected / 8-connected     R : reset
// ============================================================
const int W = 1900;
const int H = 1000;

void drawRect(int x0, int y0, int x1, int y1)
{
    ddaLine(x0, y0, x1, y0, WHITE);
    ddaLine(x1, y0, x1, y1, WHITE);
    ddaLine(x1, y1, x0, y1, WHITE);
    ddaLine(x0, y1, x0, y0, WHITE);
}

void drawScene()
{
    canvasClear(BLACK);

    // 1. nested rectangles: a ring and an inner square
    drawRect(120, 260, 520, 660);
    drawRect(220, 360, 420, 560);

    // 2. two-colour rectangle: left half is already blue, right half is black.
    //    There is NO divider line between them.
    scanLineFill({ { 621, 261 }, { 820, 261 }, { 820, 660 }, { 621, 660 } }, DARKBLUE);
    drawRect(620, 260, 1020, 660);

    // 3. circle
    bresenham_circle(1230, 460, 180, WHITE);

    // 4. diamond: its edges are diagonal, so they are only 8-connected
    ddaLine(1650, 260, 1850, 460, WHITE);
    ddaLine(1850, 460, 1650, 660, WHITE);
    ddaLine(1650, 660, 1450, 460, WHITE);
    ddaLine(1450, 460, 1650, 260, WHITE);
}

int main()
{
    InitWindow(W, H, "Assignment 4A - Flood Fill");
    SetTargetFPS(60);

    canvasEnable(W, H);
    drawScene();

    Color palette[6] = { RED, GREEN, GOLD, ORANGE, PURPLE, SKYBLUE };
    int colorIdx = 0;
    bool eight = false;
    int lastCount = 0;
    double lastMs = 0;

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_C)) colorIdx = (colorIdx + 1) % 6;
        if (IsKeyPressed(KEY_T)) eight = !eight;
        if (IsKeyPressed(KEY_R)) { drawScene(); lastCount = 0; lastMs = 0; }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            int mx = GetMouseX();
            int my = GetMouseY();
            Color oldColor = canvasGet(mx, my);     // the colour being replaced

            double t0 = GetTime();
            lastCount = eight ? floodFill8(mx, my, oldColor, palette[colorIdx])
                              : floodFill4(mx, my, oldColor, palette[colorIdx]);
            lastMs = (GetTime() - t0) * 1000.0;
        }

        BeginDrawing();
        ClearBackground(BLACK);
        canvasPresent(0, 0);

        DrawText("FLOOD FILL", 30, 20, 40, WHITE);
        DrawText(TextFormat("connectivity: %s     last fill: %d pixels in %.1f ms",
                            eight ? "8-connected" : "4-connected", lastCount, lastMs), 30, 70, 28, YELLOW);
        DrawText("fill colour:", 30, 115, 28, WHITE);
        DrawRectangle(200, 112, 60, 34, palette[colorIdx]);
        DrawText("LEFT CLICK: fill    C: colour    T: 4 / 8-connected    R: reset", 30, 960, 24, GRAY);

        DrawText("1. Click the ring, then the inner square", 120, 690, 22, GRAY);
        DrawText("2. Blue and black halves are separate", 620, 690, 22, GRAY);
        DrawText("regions: flood fill follows COLOUR", 620, 718, 22, GRAY);
        DrawText("3. Circle", 1120, 690, 22, GRAY);
        DrawText("4. Press T for 8-connected, then", 1450, 690, 22, GRAY);
        DrawText("click inside: it leaks out!", 1450, 718, 22, RED);
        EndDrawing();
    }

    canvasShutdown();
    CloseWindow();
    return 0;
}
