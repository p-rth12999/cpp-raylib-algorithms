#include "raylib.h"
#include <cmath>
#include <algorithm>
using namespace std;

void bresenhamLine(int x1, int y1, int x2, int y2, Color color)
{
    // Always draw from left to right
    if (x1 > x2)
    {
        swap(x1, x2);
        swap(y1, y2);
    }

    int delta_x = x2 - x1;
    int delta_y = y2 - y1;

    // Determine Y direction
    int yStep;

    if (delta_y >= 0)
        yStep = 1;
    else
        yStep = -1;

    // Use magnitude of delta_y
    delta_y = abs(delta_y);

    int X, Y, Xend;
    int P;

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
        DrawPixel(X, Y, color);

        if (P < 0)
        {
            P = P + 2 * delta_y;
        }
        else
        {
            Y += yStep;
            P = P + 2 * delta_y - 2 * delta_x;
        }

        X++;
    }
}

void ddaLine(int x1, int y1, int x2, int y2, Color color)
{
    int delta_x = x2 - x1;
    int delta_y = y2 - y1;

    int length = max(abs(delta_x), abs(delta_y));

    float x = x1;
    float y = y1;

    float dx = delta_x / (float)length;
    float dy = delta_y / (float)length;

    for (int i = 0; i <= length; i++)
    {
        DrawPixel(round(x), round(y), color);

        x += dx;
        y += dy;
    }
}

int main()
{
    InitWindow(1900, 1000, "Assignment 1 - Mario Kart Scene");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        //Path/Track

        ddaLine(0, 970, 320, 380, WHITE);

        // Right wall
        ddaLine(1900, 970, 1580, 380, WHITE);

        // Back wall

        ddaLine(320, 380, 1580, 380, WHITE);

        // Left vertical wall
        ddaLine(320, 380, 320, 30, WHITE);

        // Right vertical wall
        ddaLine(1580, 380, 1580, 30, WHITE);

        //Vehicle

        ddaLine(910, 650, 990, 650, WHITE);
        ddaLine(990, 650, 1010, 780, WHITE);
        ddaLine(1010, 780, 890, 780, WHITE);
        ddaLine(890, 780, 910, 650, WHITE);

        //Arrows

        // Arrow 1
        ddaLine(950, 430, 910, 470, WHITE);
        ddaLine(950, 430, 990, 470, WHITE);

        // Arrow 2
        ddaLine(950, 445, 915, 480, WHITE);
        ddaLine(950, 445, 985, 480, WHITE);

        // Arrow 3
        ddaLine(950, 460, 920, 490, WHITE);
        ddaLine(950, 460, 980, 490, WHITE);

        // Arrow 4
        ddaLine(950, 475, 925, 500, WHITE);
        ddaLine(950, 475, 975, 500, WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}