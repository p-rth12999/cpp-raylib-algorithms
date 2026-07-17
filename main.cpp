#include "raylib.h"
#include "algorithms/dda.h"
#include "algorithms/bresenham_line.h"

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