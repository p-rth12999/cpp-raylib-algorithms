#include "raylib.h"
#include <cmath>
#include <algorithm>
using namespace std;

int main()
{
    // Fixed endpoints
    int x1 = 100, y1 = 600;
    int x2 = 700, y2 = 200;

    int delta_x = x2 - x1;
    int delta_y = y2 - y1;

    int length = max(abs(delta_x), abs(delta_y));

    float dx = delta_x / (float)length;
    float dy = delta_y / (float)length;

    InitWindow(1080, 1000, "Assignment 1 - DDA Line Algorithm");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        float x = x1;
        float y = y1;

        for (int i = 0; i <= length; i++)
        {
            DrawPixel(round(x), round(y), WHITE);
            x += dx;
            y += dy;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}