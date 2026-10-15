#include "raylib.h"
#include "canvas.h"
#include "dda.h"
#include "bresenham_circle.h"
#include "fill.h"
#include <cmath>
#include <algorithm>
using namespace std;

// Algorithms used here live in algorithms/:
//   canvas (pixel buffer)   dda, bresenham_circle (Assignments 1-2)
//   fill: scanLineFill, floodFill4, boundaryFill4 (Assignment 4)

const int W = 1900;
const int H = 1000;

// ============================================================
// CAR MODEL (local coords, origin = car centre)
// Two extra vertices P and Q split the side edges so that every
// region of the car is a CLOSED outline (needed by boundary fill).
// ============================================================
enum { A, B, C, D, E, F, G, H_, P, Q, NUM_VERTS };

const Vec2 carModel[NUM_VERTS] =
{
    { -110,  125 },   // A
    {  110,  125 },   // B
    {  -90,  -75 },   // C
    {   90,  -75 },   // D
    {  -50,  -75 },   // E
    {   50,  -75 },   // F
    {  -50, -125 },   // G
    {   50, -125 },   // H
    {  -62,  -75 },   // P  (on edge G-A, level with the roof line)
    {   62,  -75 }    // Q  (on edge H-B)
};

const int NUM_EDGES = 13;
const int carEdges[NUM_EDGES][2] =
{
    { A, B }, { A, C }, { B, D }, { C, G }, { D, H_ },
    { G, H_ }, { G, P }, { P, A }, { H_, Q }, { Q, B },
    { E, F }, { P, E }, { F, Q }
};

Vec2 modelToScreen(Vec2 local, Vec2 pos)
{
    return { local.x + pos.x, local.y + pos.y };
}

void drawCar(Vec2 pos)
{
    Vec2 v[NUM_VERTS];
    for (int i = 0; i < NUM_VERTS; i++)
        v[i] = modelToScreen(carModel[i], pos);

    // 1. windshield: scan-line fill (before the outline, so the outline stays on top)
    scanLineFill({ v[G], v[P], v[E], v[F], v[Q], v[H_] }, SKYBLUE);

    // 2. outlines (all WHITE so one boundary colour works)
    for (int i = 0; i < NUM_EDGES; i++)
        ddaLine((int)roundf(v[carEdges[i][0]].x), (int)roundf(v[carEdges[i][0]].y),
                (int)roundf(v[carEdges[i][1]].x), (int)roundf(v[carEdges[i][1]].y), WHITE);

    const Vec2 wheelModel[2] = { { -70, 125 }, { 70, 125 } };
    for (int w = 0; w < 2; w++)
    {
        Vec2 c = modelToScreen(wheelModel[w], pos);
        bresenham_circle((int)c.x, (int)c.y, 40, WHITE);
    }

    // 3. boundary fills
    Vec2 body   = modelToScreen({ 0, 35 }, pos);
    Vec2 panelL = modelToScreen({ -84, -25 }, pos);
    Vec2 panelR = modelToScreen({ 84, -25 }, pos);

    boundaryFill4((int)body.x, (int)body.y, RED, WHITE, 80000);
    boundaryFill4((int)panelL.x, (int)panelL.y, MAROON, WHITE, 80000);
    boundaryFill4((int)panelR.x, (int)panelR.y, MAROON, WHITE, 80000);

    for (int w = 0; w < 2; w++)
    {
        Vec2 up = modelToScreen({ wheelModel[w].x, wheelModel[w].y - 18 }, pos);
        Vec2 dn = modelToScreen({ wheelModel[w].x, wheelModel[w].y + 18 }, pos);
        boundaryFill4((int)up.x, (int)up.y, DARKGRAY, WHITE, 20000);
        boundaryFill4((int)dn.x, (int)dn.y, DARKGRAY, WHITE, 20000);
    }
}

// ============================================================
// SCENE
// ============================================================
struct FillTimes
{
    double scan, flood, boundary;
};

void drawOutlines()
{
    // TRACK (closed with a bottom edge so flood fill has a boundary)
    ddaLine(0, 970, 520, 50, WHITE);
    ddaLine(1900, 970, 1380, 50, WHITE);
    ddaLine(520, 50, 1380, 50, WHITE);
    ddaLine(0, 970, 1899, 970, WHITE);

    // START / FINISH GATE (end caps added so every cell is closed)
    ddaLine(520, 120, 1380, 120, WHITE);
    ddaLine(520, 150, 1380, 150, WHITE);
    ddaLine(520, 180, 1380, 180, WHITE);
    ddaLine(520, 120, 520, 180, WHITE);
    ddaLine(1380, 120, 1380, 180, WHITE);

    for (int x = 560; x <= 1340; x += 80)
        ddaLine(x, 120, x, 180, WHITE);

    // RAINBOW ARCH: 6 chevrons, with the leg ends joined so every band is closed
    int archCenterX = 950;
    for (int i = 0; i < 6; i++)
    {
        int halfWidth = 60 + i * 25;
        int vertexY   = 560 - i * 25;
        int legDrop   = 40;

        ddaLine(archCenterX, vertexY, archCenterX - halfWidth, vertexY + legDrop, WHITE);
        ddaLine(archCenterX, vertexY, archCenterX + halfWidth, vertexY + legDrop, WHITE);

        if (i < 5)
        {
            ddaLine(archCenterX - halfWidth, vertexY + legDrop, archCenterX - halfWidth - 25, vertexY + 15, WHITE);
            ddaLine(archCenterX + halfWidth, vertexY + legDrop, archCenterX + halfWidth + 25, vertexY + 15, WHITE);
        }
    }

    // COINS
    bresenham_circle(700, 300, 25, WHITE);
    bresenham_circle(1200, 300, 25, WHITE);
}

void buildScene(bool fillsOn, FillTimes& ft)
{
    canvasClear(BLACK);
    ft = { 0, 0, 0 };

    double t0 = GetTime();

    // --- SCAN-LINE FILL: sky, grass, checkered gate (polygons known in advance)
    if (fillsOn)
    {
        scanLineFill({ { 0, 0 }, { W - 1, 0 }, { W - 1, 50 }, { 0, 50 } }, SKYBLUE);
        scanLineFill({ { 0, 50 }, { 520, 50 }, { 0, 970 } }, DARKGREEN);
        scanLineFill({ { 1900, 50 }, { 1380, 50 }, { 1900, 970 } }, DARKGREEN);

        int xs[12] = { 520, 560, 640, 720, 800, 880, 960, 1040, 1120, 1200, 1280, 1380 };
        for (int c = 0; c < 11; c++)
            for (int r = 0; r < 2; r++)
            {
                float x0 = xs[c] + 1, x1 = xs[c + 1] - 1;
                float y0 = 120 + r * 30 + 1, y1 = 120 + (r + 1) * 30;
                Color col = ((c + r) % 2 == 0) ? RAYWHITE : Color{ 30, 30, 30, 255 };
                scanLineFill({ { x0, y0 }, { x1, y0 }, { x1, y1 }, { x0, y1 } }, col);
            }
    }
    ft.scan = (GetTime() - t0) * 1000.0;

    drawOutlines();

    // the car's own outlines and fills are drawn inside drawCar()
    // (it uses scan-line + boundary fill); skipped when fills are off
    Vec2 carPos = { 950, 765 };

    if (fillsOn)
    {
        // --- FLOOD FILL: track surface, coins
        t0 = GetTime();
        floodFill4(950, 950, BLACK, GRAY);
        floodFill4(700, 300, BLACK, GOLD);
        floodFill4(1200, 300, BLACK, GOLD);
        ft.flood = (GetTime() - t0) * 1000.0;

        // --- BOUNDARY FILL: rainbow arch bands, car body, wheels
        t0 = GetTime();
        Color rainbow[5] = { RED, ORANGE, YELLOW, GREEN, BLUE };
        for (int i = 0; i < 5; i++)
            boundaryFill4(950, 548 - i * 25, rainbow[i], WHITE, 60000);

        drawCar(carPos);
        ft.boundary = (GetTime() - t0) * 1000.0;
    }
    else
    {
        // outline only, same as Assignment 2
        Vec2 v[NUM_VERTS];
        for (int i = 0; i < NUM_VERTS; i++)
            v[i] = modelToScreen(carModel[i], carPos);
        for (int i = 0; i < NUM_EDGES; i++)
            ddaLine((int)roundf(v[carEdges[i][0]].x), (int)roundf(v[carEdges[i][0]].y),
                    (int)roundf(v[carEdges[i][1]].x), (int)roundf(v[carEdges[i][1]].y), WHITE);
        bresenham_circle(880, 890, 40, WHITE);
        bresenham_circle(1020, 890, 40, WHITE);
    }
}

int main()
{
    InitWindow(W, H, "Game Version 0.4 - Colour Fill Techniques");
    SetTargetFPS(60);

    canvasEnable(W, H);

    bool fillsOn = true;
    FillTimes ft = { 0, 0, 0 };

    buildScene(fillsOn, ft);

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_SPACE))
        {
            fillsOn = !fillsOn;
            buildScene(fillsOn, ft);
                }

        BeginDrawing();
        ClearBackground(BLACK);
        canvasPresent(0, 0);

        /////////////////////////////////////////////////
        // HUD
        /////////////////////////////////////////////////
        DrawText("SCORE", 90, 20, 50, WHITE);
        DrawText("100000", 70, 90, 45, WHITE);
        DrawText("1ST / 10", 1600, 20, 50, WHITE);

        DrawText(TextFormat("FILLS %s   (SPACE to toggle)   scan-line %.1f ms | flood %.1f ms | boundary %.1f ms",
                            fillsOn ? "ON" : "OFF", ft.scan, ft.flood, ft.boundary),
                 20, 975, 20, YELLOW);

        EndDrawing();
    }

    canvasShutdown();
    CloseWindow();
    return 0;
}
