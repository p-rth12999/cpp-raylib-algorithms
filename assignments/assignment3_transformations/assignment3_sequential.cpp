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
// ASSIGNMENT 3D - SEQUENTIAL TRANSFORMATIONS (step by step)
// Applies Scale, Rotate and Translate one after another and shows
// every intermediate shape. Order matters!
//   SPACE : next step      BACKSPACE : back to the start
//   O : swap order  (A: S->R->T   B: T->R->S)
//   Q/E rotation   Z/X scale   ARROWS translation   R : reset values
// ============================================================
int main()
{
    InitWindow(SW, SH, "Assignment 3D - Sequential Transformations");
    SetTargetFPS(60);

    float scale = 1.3f, angleDeg = 30, tx = 250, ty = -150;
    int stage = 0;
    bool orderA = true;

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_SPACE))     stage = min(stage + 1, 3);
        if (IsKeyPressed(KEY_BACKSPACE)) stage = 0;
        if (IsKeyPressed(KEY_O))         orderA = !orderA;
        if (IsKeyDown(KEY_E)) angleDeg += 1.0f;
        if (IsKeyDown(KEY_Q)) angleDeg -= 1.0f;
        if (IsKeyDown(KEY_X)) scale += 0.01f;
        if (IsKeyDown(KEY_Z)) scale -= 0.01f;
        if (IsKeyDown(KEY_RIGHT)) tx += 3;
        if (IsKeyDown(KEY_LEFT))  tx -= 3;
        if (IsKeyDown(KEY_UP))    ty += 3;
        if (IsKeyDown(KEY_DOWN))  ty -= 3;
        if (IsKeyPressed(KEY_R)) { scale = 1.3f; angleDeg = 30; tx = 250; ty = -150; }
        scale = clampf(scale, 0.3f, 1.8f);

        // stages[0] = original, stages[1..3] = after each step
        Vec2 stages[4][NV];
        getObject(stages[0]);

        Vec2 origin = { 0, 0 };
        const char* labels[4];
        labels[0] = "ORIGINAL";

        for (int s = 1; s <= 3; s++)
        {
            // which operation is step s ?   A: scale, rotate, translate   B: translate, rotate, scale
            int op = orderA ? s : (4 - s);   // 1 = scale, 2 = rotate, 3 = translate
            for (int i = 0; i < NV; i++)
            {
                Vec2 p = stages[s - 1][i];
                if (op == 1) p = scalePoint(p, scale, scale, origin);
                if (op == 2) p = rotatePoint(p, angleDeg * DEG2RAD, origin);
                if (op == 3) p = translatePoint(p, tx, ty);
                stages[s][i] = p;
            }
            labels[s] = (op == 1) ? "SCALE" : (op == 2) ? "ROTATE" : "TRANSLATE";
        }

        Color colours[4] = { WHITE, SKYBLUE, ORANGE, GREEN };

        BeginDrawing();
        ClearBackground(BLACK);
        drawAxes();

        for (int s = 0; s <= stage; s++)
        {
            drawObject(stages[s], colours[s]);
            Vec2 c = centroid(stages[s]);
            DrawText(TextFormat("%d. %s", s, labels[s]), toScreenX(c.x) - 50, toScreenY(c.y) - 12, 22, colours[s]);
        }

        DrawText("SEQUENTIAL TRANSFORMATIONS", 30, 20, 40, WHITE);
        DrawText(orderA ? "ORDER A :  Scale -> Rotate -> Translate   (all about the origin)"
                        : "ORDER B :  Translate -> Rotate -> Scale   (all about the origin)", 30, 70, 28, YELLOW);
        DrawText(TextFormat("scale = %.2f     angle = %.0f deg     t = (%.0f, %.0f)     step %d / 3", scale, angleDeg, tx, ty, stage), 30, 110, 28, GREEN);
        DrawText("Press O to swap the order: the final position is different, so the operations do not commute.", 30, 150, 22, RED);
        DrawText("SPACE: next step   BACKSPACE: restart   O: swap order   Q/E rotate   Z/X scale   ARROWS translate   R: reset", 30, 945, 22, GRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
