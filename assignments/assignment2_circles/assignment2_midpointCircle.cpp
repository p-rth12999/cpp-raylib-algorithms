#include "raylib.h"
#include <iostream>
using namespace std;

// ============================================================
// Midpoint Circle Drawing Algorithm
// ============================================================
void midpoint_circle(int xc, int yc, int r, Color color)
{
    int x = 0;
    int y = r;
    int p = 1 - r;   // initial decision parameter

    while (x <= y)
    {
        // 8-way symmetry
        DrawPixel(xc + x, yc + y, color);
        DrawPixel(xc - x, yc + y, color);
        DrawPixel(xc + x, yc - y, color);
        DrawPixel(xc - x, yc - y, color);

        DrawPixel(xc + y, yc + x, color);
        DrawPixel(xc - y, yc + x, color);
        DrawPixel(xc + y, yc - x, color);
        DrawPixel(xc - y, yc - x, color);

        x++;

        if (p < 0)
        {
            p += 2 * x + 1;
        }
        else
        {
            y--;
            p += 2 * (x - y) + 1;
        }
    }
}

int main()
{
    int xc, yc, r;

    cout << "Enter circle origin (xc yc): ";
    cin >> xc >> yc;

    cout << "Enter circle radius: ";
    cin >> r;

    InitWindow(800, 600, "Midpoint Circle Algorithm");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        midpoint_circle(xc, yc, r, GREEN);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}