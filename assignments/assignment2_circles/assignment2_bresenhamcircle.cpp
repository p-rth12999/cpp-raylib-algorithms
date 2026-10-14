#include "raylib.h"
#include <iostream>

using namespace std;

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
    InitWindow(800, 600, "Bresenham Circle");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(BLACK);

        bresenham_circle(200, 150, 80, BLUE);
        bresenham_circle(400, 300, 80, RED);
        bresenham_circle(600, 450, 80, GREEN);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}