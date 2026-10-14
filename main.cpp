

#include "raylib.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <algorithm>

using namespace std;

const Color LIGHTBLUE = SKYBLUE;
const Color LIGHTGREEN = LIME;
const Color LIGHTCYAN = Color{224, 255, 255, 255};

void fillPolygon(const int *points, int pointCount, Color color)
{
    if (pointCount < 3) return;
    Vector2 center = { (float)points[0], (float)points[1] };
    for (int i = 1; i < pointCount - 1; i++)
    {
        Vector2 a = { (float)points[i * 2], (float)points[i * 2 + 1] };
        Vector2 b = { (float)points[(i + 1) * 2], (float)points[(i + 1) * 2 + 1] };
        DrawTriangle(center, a, b, color);
    }
}

void drawTextAt(int x, int y, const char *text, Color color, int size)
{
    DrawText(text, x, y, size, color);
}

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 700;

const int ROAD_TOP = 220;
const int ROAD_BOTTOM = 690;

struct Point2D
{
    int x;
    int y;

    Point2D(int px = 0, int py = 0)
    {
        x = px;
        y = py;
    }
};

struct RaceObject
{
    float z;
    int lane;
    int type;
    bool active;

    RaceObject()
    {
        z = 0;
        lane = 0;
        type = 0;
        active = true;
    }
};

float playerX = 0.0f;

float speed = 0.0f;
float maxSpeed = 1.8f;

float roadOffset = 0.0f;

float raceDistance = 0.0f;

const float LAP_DISTANCE = 3000.0f;

int currentLap = 1;
const int TOTAL_LAPS = 3;

int score = 0;

int lives = 3;

int racePosition = 1;

bool raceStarted = false;
bool gameOver = false;
bool raceFinished = false;

bool boostActive = false;

int countdownValue = 3;

RaceObject objects[18];

void drawDDALine(
    int x1,
    int y1,
    int x2,
    int y2,
    int color)
{
    float dx = x2 - x1;
    float dy = y2 - y1;

    int steps = max(abs((int)dx), abs((int)dy));

    if (steps == 0)
    {
        DrawPixel(x1, y1, color);
        return;
    }

    float xIncrement = dx / (float)steps;
    float yIncrement = dy / (float)steps;

    float x = x1;
    float y = y1;

    for (int i = 0; i <= steps; i++)
    {
        DrawPixel(
            (int)round(x),
            (int)round(y),
            color
        );

        x += xIncrement;
        y += yIncrement;
    }
}

void drawBresenhamLine(
    int x1,
    int y1,
    int x2,
    int y2,
    int color)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    int x = x1;
    int y = y1;

    DrawPixel(x, y, color);

    if (dx >= dy)
    {
        int p = 2 * dy - dx;

        while (x != x2)
        {
            x += sx;

            if (p < 0)
            {
                p += 2 * dy;
            }
            else
            {
                y += sy;
                p += 2 * dy - 2 * dx;
            }

            DrawPixel(x, y, color);
        }
    }
    else
    {
        int p = 2 * dx - dy;

        while (y != y2)
        {
            y += sy;

            if (p < 0)
            {
                p += 2 * dx;
            }
            else
            {
                x += sx;
                p += 2 * dx - 2 * dy;
            }

            DrawPixel(x, y, color);
        }
    }
}

void drawDashedLine(
    int x1,
    int y1,
    int x2,
    int y2,
    int dashLength,
    int gapLength,
    int color)
{
    float dx = x2 - x1;
    float dy = y2 - y1;

    int steps = max(abs((int)dx), abs((int)dy));

    if (steps == 0)
    {
        DrawPixel(x1, y1, color);
        return;
    }

    float xIncrement = dx / steps;
    float yIncrement = dy / steps;

    float x = x1;
    float y = y1;

    int cycle = dashLength + gapLength;

    for (int i = 0; i <= steps; i++)
    {
        if ((i % cycle) < dashLength)
        {
            DrawPixel(
                (int)round(x),
                (int)round(y),
                color
            );
        }

        x += xIncrement;
        y += yIncrement;
    }
}

void drawStartScreen()
{
    BeginDrawing();
    ClearBackground(BLACK);

    ClearBackground(BLACK);

    

    drawTextAt(270, 120, "RETRO KART", WHITE, 24);

    

    drawTextAt(330, 190, "LIVE RACING", WHITE, 24);

    

    drawTextAt(300, 290, "COMPUTER GRAPHICS & GAMING", WHITE, 24);

    drawTextAt(350, 350, "UP    - ACCELERATE", WHITE, 24);

    drawTextAt(350, 385, "DOWN  - BRAKE", WHITE, 24);

    drawTextAt(350, 420, "LEFT  - STEER LEFT", WHITE, 24);

    drawTextAt(350, 455, "RIGHT - STEER RIGHT", WHITE, 24);

    drawTextAt(350, 490, "SPACE - BOOST", WHITE, 24);

    drawTextAt(330, 570, "PRESS ENTER TO START", LIGHTGREEN, 24);
    EndDrawing();

    while (true)
    {
        BeginDrawing();
        EndDrawing();

        if (IsKeyDown(KEY_ENTER))
        {
            break;
        }

        if (IsKeyDown(KEY_ESCAPE))
        {
            CloseWindow();
            exit(0);
        }

        WaitTime(0.03);
    }
}

void countdown()
{
    for (int i = 3; i >= 1; i--)
    {
        BeginDrawing();
        ClearBackground(BLACK);

        

        char number[10];

        sprintf(number, "%d", i);

        drawTextAt(470, 260, number, YELLOW, 80);

        

        drawTextAt(380, 430, "GET READY!", WHITE, 24);
        EndDrawing();
        WaitTime(1.0);
    }

    BeginDrawing();
    ClearBackground(BLACK);
    drawTextAt(390, 280, "GO!", LIGHTGREEN, 48);
    EndDrawing();
    WaitTime(0.6);

    raceStarted = true;
}

void drawSky()
{

    DrawRectangle(0, 0, SCREEN_WIDTH, 320, LIGHTBLUE);

    

    DrawEllipse(820, 90, 45, 45, YELLOW);

    

    DrawEllipse(150, 100, 40, 20, WHITE);

    DrawEllipse(190, 100, 50, 25, WHITE);

    DrawEllipse(230, 100, 35, 18, WHITE);

    DrawEllipse(500, 130, 40, 20, WHITE);

    DrawEllipse(540, 130, 50, 25, WHITE);

    DrawEllipse(580, 130, 35, 18, WHITE);
}

void drawMountains()
{

    

    int mountain[] =
    {
        0, 320,
        100, 190,
        200, 320,
        320, 170,
        450, 320,
        580, 180,
        720, 320,
        850, 170,
        1000, 320,
        1000, 360,
        0, 360
    };

    fillPolygon(mountain, 6, GREEN);
}

float getRoadCenter(float y)
{
    float curve =
        sin((y + roadOffset) * 0.012f) * 90.0f;

    return SCREEN_WIDTH / 2.0f + curve;
}

float getRoadWidth(float y)
{
    float t =
        (y - ROAD_TOP) /
        (float)(ROAD_BOTTOM - ROAD_TOP);

    if (t < 0)
        t = 0;

    if (t > 1)
        t = 1;

    return 100 + t * 800;
}

void drawRoad()
{

    

    DrawRectangle(0, 300, SCREEN_WIDTH, SCREEN_HEIGHT - 300, DARKGREEN);

    int topLeft =
        SCREEN_WIDTH / 2 - 50;

    int topRight =
        SCREEN_WIDTH / 2 + 50;

    int bottomLeft = 40;

    int bottomRight =
        SCREEN_WIDTH - 40;

    

    int road[] =
    {
        topLeft,
        ROAD_TOP,

        topRight,
        ROAD_TOP,

        bottomRight,
        ROAD_BOTTOM,

        bottomLeft,
        ROAD_BOTTOM
    };

    fillPolygon(road, 4, DARKGRAY);

    drawBresenhamLine(
        topLeft,
        ROAD_TOP,
        bottomLeft,
        ROAD_BOTTOM,
        WHITE
    );

    drawBresenhamLine(
        topRight,
        ROAD_TOP,
        bottomRight,
        ROAD_BOTTOM,
        WHITE
    );

    for (int i = 0; i < 14; i++)
    {
        float z =
            fmod(
                i * 100 + roadOffset,
                1000.0f
            );

        float t =
            z / 1000.0f;

        float y =
            ROAD_TOP +
            t *
            (ROAD_BOTTOM - ROAD_TOP);

        if (y < ROAD_TOP)
            continue;

        float center =
            getRoadCenter(y);

        float width =
            getRoadWidth(y);

        int dashWidth =
            max(
                2,
                (int)(width * 0.012f)
            );

        int dashHeight =
            15 + (int)(t * 30);

        drawBresenhamLine(
            (int)(center - dashWidth),
            (int)y,
            (int)(center + dashWidth),
            (int)(y + dashHeight),
            WHITE
        );
    }

    for (int i = 0; i < 16; i++)
    {
        float z =
            fmod(
                i * 70 + roadOffset,
                900.0f
            );

        float t =
            z / 900.0f;

        float y =
            ROAD_TOP +
            t *
            (ROAD_BOTTOM - ROAD_TOP);

        float width =
            getRoadWidth(y);

        float center =
            getRoadCenter(y);

        int left =
            (int)(center - width / 2);

        int right =
            (int)(center + width / 2);

        int stripeSize =
            8 + (int)(t * 18);

        if (i % 2 == 0)
        {
            drawBresenhamLine(
                left,
                (int)y,
                left - stripeSize,
                (int)(y + 20 + t * 20),
                RED
            );

            drawBresenhamLine(
                right,
                (int)y,
                right + stripeSize,
                (int)(y + 20 + t * 20),
                RED
            );
        }
    }
}

void drawTree(
    int x,
    int y,
    float scale)
{
    int trunkWidth =
        (int)(12 * scale);

    int trunkHeight =
        (int)(50 * scale);

    drawBresenhamLine(
        x - trunkWidth,
        y,
        x - trunkWidth,
        y + trunkHeight,
        BROWN
    );

    drawBresenhamLine(
        x - trunkWidth,
        y + trunkHeight,
        x + trunkWidth,
        y + trunkHeight,
        BROWN
    );

    drawBresenhamLine(
        x + trunkWidth,
        y + trunkHeight,
        x + trunkWidth,
        y,
        BROWN
    );

    drawBresenhamLine(
        x + trunkWidth,
        y,
        x - trunkWidth,
        y,
        BROWN
    );

    int r =
        (int)(35 * scale);

    DrawCircle(x, (int)(y - 25 * scale), r, GREEN);

    DrawCircle((int)(x - 20 * scale), y, (int)(r * 0.7), GREEN);

    DrawCircle((int)(x + 20 * scale), y, (int)(r * 0.7), GREEN);
}

void drawScenery()
{
    for (int i = 0; i < 12; i++)
    {
        float z =
            fmod(
                i * 110 + roadOffset * 0.7f,
                1000.0f
            );

        float t =
            z / 1000.0f;

        float y =
            ROAD_TOP +
            t *
            (ROAD_BOTTOM - ROAD_TOP);

        float roadWidth =
            getRoadWidth(y);

        float center =
            getRoadCenter(y);

        int leftX =
            (int)(
                center -
                roadWidth / 2 -
                60 -
                t * 80
            );

        int rightX =
            (int)(
                center +
                roadWidth / 2 +
                60 +
                t * 80
            );

        float scale =
            0.2f +
            t * 0.9f;

        drawTree(
            leftX,
            (int)y,
            scale
        );

        drawTree(
            rightX,
            (int)y,
            scale
        );
    }
}

void drawCoin(
    RaceObject &obj)
{
    if (!obj.active)
        return;

    if (obj.z < 0 || obj.z > 1000)
        return;

    float t =
        1.0f -
        obj.z / 1000.0f;

    float y =
        ROAD_TOP +
        t *
        (ROAD_BOTTOM - ROAD_TOP);

    float roadWidth =
        getRoadWidth(y);

    float center =
        getRoadCenter(y);

    float x =
        center +
        obj.lane *
        roadWidth *
        0.23f;

    int radius =
        4 +
        (int)(18 * t);

    drawDDALine(
        (int)x - radius,
        (int)y,
        (int)x + radius,
        (int)y,
        YELLOW
    );

    drawDDALine(
        (int)x,
        (int)y - radius,
        (int)x,
        (int)y + radius,
        YELLOW
    );

    drawBresenhamLine(
        (int)x - radius / 2,
        (int)y - radius / 2,
        (int)x + radius / 2,
        (int)y + radius / 2,
        YELLOW
    );

    drawBresenhamLine(
        (int)x - radius / 2,
        (int)y + radius / 2,
        (int)x + radius / 2,
        (int)y - radius / 2,
        YELLOW
    );
}

void drawOpponent(
    RaceObject &obj)
{
    if (!obj.active)
        return;

    if (obj.z < 0 || obj.z > 1000)
        return;

    float t =
        1.0f -
        obj.z / 1000.0f;

    float y =
        ROAD_TOP +
        t *
        (ROAD_BOTTOM - ROAD_TOP);

    float roadWidth =
        getRoadWidth(y);

    float center =
        getRoadCenter(y);

    int x =
        (int)(
            center +
            obj.lane *
            roadWidth *
            0.25f
        );

    int carWidth =
        8 +
        (int)(35 * t);

    int carHeight =
        12 +
        (int)(55 * t);

    drawBresenhamLine(
        x - carWidth,
        y,
        x + carWidth,
        y,
        RED
    );

    drawBresenhamLine(
        x - carWidth,
        y,
        x - carWidth,
        y + carHeight,
        RED
    );

    drawBresenhamLine(
        x + carWidth,
        y,
        x + carWidth,
        y + carHeight,
        RED
    );

    drawBresenhamLine(
        x - carWidth,
        y + carHeight,
        x + carWidth,
        y + carHeight,
        RED
    );

    drawBresenhamLine(
        x - carWidth / 2,
        y,
        x - carWidth / 3,
        y - carHeight / 2,
        CYAN
    );

    drawBresenhamLine(
        x - carWidth / 3,
        y - carHeight / 2,
        x + carWidth / 3,
        y - carHeight / 2,
        CYAN
    );

    drawBresenhamLine(
        x + carWidth / 3,
        y - carHeight / 2,
        x + carWidth / 2,
        y,
        CYAN
    );

    drawBresenhamLine(
        x - carWidth,
        y + carHeight - 5,
        x - carWidth,
        y + carHeight + 8,
        WHITE
    );

    drawBresenhamLine(
        x + carWidth,
        y + carHeight - 5,
        x + carWidth,
        y + carHeight + 8,
        WHITE
    );
}

void drawPlayerKart()
{
    int cx =
        SCREEN_WIDTH / 2 +
        (int)(playerX * 280);

    int cy = 580;

    float scale = 1.5f;

    int w =
        (int)(26 * scale);

    int h =
        (int)(42 * scale);

    

    int flx1 =
        cx - w * 0.75f;

    int fly1 =
        cy - h * 0.45f;

    int flx2 =
        cx - w * 0.45f;

    int fly2 =
        cy - h * 0.15f;

    drawBresenhamLine(
        flx1,
        fly1,
        flx2,
        fly1,
        LIGHTGRAY
    );

    drawBresenhamLine(
        flx2,
        fly1,
        flx2,
        fly2,
        LIGHTGRAY
    );

    drawBresenhamLine(
        flx2,
        fly2,
        flx1,
        fly2,
        LIGHTGRAY
    );

    drawBresenhamLine(
        flx1,
        fly2,
        flx1,
        fly1,
        LIGHTGRAY
    );

    int frx1 =
        cx + w * 0.45f;

    int fry1 =
        cy - h * 0.45f;

    int frx2 =
        cx + w * 0.75f;

    int fry2 =
        cy - h * 0.15f;

    drawBresenhamLine(
        frx1,
        fry1,
        frx2,
        fry1,
        LIGHTGRAY
    );

    drawBresenhamLine(
        frx2,
        fry1,
        frx2,
        fry2,
        LIGHTGRAY
    );

    drawBresenhamLine(
        frx2,
        fry2,
        frx1,
        fry2,
        LIGHTGRAY
    );

    drawBresenhamLine(
        frx1,
        fry2,
        frx1,
        fry1,
        LIGHTGRAY
    );

    drawDDALine(
        flx2,
        cy - h * 0.30f,
        frx1,
        cy - h * 0.30f,
        LIGHTGRAY
    );

    

    int rlx1 =
        cx - w * 0.85f;

    int rly1 =
        cy + h * 0.10f;

    int rlx2 =
        cx - w * 0.50f;

    int rly2 =
        cy + h * 0.42f;

    drawBresenhamLine(
        rlx1,
        rly1,
        rlx2,
        rly1,
        LIGHTGRAY
    );

    drawBresenhamLine(
        rlx2,
        rly1,
        rlx2,
        rly2,
        LIGHTGRAY
    );

    drawBresenhamLine(
        rlx2,
        rly2,
        rlx1,
        rly2,
        LIGHTGRAY
    );

    drawBresenhamLine(
        rlx1,
        rly2,
        rlx1,
        rly1,
        LIGHTGRAY
    );

    int rrx1 =
        cx + w * 0.50f;

    int rry1 =
        cy + h * 0.10f;

    int rrx2 =
        cx + w * 0.85f;

    int rry2 =
        cy + h * 0.42f;

    drawBresenhamLine(
        rrx1,
        rry1,
        rrx2,
        rry1,
        LIGHTGRAY
    );

    drawBresenhamLine(
        rrx2,
        rry1,
        rrx2,
        rry2,
        LIGHTGRAY
    );

    drawBresenhamLine(
        rrx2,
        rry2,
        rrx1,
        rry2,
        LIGHTGRAY
    );

    drawBresenhamLine(
        rrx1,
        rry2,
        rrx1,
        rry1,
        LIGHTGRAY
    );

    drawDDALine(
        rlx2,
        cy + h * 0.26f,
        rrx1,
        cy + h * 0.26f,
        LIGHTGRAY
    );

    

    int mx1 =
        cx - w * 0.35f;

    int my1 =
        cy - h * 0.20f;

    int mx2 =
        cx + w * 0.35f;

    int my2 =
        cy + h * 0.35f;

    drawBresenhamLine(
        mx1,
        my1,
        mx2,
        my1,
        LIGHTCYAN
    );

    drawBresenhamLine(
        mx2,
        my1,
        mx2,
        my2,
        LIGHTCYAN
    );

    drawBresenhamLine(
        mx2,
        my2,
        mx1,
        my2,
        LIGHTCYAN
    );

    drawBresenhamLine(
        mx1,
        my2,
        mx1,
        my1,
        LIGHTCYAN
    );

    

    drawBresenhamLine(
        cx - w * 0.50f,
        cy - h * 0.05f,
        mx1,
        cy - h * 0.15f,
        CYAN
    );

    drawBresenhamLine(
        cx - w * 0.50f,
        cy - h * 0.05f,
        cx - w * 0.50f,
        cy + h * 0.25f,
        CYAN
    );

    drawBresenhamLine(
        cx - w * 0.50f,
        cy + h * 0.25f,
        mx1,
        cy + h * 0.30f,
        CYAN
    );

    drawBresenhamLine(
        cx + w * 0.50f,
        cy - h * 0.05f,
        mx2,
        cy - h * 0.15f,
        CYAN
    );

    drawBresenhamLine(
        cx + w * 0.50f,
        cy - h * 0.05f,
        cx + w * 0.50f,
        cy + h * 0.25f,
        CYAN
    );

    drawBresenhamLine(
        cx + w * 0.50f,
        cy + h * 0.25f,
        mx2,
        cy + h * 0.30f,
        CYAN
    );

    

    drawBresenhamLine(
        mx1,
        my1,
        cx,
        cy - h * 0.55f,
        GREEN
    );

    drawBresenhamLine(
        cx,
        cy - h * 0.55f,
        mx2,
        my1,
        GREEN
    );

    

    int fwx1 =
        cx - w * 0.85f;

    int fwx2 =
        cx + w * 0.85f;

    int fwy =
        cy - h * 0.52f;

    drawDDALine(
        fwx1,
        fwy,
        fwx2,
        fwy,
        GREEN
    );

    drawDDALine(
        fwx1,
        fwy - 4,
        fwx2,
        fwy - 4,
        GREEN
    );

    drawBresenhamLine(
        fwx1,
        fwy - 4,
        fwx1,
        fwy + 3,
        GREEN
    );

    drawBresenhamLine(
        fwx2,
        fwy - 4,
        fwx2,
        fwy + 3,
        GREEN
    );

    

    drawDDALine(
        cx - 5,
        cy - 8,
        cx + 5,
        cy - 8,
        LIGHTBLUE
    );

    drawBresenhamLine(
        cx - 5,
        cy - 2,
        cx + 5,
        cy - 2,
        YELLOW
    );

    drawBresenhamLine(
        cx + 5,
        cy - 2,
        cx + 5,
        cy + 9,
        YELLOW
    );

    drawBresenhamLine(
        cx + 5,
        cy + 9,
        cx - 5,
        cy + 9,
        YELLOW
    );

    drawBresenhamLine(
        cx - 5,
        cy + 9,
        cx - 5,
        cy - 2,
        YELLOW
    );

    drawDDALine(
        cx - 4,
        cy + 1,
        cx + 4,
        cy + 1,
        WHITE
    );

    

    int rwx1 =
        cx - w * 0.80f;

    int rwx2 =
        cx + w * 0.80f;

    int rwy1 =
        cy + h * 0.44f;

    int rwy2 =
        cy + h * 0.52f;

    drawDDALine(
        rwx1,
        rwy1,
        rwx2,
        rwy1,
        RED
    );

    drawDDALine(
        rwx1,
        rwy2,
        rwx2,
        rwy2,
        RED
    );

    drawBresenhamLine(
        rwx1,
        rwy1,
        rwx1,
        rwy2,
        RED
    );

    drawBresenhamLine(
        rwx2,
        rwy1,
        rwx2,
        rwy2,
        RED
    );

    drawBresenhamLine(
        cx - w * 0.25f,
        cy + h * 0.35f,
        cx - w * 0.25f,
        rwy1,
        LIGHTGRAY
    );

    drawBresenhamLine(
        cx + w * 0.25f,
        cy + h * 0.35f,
        cx + w * 0.25f,
        rwy1,
        LIGHTGRAY
    );

    

    if (boostActive)
    {

        drawBresenhamLine(
            cx - 15,
            cy + h * 0.50f,
            cx,
            cy + h * 0.75f,
            YELLOW
        );

        drawBresenhamLine(
            cx,
            cy + h * 0.75f,
            cx + 15,
            cy + h * 0.50f,
            YELLOW
        );

        drawBresenhamLine(
            cx - 8,
            cy + h * 0.48f,
            cx,
            cy + h * 0.65f,
            RED
        );

        drawBresenhamLine(
            cx,
            cy + h * 0.65f,
            cx + 8,
            cy + h * 0.48f,
            RED
        );
    }
}

void drawFinishLine()
{
    int y = 280;

    int left = 380;
    int right = 620;

    for (int i = 0; i < 8; i++)
    {
        int x1 =
            left +
            i * 30;

        int x2 =
            x1 + 30;

        if (i % 2 == 0)
        {
            drawBresenhamLine(
                x1,
                y,
                x2,
                y,
                WHITE
            );
        }
        else
        {
            drawBresenhamLine(
                x1,
                y,
                x2,
                y,
                BLACK
            );
        }
    }

    drawBresenhamLine(
        left,
        y,
        right,
        y,
        WHITE
    );
}

void drawHUD()
{

    

    drawTextAt(25, 15, "RETRO KART CGG - LIVE RACING", WHITE, 24);

    

    char buffer[100];

    sprintf(
        buffer,
        "SPEED: %d",
        (int)(speed * 100)
    );

    drawTextAt(25, 55, buffer, WHITE, 20);

    sprintf(
        buffer,
        "LAP: %d / %d",
        currentLap,
        TOTAL_LAPS
    );

    drawTextAt(25, 80, buffer, WHITE, 20);

    sprintf(
        buffer,
        "SCORE: %d",
        score
    );

    drawTextAt(25, 105, buffer, WHITE, 20);

    sprintf(
        buffer,
        "LIVES: %d",
        lives
    );

    drawTextAt(25, 130, buffer, WHITE, 20);

    sprintf(
        buffer,
        "POSITION: %d / 6",
        racePosition
    );

    drawTextAt(25, 155, buffer, WHITE, 20);

    sprintf(
        buffer,
        "DISTANCE: %d",
        (int)raceDistance
    );

    drawTextAt(25, 180, buffer, WHITE, 20);

    if (boostActive)
    {

        drawTextAt(800, 55, "BOOST!", WHITE, 24);
    }

    

    drawTextAt(720, 650, "ARROWS: DRIVE", WHITE, 24);

    drawTextAt(720, 670, "SPACE: BOOST", WHITE, 24);
}

void createObjects()
{
    for (int i = 0; i < 18; i++)
    {
        objects[i].z =
            150 +
            rand() % 900;

        objects[i].lane =
            rand() % 3 - 1;

        

        if (rand() % 3 == 0)
            objects[i].type = 0;
        else
            objects[i].type = 1;

        objects[i].active = true;
    }
}

void respawnObject(int i)
{
    objects[i].z =
        700 +
        rand() % 900;

    objects[i].lane =
        rand() % 3 - 1;

    if (rand() % 3 == 0)
        objects[i].type = 0;
    else
        objects[i].type = 1;

    objects[i].active = true;
}

void handleInput()
{

    if (IsKeyDown(KEY_LEFT))
    {
        playerX -= 0.035f;
    }

    if (IsKeyDown(KEY_RIGHT))
    {
        playerX += 0.035f;
    }

    if (playerX < -1.0f)
        playerX = -1.0f;

    if (playerX > 1.0f)
        playerX = 1.0f;

    if (IsKeyDown(KEY_UP))
    {
        speed += 0.025f;
    }
    else
    {
        speed -= 0.008f;
    }

    if (IsKeyDown(KEY_DOWN))
    {
        speed -= 0.035f;
    }

    if (IsKeyDown(KEY_SPACE))
    {
        boostActive = true;

        speed += 0.04f;

        if (score >= 20)
        {
            score -= 20;
        }
    }
    else
    {
        boostActive = false;
    }

    if (speed < 0)
        speed = 0;

    if (speed > maxSpeed)
        speed = maxSpeed;
}

void checkCollision(
    RaceObject &obj)
{
    if (!obj.active)
        return;

    if (
        obj.z < 100 &&
        obj.z > 0
    )
    {
        float laneDifference =
            fabs(
                obj.lane -
                playerX
            );

        if (laneDifference < 0.45f)
        {

            

            if (obj.type == 0)
            {
                score += 100;

                obj.active = false;
            }

            

            else
            {
                lives--;

                speed *= 0.35f;

                obj.active = false;

                if (lives <= 0)
                {
                    gameOver = true;
                }
            }
        }
    }
}

void updateObjects()
{
    for (int i = 0; i < 18; i++)
    {
        if (!objects[i].active)
            continue;

        objects[i].z -=
            speed * 8.0f;

        checkCollision(
            objects[i]
        );

        if (objects[i].z < -100)
        {
            respawnObject(i);
        }
    }
}

void updateGame()
{

    roadOffset +=
        speed * 10.0f;

    raceDistance +=
        speed * 1.2f;

    if (
        raceDistance >=
        currentLap * LAP_DISTANCE
    )
    {
        currentLap++;

        score += 500;

        if (currentLap > TOTAL_LAPS)
        {
            raceFinished = true;
        }
    }

    if (speed > 1.3f)
        racePosition = 1;
    else if (speed > 1.0f)
        racePosition = 2;
    else if (speed > 0.7f)
        racePosition = 3;
    else if (speed > 0.4f)
        racePosition = 4;
    else
        racePosition = 5;

    updateObjects();
}

void drawGameOver()
{
    BeginDrawing();
    ClearBackground(BLACK);

    

    drawTextAt(330, 200, "GAME OVER", WHITE, 24);

    

    char buffer[100];

    sprintf(
        buffer,
        "FINAL SCORE: %d",
        score
    );

    drawTextAt(380, 320, buffer, WHITE, 20);

    sprintf(
        buffer,
        "LAP REACHED: %d",
        currentLap
    );

    drawTextAt(380, 360, buffer, WHITE, 20);

    drawTextAt(320, 450, "PRESS ENTER TO EXIT", YELLOW, 24);
    EndDrawing();

    while (true)
    {
        BeginDrawing();
        EndDrawing();

        if (IsKeyDown(KEY_ENTER))
        {
            break;
        }

        WaitTime(0.03);
    }
}

void drawFinishScreen()
{
    BeginDrawing();
    ClearBackground(BLACK);

    

    drawTextAt(350, 180, "YOU WIN!", WHITE, 24);

    

    drawTextAt(350, 270, "RACE COMPLETED", WHITE, 24);

    

    char buffer[100];

    sprintf(
        buffer,
        "FINAL SCORE: %d",
        score
    );

    drawTextAt(380, 350, buffer, WHITE, 20);

    sprintf(
        buffer,
        "POSITION: %d / 6",
        racePosition
    );

    drawTextAt(380, 390, buffer, WHITE, 20);

    drawTextAt(320, 480, "PRESS ENTER TO EXIT", YELLOW, 24);
    EndDrawing();

    while (true)
    {
        BeginDrawing();
        EndDrawing();

        if (IsKeyDown(KEY_ENTER))
        {
            break;
        }

        WaitTime(0.03);
    }
}

void drawGameFrame()
{
    ClearBackground(BLACK);

    drawSky();

    drawMountains();

    drawScenery();

    drawRoad();

    if (
        ((int)raceDistance % 1000) < 80
    )
    {
        drawFinishLine();
    }

    for (int i = 0; i < 18; i++)
    {
        if (!objects[i].active)
            continue;

        if (objects[i].type == 0)
        {
            drawCoin(
                objects[i]
            );
        }
        else
        {
            drawOpponent(
                objects[i]
            );
        }
    }

    drawPlayerKart();

    drawHUD();
}

void gameLoop()
{
    createObjects();

    countdown();

    while (
        !gameOver &&
        !raceFinished
    )
    {

        if (
            IsKeyDown(KEY_ESCAPE)
        )
        {
            break;
        }

        handleInput();

        updateGame();

        BeginDrawing();
        drawGameFrame();
        EndDrawing();

        WaitTime(0.02);
    }
}

int main()
{
    srand(
        (unsigned)time(NULL)
    );
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Retro Kart CGG - Assignment 2 Live Racing");
    SetTargetFPS(50);

    drawStartScreen();

    gameLoop();

    if (gameOver)
    {
        drawGameOver();
    }
    else if (raceFinished)
    {
        drawFinishScreen();
    }

    CloseWindow();

    return 0;
} 