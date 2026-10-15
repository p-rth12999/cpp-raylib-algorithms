#include "raylib.h"
#include "dda.h"
#include "clipping.h"
using namespace std;

// ============================================================
// ASSIGNMENT 5B - LIANG-BARSKY LINE CLIPPING
// The line is written as  P(t) = P1 + t (P2 - P1),  0 <= t <= 1.
// For each window edge:   p = -dx, dx, -dy, dy     q = distance to the edge
//   p < 0 : the line ENTERS across this edge  -> t0 = max(t0, q/p)
//   p > 0 : the line LEAVES across this edge  -> t1 = min(t1, q/p)
//   p = 0 : parallel; outside if q < 0
// If t0 > t1 the line is rejected, else draw P(t0) .. P(t1).
//
//   MOUSE : the bright line runs from the anchor point to the mouse
//   LEFT CLICK : move the anchor point
//   ARROWS : move the window     [ ] : resize the window     R : reset
// ============================================================
const int W = 1900;
const int H = 1000;

struct Seg
{
    float x1, y1, x2, y2;
};

const Seg presets[8] =
{
    {  700, 400, 1200, 600 },
    {  300, 200, 1600, 800 },
    {  100, 500, 1800, 500 },
    {  950, 100,  950, 900 },
    {  200, 150,  500, 250 },
    { 1500, 150, 1700, 400 },
    {  400, 800,  900, 250 },
    { 1000, 650, 1500, 900 }
};

void drawWindow(const ClipWindow& w)
{
    ddaLine((int)w.xmin, (int)w.ymin, (int)w.xmax, (int)w.ymin, YELLOW);
    ddaLine((int)w.xmax, (int)w.ymin, (int)w.xmax, (int)w.ymax, YELLOW);
    ddaLine((int)w.xmax, (int)w.ymax, (int)w.xmin, (int)w.ymax, YELLOW);
    ddaLine((int)w.xmin, (int)w.ymax, (int)w.xmin, (int)w.ymin, YELLOW);
}

void drawClipped(Seg s, const ClipWindow& w, Color colour)
{
    ddaLine((int)s.x1, (int)s.y1, (int)s.x2, (int)s.y2, Color{ 70, 70, 70, 255 });

    float a = s.x1, b = s.y1, c = s.x2, d = s.y2;
    if (liangBarsky(a, b, c, d, w) != CLIP_REJECTED)
        ddaLine((int)a, (int)b, (int)c, (int)d, colour);
}

void drawCross(float x, float y, Color c)
{
    ddaLine((int)x - 8, (int)y, (int)x + 8, (int)y, c);
    ddaLine((int)x, (int)y - 8, (int)x, (int)y + 8, c);
}

int main()
{
    InitWindow(W, H, "Assignment 5B - Liang-Barsky Line Clipping");
    SetTargetFPS(60);

    const ClipWindow startWin = { 600, 300, 1300, 700 };
    ClipWindow win = startWin;
    float ax = 300, ay = 850;

    while (!WindowShouldClose())
    {
        if (IsKeyDown(KEY_RIGHT)) { win.xmin += 6; win.xmax += 6; }
        if (IsKeyDown(KEY_LEFT))  { win.xmin -= 6; win.xmax -= 6; }
        if (IsKeyDown(KEY_DOWN))  { win.ymin += 6; win.ymax += 6; }
        if (IsKeyDown(KEY_UP))    { win.ymin -= 6; win.ymax -= 6; }
        if (IsKeyPressed(KEY_LEFT_BRACKET) && win.xmax - win.xmin > 160 && win.ymax - win.ymin > 120)
        {
            win.xmin += 20; win.xmax -= 20; win.ymin += 15; win.ymax -= 15;
        }
        if (IsKeyPressed(KEY_RIGHT_BRACKET)) { win.xmin -= 20; win.xmax += 20; win.ymin -= 15; win.ymax += 15; }
        if (IsKeyPressed(KEY_R)) win = startWin;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { ax = (float)GetMouseX(); ay = (float)GetMouseY(); }

        Seg m = { ax, ay, (float)GetMouseX(), (float)GetMouseY() };

        BeginDrawing();
        ClearBackground(BLACK);

        drawWindow(win);
        for (int i = 0; i < 8; i++)
            drawClipped(presets[i], win, GREEN);

        // ---- the mouse line, with the numbers shown on screen ----
        ddaLine((int)m.x1, (int)m.y1, (int)m.x2, (int)m.y2, Color{ 70, 70, 70, 255 });

        float a = m.x1, b = m.y1, c = m.x2, d = m.y2;
        float t0 = 0, t1 = 1;
        ClipResult r = liangBarsky(a, b, c, d, win, &t0, &t1);

        if (r != CLIP_REJECTED)
        {
            ddaLine((int)a, (int)b, (int)c, (int)d, SKYBLUE);
            drawCross(a, b, ORANGE);       // P(t0)
            drawCross(c, d, ORANGE);       // P(t1)
        }

        float dx = m.x2 - m.x1, dy = m.y2 - m.y1;
        float p[4] = { -dx, dx, -dy, dy };
        float q[4] = { m.x1 - win.xmin, win.xmax - m.x1, m.y1 - win.ymin, win.ymax - m.y1 };
        const char* edge[4] = { "left  ", "right ", "top   ", "bottom" };

        const char* names[3] = { "REJECTED", "ACCEPTED", "CLIPPED" };
        DrawText("LIANG-BARSKY LINE CLIPPING", 30, 20, 40, WHITE);
        DrawText(TextFormat("result: %s     t0 = %.3f     t1 = %.3f", names[r], t0, t1), 30, 70, 28, GREEN);

        for (int k = 0; k < 4; k++)
        {
            if (p[k] == 0)
                DrawText(TextFormat("%s   p = %8.1f   q = %8.1f   parallel", edge[k], p[k], q[k]), 30, 112 + k * 30, 24, YELLOW);
            else
                DrawText(TextFormat("%s   p = %8.1f   q = %8.1f   q/p = %7.3f   (%s)", edge[k], p[k], q[k], q[k] / p[k],
                                    p[k] < 0 ? "enters" : "leaves"), 30, 112 + k * 30, 24, YELLOW);
        }

        DrawText("grey = original line     blue = kept part     orange crosses = P(t0) and P(t1)", 30, 240, 22, GRAY);
        DrawText("MOUSE: move the line    CLICK: set anchor    ARROWS: move window    [ ]: resize    R: reset", 30, 965, 22, GRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
