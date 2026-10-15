#include "raylib.h"
#include "dda.h"
#include "transform2d.h"
#include <cmath>
#include <algorithm>
using namespace std;

// ============================================================
// COORDINATE SYSTEM
// Maths convention: origin at the screen centre, x -> right, y -> UP.
// A positive angle therefore rotates COUNTER-CLOCKWISE (textbook style).
// ============================================================
const int SW = 1900;
const int SH = 1000;
const int OX = SW / 2;
const int OY = SH / 2;

int toScreenX(float x) { return OX + (int)roundf(x); }
int toScreenY(float y) { return OY - (int)roundf(y); }

void drawSeg(Vec2 a, Vec2 b, Color c)
{
    ddaLine(toScreenX(a.x), toScreenY(a.y), toScreenX(b.x), toScreenY(b.y), c);
}

void drawAxes()
{
    ddaLine(0, OY, SW - 1, OY, DARKGRAY);
    ddaLine(OX, 0, OX, SH - 1, DARKGRAY);

    for (int i = -900; i <= 900; i += 100)
    {
        ddaLine(OX + i, OY - 5, OX + i, OY + 5, GRAY);
        ddaLine(OX - 5, OY + i, OX + 5, OY + i, GRAY);
    }
}

// ============================================================
// OBJECT = the car outline from the game (9 edges, 8 vertices)
// Its centre is moved to (250, 100) so you can SEE the difference
// between transforming about the origin and about the object.
// ============================================================
enum { A, B, C, D, E, F, G, H, NV };

const Vec2 carLocal[NV] =
{
    { -110, -125 }, {  110, -125 }, {  -90,   75 }, {   90,   75 },
    {  -50,   75 }, {   50,   75 }, {  -50,  125 }, {   50,  125 }
};

const int NE = 9;
const int carEdges[NE][2] =
{
    { A, B }, { A, C }, { B, D }, { E, F }, { C, G }, { D, H }, { G, H }, { G, A }, { H, B }
};

const Vec2 CENTRE = { 250, 100 };

void getObject(Vec2 out[NV])
{
    for (int i = 0; i < NV; i++)
        out[i] = { carLocal[i].x + CENTRE.x, carLocal[i].y + CENTRE.y };
}

void drawObject(const Vec2 v[NV], Color c)
{
    for (int i = 0; i < NE; i++)
        drawSeg(v[carEdges[i][0]], v[carEdges[i][1]], c);
}

Vec2 centroid(const Vec2 v[NV])
{
    Vec2 s = { 0, 0 };
    for (int i = 0; i < NV; i++) { s.x += v[i].x; s.y += v[i].y; }
    return { s.x / NV, s.y / NV };
}

void drawCross(Vec2 p, Color c)
{
    ddaLine(toScreenX(p.x) - 8, toScreenY(p.y), toScreenX(p.x) + 8, toScreenY(p.y), c);
    ddaLine(toScreenX(p.x), toScreenY(p.y) - 8, toScreenX(p.x), toScreenY(p.y) + 8, c);
}

float clampf(float v, float lo, float hi) { return max(lo, min(hi, v)); }

// ============================================================
// ASSIGNMENT 3A - TRANSLATION
//   ARROWS : change tx / ty        R : reset
// ============================================================
int main()
{
    InitWindow(SW, SH, "Assignment 3A - Translation");
    SetTargetFPS(60);

    float tx = 0, ty = 0;

    while (!WindowShouldClose())
    {
        if (IsKeyDown(KEY_RIGHT)) tx += 3;
        if (IsKeyDown(KEY_LEFT))  tx -= 3;
        if (IsKeyDown(KEY_UP))    ty += 3;
        if (IsKeyDown(KEY_DOWN))  ty -= 3;
        if (IsKeyPressed(KEY_R))  tx = ty = 0;

        tx = clampf(tx, -600, 600);
        ty = clampf(ty, -300, 300);

        Vec2 original[NV], moved[NV];
        getObject(original);
        for (int i = 0; i < NV; i++)
            moved[i] = translate2D(original[i], tx, ty);

        BeginDrawing();
        ClearBackground(BLACK);
        drawAxes();

        drawObject(original, WHITE);
        drawObject(moved, GREEN);
        drawSeg(centroid(original), centroid(moved), YELLOW);   // displacement vector

        DrawText("TRANSLATION", 30, 20, 40, WHITE);
        DrawText("x' = x + tx        y' = y + ty", 30, 70, 28, YELLOW);
        DrawText(TextFormat("tx = %.0f     ty = %.0f", tx, ty), 30, 110, 28, GREEN);
        DrawText("white = original     green = translated     yellow = displacement", 30, 910, 22, GRAY);
        DrawText("ARROWS: move     R: reset", 30, 945, 22, GRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
