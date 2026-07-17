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

/////////////////////////////////////////////////
// TRACK
/////////////////////////////////////////////////

// Left boundary
ddaLine(0,970,520,50,WHITE);

// Right boundary
ddaLine(1900,970,1380,50,WHITE);

// Horizon
ddaLine(520,50,1380,50,WHITE);

/////////////////////////////////////////////////
// START GATE
/////////////////////////////////////////////////

ddaLine(520,120,1380,120,WHITE);

for(int x=560;x<=1340;x+=80)
{
    ddaLine(x,120,x,180,WHITE);
}

ddaLine(520,150,1380,150,WHITE);
ddaLine(520,180,1380,180,WHITE);


/////////////////////////////////////////////////
// BOOST ARROWS
/////////////////////////////////////////////////

for(int i=0;i<4;i++)
{
    int y=500+i*20;

    ddaLine(950,y,870,y+45,WHITE);
    ddaLine(950,y,1030,y+45,WHITE);
}

/////////////////////////////////////////////////
// CAR
/////////////////////////////////////////////////

// Front

ddaLine(840,890,1060,890,WHITE);

// Left side

ddaLine(840,890,860,690,WHITE);

// Right side

ddaLine(1060,890,1040,690,WHITE);

// Roof

ddaLine(900,690,1000,690,WHITE);

// Roof sides

ddaLine(860,690,900,640,WHITE);
ddaLine(1040,690,1000,640,WHITE);

// Top

ddaLine(900,640,1000,640,WHITE);

// Back

ddaLine(900,640,840,890,WHITE);
ddaLine(1000,640,1060,890,WHITE);

/////////////////////////////////////////////////
// HUD
/////////////////////////////////////////////////

DrawText("SCORE",90,20,50,WHITE);
DrawText("100000",70,90,45,WHITE);

DrawText("1ST / 10",1600,20,50,WHITE);

EndDrawing();
    }

    CloseWindow();
    return 0;
}