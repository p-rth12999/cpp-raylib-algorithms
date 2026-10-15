#include "raylib.h"
#include "canvas.h"
#include "dda.h"
#include "bresenham_circle.h"
#include "fill.h"
#include <cmath>
using namespace std;

// ============================================================
// ASSIGNMENT 4B - BOUNDARY FILL
// Spreads from the clicked pixel and stops when it meets the
// BOUNDARY COLOUR (whatever is inside gets overwritten).
//
//   LEFT CLICK : fill the region under the mouse
//   C : next colour     B : boundary colour WHITE / RED
//   T : 4-connected / 8-connected     R : reset
// ============================================================
const int W = 1900;
const int H = 1000;

void drawRect(int x0, int y0, int x1, int y1, Color c)
{
    ddaLine(x0, y0, x1, y0, c);
    ddaLine(x1, y0, x1, y1, c);
    ddaLine(x1, y1, x0, y1, c);
    ddaLine(x0, y1, x0, y0, c);
}

void drawScene()
{
    canvasClear(BLACK);

    // 1. circle
    bresenham_circle(250, 500, 170, WHITE);

    // 2. donut: outer circle + inner square (click the ring, then the square)
    bresenham_circle(720, 500, 200, WHITE);
    drawRect(630, 410, 810, 590, WHITE);

    // 3. pentagram: 6 separate regions (centre pentagon + 5 points)
    int px[5], py[5];
    for (int k = 0; k < 5; k++)
    {
        float a = (-90 + 72 * k) * DEG2RAD;
        px[k] = 1190 + (int)roundf(200 * cosf(a));
        py[k] = 520 + (int)roundf(200 * sinf(a));
    }
    for (int k = 0; k < 5; k++)
        ddaLine(px[k], py[k], px[(k + 2) % 5], py[(k + 2) % 5], WHITE);

    // 4. square whose TOP edge is red: the outline has two colours
    ddaLine(1490, 330, 1830, 330, RED);
    ddaLine(1830, 330, 1830, 670, WHITE);
    ddaLine(1830, 670, 1490, 670, WHITE);
    ddaLine(1490, 670, 1490, 330, WHITE);
}

int main()
{
    InitWindow(W, H, "Assignment 4B - Boundary Fill");
    SetTargetFPS(60);

    canvasEnable(W, H);
    drawScene();

    Color palette[6] = { RED, GREEN, GOLD, ORANGE, PURPLE, SKYBLUE };
    int colorIdx = 1;
    bool eight = false;
    bool boundaryRed = false;
    int lastCount = 0;
    double lastMs = 0;

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_C)) colorIdx = (colorIdx + 1) % 6;
        if (IsKeyPressed(KEY_T)) eight = !eight;
        if (IsKeyPressed(KEY_B)) boundaryRed = !boundaryRed;
        if (IsKeyPressed(KEY_R)) { drawScene(); lastCount = 0; lastMs = 0; }

        Color boundary = boundaryRed ? RED : WHITE;

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            int mx = GetMouseX();
            int my = GetMouseY();

            double t0 = GetTime();
            lastCount = eight ? boundaryFill8(mx, my, palette[colorIdx], boundary)
                              : boundaryFill4(mx, my, palette[colorIdx], boundary);
            lastMs = (GetTime() - t0) * 1000.0;
        }

        BeginDrawing();
        ClearBackground(BLACK);
        canvasPresent(0, 0);

        DrawText("BOUNDARY FILL", 30, 20, 40, WHITE);
        DrawText(TextFormat("connectivity: %s     boundary colour: %s     last fill: %d pixels in %.1f ms",
                            eight ? "8-connected" : "4-connected", boundaryRed ? "RED" : "WHITE", lastCount, lastMs),
                 30, 70, 28, YELLOW);
        DrawText("fill colour:", 30, 115, 28, WHITE);
        DrawRectangle(200, 112, 60, 34, palette[colorIdx]);
        DrawText("LEFT CLICK: fill    C: colour    B: boundary colour    T: 4 / 8-connected    R: reset", 30, 960, 24, GRAY);

        DrawText("1. Circle", 190, 720, 22, GRAY);
        DrawText("2. Donut: ring, then square", 590, 720, 22, GRAY);
        DrawText("3. Pentagram: 6 regions", 1060, 720, 22, GRAY);
        DrawText("4. Two-colour outline:", 1490, 720, 22, GRAY);
        DrawText("one boundary colour can't", 1490, 748, 22, RED);
        DrawText("stop the fill on every side", 1490, 776, 22, RED);
        EndDrawing();
    }

    canvasShutdown();
    CloseWindow();
    return 0;
}
