#include "raylib.h"
#include "canvas.h"
#include "dda.h"
#include "fill.h"
#include <cmath>
#include <algorithm>
#include <vector>
#include <string>
using namespace std;

// ============================================================
// ASSIGNMENT 4C - SCAN-LINE POLYGON FILL
// The yellow line sweeps down the polygon. On every row the edge
// crossings (red ticks) are sorted and the pixels BETWEEN each pair
// are filled.
//
//   1 triangle   2 concave arrow   3 star   4 draw your own
//   (mode 4)  LEFT CLICK: add vertex   BACKSPACE: undo   ENTER: close + fill
//   SPACE : replay     UP / DOWN : sweep speed     C : clear
// ============================================================
const int W = 1900;
const int H = 1000;

vector<Vec2> makePreset(int id)
{
    if (id == 1)
        return { { 950, 300 }, { 1250, 800 }, { 650, 800 } };

    if (id == 2)   // concave arrow
        return { { 600, 420 }, { 1000, 420 }, { 1000, 300 }, { 1300, 560 },
                 { 1000, 820 }, { 1000, 700 }, { 600, 700 } };

    vector<Vec2> star;   // 10-vertex star
    for (int k = 0; k < 10; k++)
    {
        float r = (k % 2 == 0) ? 300.0f : 125.0f;
        float a = (-90 + 36 * k) * DEG2RAD;
        star.push_back({ 950 + r * cosf(a), 560 + r * sinf(a) });
    }
    return star;
}

int main()
{
    InitWindow(W, H, "Assignment 4C - Scan-Line Polygon Fill");
    SetTargetFPS(60);

    canvasEnable(W, H);

    int mode = 2;
    vector<Vec2> poly = makePreset(mode);
    bool closed = true;
    float speed = 300;     // rows per second
    float scanY = 0;

    auto restart = [&]()
    {
        float minY = 99999;
        for (const Vec2& p : poly) minY = min(minY, p.y);
        scanY = minY;
    };
    restart();

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        for (int k = 1; k <= 3; k++)
            if (IsKeyPressed(KEY_ONE + k - 1)) { mode = k; poly = makePreset(k); closed = true; restart(); }
        if (IsKeyPressed(KEY_FOUR)) { mode = 4; poly.clear(); closed = false; }
        if (IsKeyPressed(KEY_C))    { mode = 4; poly.clear(); closed = false; }
        if (IsKeyPressed(KEY_SPACE) && closed) restart();
        if (IsKeyDown(KEY_UP))   speed = min(2000.0f, speed + 300 * dt);
        if (IsKeyDown(KEY_DOWN)) speed = max(30.0f, speed - 300 * dt);

        if (mode == 4 && !closed)
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && GetMouseY() > 200)
                poly.push_back({ (float)GetMouseX(), (float)GetMouseY() });
            if (IsKeyPressed(KEY_BACKSPACE) && !poly.empty())
                poly.pop_back();
            if (IsKeyPressed(KEY_ENTER) && poly.size() >= 3)
            {
                closed = true;
                restart();
            }
        }

        if (closed)
            scanY += speed * dt;

        // ---------------- draw into the canvas ----------------
        canvasClear(BLACK);

        int n = (int)poly.size();
        vector<float> crossings;

        if (closed && n >= 3)
        {
            scanLineFill(poly, SKYBLUE, (int)scanY);       // rows above the sweep line

            for (int i = 0; i < n; i++)
                ddaLine((int)poly[i].x, (int)poly[i].y,
                        (int)poly[(i + 1) % n].x, (int)poly[(i + 1) % n].y, WHITE);

            float maxY = 0;
            for (const Vec2& p : poly) maxY = max(maxY, p.y);

            if (scanY < maxY)
            {
                ddaLine(0, (int)scanY, W - 1, (int)scanY, YELLOW);
                crossings = scanLineCrossings(poly, (int)scanY);
                for (float x : crossings)
                    ddaLine((int)x, (int)scanY - 12, (int)x, (int)scanY + 12, RED);
            }
        }
        else
        {
            for (int i = 0; i + 1 < n; i++)                // polyline while drawing
                ddaLine((int)poly[i].x, (int)poly[i].y, (int)poly[i + 1].x, (int)poly[i + 1].y, WHITE);
            for (int i = 0; i < n; i++)
                ddaLine((int)poly[i].x - 4, (int)poly[i].y, (int)poly[i].x + 4, (int)poly[i].y, YELLOW);
        }

        // ---------------- screen ----------------
        BeginDrawing();
        ClearBackground(BLACK);
        canvasPresent(0, 0);

        const char* names[5] = { "", "triangle", "concave arrow", "star", "custom polygon" };
        DrawText("SCAN-LINE POLYGON FILL", 30, 20, 40, WHITE);
        DrawText(TextFormat("polygon: %s (%d vertices)     sweep speed: %.0f rows/s", names[mode], n, speed), 30, 70, 28, YELLOW);

        if (closed && !crossings.empty())
        {
            DrawText(TextFormat("scan line y = %d  ->  %d crossings (fill between pairs 1-2, 3-4, ...)",
                                (int)scanY, (int)crossings.size()), 30, 110, 26, GREEN);
            string xs = "x = ";
            for (size_t i = 0; i < crossings.size() && i < 6; i++)
                xs += TextFormat("%.1f  ", crossings[i]);
            DrawText(xs.c_str(), 30, 145, 26, RED);
        }
        else if (!closed)
            DrawText("click inside the window to add vertices, ENTER to close and fill", 30, 110, 26, GREEN);

        DrawText("1 triangle   2 concave arrow   3 star   4 draw your own   SPACE replay   UP/DOWN speed   C clear", 30, 960, 24, GRAY);
        EndDrawing();
    }

    canvasShutdown();
    CloseWindow();
    return 0;
}
