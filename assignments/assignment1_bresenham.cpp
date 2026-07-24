#include "raylib.h"
#include <iostream>
using namespace std;

// ============================================================
// Bresenham's Circle Drawing Algorithm
// (Unmodified — core algorithm implementation, per assignment
//  instructions this part is done without AI assistance)
// ============================================================
void bresenham_circle(int xc, int yc, int r, Color color)
{
    int x = 0;
    int y = r;

    int Limit = 0;
    int delta = 2 * (1 - r);

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

        int ro_delta;

        if (y <= Limit)
            break;

        if (delta < 0)
        {
            ro_delta = 2 * delta + 2 * y - 1;

            if (ro_delta <= 0)
            {
                x++;
                delta += 2 * x + 1;
            }
            else
            {
                x++;
                y--;
                delta += 2 * (x - y) + 2;
            }
        }
        else
        {
            ro_delta = 2 * delta - 2 * x - 1;

            if (ro_delta <= 0)
            {
                x++;
                y--;
                delta += 2 * (x - y) + 2;
            }
            else
            {
                y--;
                delta += -2 * y + 1;
            }
        }
    }
}

int main()
{
    // ---- Input: Origin (xc, yc) and Radius ----
    int xc, yc, r;

    cout << "Enter circle origin (xc yc): ";
    cin >> xc >> yc;

    cout << "Enter circle radius: ";
    cin >> r;

    InitWindow(800, 600, "Assignment 2 - Bresenham Circle Algorithm");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        bresenham_circle(xc, yc, r, BLUE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}