#include "raylib.h"
#include <iostream>
using namespace std;

int main()
{
    // Endpoints
    int x1 = 100, y1 = 600;
    int x2 = 300, y2 = 600;

    // Draw from left to right
    if (x1 > x2)
    {
        swap(x1, x2);
        swap(y1, y2);
    }

    int delta_x = x2 - x1;
    int delta_y = y2 - y1;

    // Variables used by Bresenham
    int X, Y, Xend;
    int P;

    InitWindow(1080, 1000, "Bresenham Line Algorithm");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        // Initialize every frame
        if (x1 < x2)
        {
            X = x1;
            Y = y1;
            Xend = x2;
        }
        else
        {
            X = x2;
            Y = y2;
            Xend = x1;
        }

        P = 2 * delta_y - delta_x;

        while (X <= Xend)
        {
            DrawPixel(X, Y, WHITE);

            if (P < 0)
            {
                P = P + 2 * delta_y;
            }
            else
            {
                Y++;
                P = P + 2 * delta_y - 2 * delta_x;
            }

            X++;
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}