#include "raylib.h"
#include "dda.h"
#include "clipping.h"
#include <string>
using namespace std;

// ============================================================
// ASSIGNMENT 5A - COHEN-SUTHERLAND LINE CLIPPING
// Every end point gets a 4-bit region code (T B R L).
//   both codes 0000           -> trivially ACCEPTED
//   (code1 AND code2) != 0    -> trivially REJECTED
//   otherwise                 -> move an outside end point onto a window
//                                edge and repeat
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
    {  700, 400, 1200, 600 },    // inside
    {  300, 200, 1600, 800 },    // crosses the window
    {  100, 500, 1800, 500 },    // horizontal, crosses both sides
    {  950, 100,  950, 900 },    // vertical, crosses top and bottom
    {  200, 150,  500, 250 },    // outside, same side -> rejected
    { 1500, 150, 1700, 400 },    // outside, same side -> rejected
    {  400, 800,  900, 250 },    // cuts a corner
    { 1000, 650, 1500, 900 }     // starts inside, ends outside
};

string codeString(int code)
{
    string s = "";
    s += (code & CLIP_TOP)    ? '1' : '0';
    s += (code & CLIP_BOTTOM) ? '1' : '0';
    s += (code & CLIP_RIGHT)  ? '1' : '0';
    s += (code & CLIP_LEFT)   ? '1' : '0';
    return s;
}

void drawWindow(const ClipWindow& w)
{
    int x0 = (int)w.xmin, y0 = (int)w.ymin, x1 = (int)w.xmax, y1 = (int)w.ymax;

    // the 4 edges extended across the screen split it into 9 regions
    ddaLine(x0, 0, x0, H - 1, Color{ 60, 60, 60, 255 });
    ddaLine(x1, 0, x1, H - 1, Color{ 60, 60, 60, 255 });
    ddaLine(0, y0, W - 1, y0, Color{ 60, 60, 60, 255 });
    ddaLine(0, y1, W - 1, y1, Color{ 60, 60, 60, 255 });

    ddaLine(x0, y0, x1, y0, YELLOW);
    ddaLine(x1, y0, x1, y1, YELLOW);
    ddaLine(x1, y1, x0, y1, YELLOW);
    ddaLine(x0, y1, x0, y0, YELLOW);

    // region code of every one of the 9 regions
    float xs[4] = { 0, w.xmin, w.xmax, (float)W };
    float ys[4] = { 0, w.ymin, w.ymax, (float)H };
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
        {
            float cx = (xs[i] + xs[i + 1]) / 2;
            float cy = (ys[j] + ys[j + 1]) / 2;
            if (cy < 210 || cy > 950) continue;      // keep clear of the text
            DrawText(codeString(clipOutcode(cx, cy, w)).c_str(), (int)cx - 28, (int)cy - 10, 22, Color{ 110, 110, 110, 255 });
        }
}

// draw the original in grey and what survives clipping in colour
ClipResult drawClipped(Seg s, const ClipWindow& w, Color colour, int* steps = nullptr)
{
    ddaLine((int)s.x1, (int)s.y1, (int)s.x2, (int)s.y2, Color{ 70, 70, 70, 255 });

    float a = s.x1, b = s.y1, c = s.x2, d = s.y2;
    ClipResult r = cohenSutherland(a, b, c, d, w, steps);

    if (r != CLIP_REJECTED)
        ddaLine((int)a, (int)b, (int)c, (int)d, colour);
    return r;
}

int main()
{
    InitWindow(W, H, "Assignment 5A - Cohen-Sutherland Line Clipping");
    SetTargetFPS(60);

    const ClipWindow startWin = { 600, 300, 1300, 700 };
    ClipWindow win = startWin;
    float ax = 300, ay = 850;     // anchor point of the mouse line

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

        Seg mouseLine = { ax, ay, (float)GetMouseX(), (float)GetMouseY() };

        BeginDrawing();
        ClearBackground(BLACK);

        drawWindow(win);

        for (int i = 0; i < 8; i++)
            drawClipped(presets[i], win, GREEN);

        int steps = 0;
        ClipResult r = drawClipped(mouseLine, win, SKYBLUE, &steps);

        int c1 = clipOutcode(mouseLine.x1, mouseLine.y1, win);
        int c2 = clipOutcode(mouseLine.x2, mouseLine.y2, win);
        DrawText(codeString(c1).c_str(), (int)mouseLine.x1 + 10, (int)mouseLine.y1 + 10, 24, ORANGE);
        DrawText(codeString(c2).c_str(), (int)mouseLine.x2 + 10, (int)mouseLine.y2 + 10, 24, ORANGE);

        const char* names[3] = { "REJECTED", "ACCEPTED", "CLIPPED" };
        DrawText("COHEN-SUTHERLAND LINE CLIPPING", 30, 20, 40, WHITE);
        DrawText(TextFormat("blue line: P1 code %s   P2 code %s   AND = %s   OR = %s",
                            codeString(c1).c_str(), codeString(c2).c_str(),
                            codeString(c1 & c2).c_str(), codeString(c1 | c2).c_str()), 30, 70, 26, YELLOW);
        DrawText(TextFormat("result: %s     edge intersections calculated: %d", names[r], steps), 30, 108, 26, GREEN);
        DrawText("code bits: T B R L     grey = original line     coloured = kept part", 30, 146, 22, GRAY);
        DrawText("MOUSE: move the line    CLICK: set anchor    ARROWS: move window    [ ]: resize    R: reset", 30, 965, 22, GRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
