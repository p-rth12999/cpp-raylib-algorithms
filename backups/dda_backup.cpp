#include "raylib.h"
#include <cmath>
#include <iostream>
#include <algorithm>
using namespace std;

using namespace std;

int main()
{
    // Line endpoints
    int x1, y1, x2, y2;

    cout << "Enter the coordinates of the first point (x1, y1): ";
    cin >> x1 >> y1;
    cout << "Enter the coordinates of the second point (x2, y2): ";
    cin >> x2 >> y2;

    int delta_x = x2 - x1;
    int delta_y = y2 - y1;

    int length = max(abs(delta_x), abs(delta_y));

    float x = x1;
    float y = y1;

    float dx = delta_x / (float)length;
    float dy = delta_y / (float)length;

    InitWindow(80, 60, "DDA Line Algorithm");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(BLACK);
        float currentX = x;
        float currentY = y;

        for (int i = 0; i <= length; i++)
        {
            DrawPixel(round(currentX), round(currentY), WHITE);

            currentX += dx;
            currentY += dy;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}