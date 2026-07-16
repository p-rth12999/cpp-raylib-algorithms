#include "raylib.h"
#include "algorithms/dda.h"
#include <algorithm>
#include <iostream>
using namespace std;

int main()
{
    InitWindow(800, 600, "DDA Line Drawing Algorithm");
    SetTargetFPS(60);

    int x1, y1, x2, y2;
    cout << "ENTER THE COORDINATES OF THE FIRST POINT (x1,y1): ";
    cin >> x1 >> y1;
    cout << "ENTER THE COORDINATES OF THE SECOND POINT (x2,y2): ";
    cin >> x2 >> y2;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        ddaLine(x1, y1, x2, y2, WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}