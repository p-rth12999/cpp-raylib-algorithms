#include "raylib.h"
#include <cmath>
#include <algorithm>
using namespace std;

// ============================================================
// LINE ALGORITHM (Assignment 1 - DDA)
// ============================================================
void ddaLine(int x1, int y1, int x2, int y2, Color color)
{
    int delta_x = x2 - x1;
    int delta_y = y2 - y1;

    int length = max(abs(delta_x), abs(delta_y));

    if (length == 0)
    {
        DrawPixel(x1, y1, color);
        return;
    }

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

// ============================================================
// COORDINATE SYSTEM
// Maths convention: origin at the screen centre, x -> right, y -> UP.
// A positive angle therefore rotates COUNTER-CLOCKWISE (textbook style).
// ============================================================
const int SW = 1900;
const int SH = 1000;
const int OX = SW / 2;
const int OY = SH / 2;

struct Vec2
{
    float x, y;
};

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

// ============================================================
// BASIC TRANSFORMATIONS (point -> point)
// ============================================================
// Translation:  x' = x + tx ,  y' = y + ty
Vec2 translatePoint(Vec2 p, float tx, float ty)
{
    return { p.x + tx, p.y + ty };
}

// Scaling about a fixed point F:  x' = Fx + (x - Fx) * sx ,  y' = Fy + (y - Fy) * sy
Vec2 scalePoint(Vec2 p, float sx, float sy, Vec2 f)
{
    return { f.x + (p.x - f.x) * sx, f.y + (p.y - f.y) * sy };
}

// Rotation about a pivot R:  x' = Rx + dx*cos - dy*sin ,  y' = Ry + dx*sin + dy*cos
Vec2 rotatePoint(Vec2 p, float rad, Vec2 r)
{
    float c = cosf(rad), s = sinf(rad);
    float dx = p.x - r.x, dy = p.y - r.y;
    return { r.x + dx * c - dy * s, r.y + dx * s + dy * c };
}

float clampf(float v, float lo, float hi) { return max(lo, min(hi, v)); }

// ============================================================
// ASSIGNMENT 3C - ROTATION
//   Q / E (or LEFT/RIGHT) : rotate     P : pivot = origin / object centre / mouse
//   R : reset
// ============================================================
int main()
{
    InitWindow(SW, SH, "Assignment 3C - Rotation");
    SetTargetFPS(60);

    float angleDeg = 0;
    int pivotMode = 0;   // 0 origin, 1 object centre, 2 mouse position

    while (!WindowShouldClose())
    {
        if (IsKeyDown(KEY_E) || IsKeyDown(KEY_LEFT))  angleDeg += 1.5f;   // counter-clockwise
        if (IsKeyDown(KEY_Q) || IsKeyDown(KEY_RIGHT)) angleDeg -= 1.5f;   // clockwise
        if (IsKeyPressed(KEY_P)) pivotMode = (pivotMode + 1) % 3;
        if (IsKeyPressed(KEY_R)) angleDeg = 0;

        if (angleDeg > 360) angleDeg -= 360;
        if (angleDeg < -360) angleDeg += 360;

        Vec2 pivot = { 0, 0 };
        if (pivotMode == 1) pivot = CENTRE;
        if (pivotMode == 2)
        {
            Vector2 m = GetMousePosition();
            pivot = { m.x - OX, OY - m.y };
        }

        Vec2 original[NV], rotated[NV];
        getObject(original);
        for (int i = 0; i < NV; i++)
            rotated[i] = rotatePoint(original[i], angleDeg * DEG2RAD, pivot);

        BeginDrawing();
        ClearBackground(BLACK);
        drawAxes();

        drawObject(original, WHITE);
        drawObject(rotated, GREEN);
        drawCross(pivot, RED);

        const char* names[3] = { "ORIGIN", "OBJECT CENTRE", "MOUSE" };
        DrawText("ROTATION", 30, 20, 40, WHITE);
        DrawText("x' = xr + (x - xr)cos(t) - (y - yr)sin(t)", 30, 70, 28, YELLOW);
        DrawText("y' = yr + (x - xr)sin(t) + (y - yr)cos(t)", 30, 105, 28, YELLOW);
        DrawText(TextFormat("angle = %.1f deg      pivot = %s (%.0f, %.0f)", angleDeg, names[pivotMode], pivot.x, pivot.y), 30, 150, 28, GREEN);
        DrawText("Q/E or LEFT/RIGHT: rotate    P: change pivot    R: reset", 30, 945, 22, GRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
