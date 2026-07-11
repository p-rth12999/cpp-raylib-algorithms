#include "raylib.h"
#include "algorithms/dda.h"

int main()
{
    InitWindow(800, 600, "DDA");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(BLACK);

        ddaLine(100, 100, 600, 400, WHITE);
        ddaLine(500, 100, 600, 400, WHITE);
        ddaLine(600, 200, 500, 400, WHITE);
        ddaLine(100, 600, 1000, 400, WHITE);

        EndDrawing();
    }

    CloseWindow();
}