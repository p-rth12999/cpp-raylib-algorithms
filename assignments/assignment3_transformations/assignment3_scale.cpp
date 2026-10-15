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
// ASSIGNMENT 3B - SCALING
//   LEFT/RIGHT : sx      UP/DOWN : sy
//   U : uniform on/off   F : fixed point = origin / object centre
//   R : reset
// ============================================================
int main()
{
    InitWindow(SW, SH, "Assignment 3B - Scaling");
    SetTargetFPS(60);

    float sx = 1.0f, sy = 1.0f;
    bool uniform = true;
    bool aboutCentre = false;

    while (!WindowShouldClose())
    {
        float step = 0.015f;
        if (IsKeyDown(KEY_RIGHT)) { sx += step; if (uniform) sy = sx; }
        if (IsKeyDown(KEY_LEFT))  { sx -= step; if (uniform) sy = sx; }
        if (IsKeyDown(KEY_UP))    { sy += step; if (uniform) sx = sy; }
        if (IsKeyDown(KEY_DOWN))  { sy -= step; if (uniform) sx = sy; }
        if (IsKeyPressed(KEY_U))  { uniform = !uniform; if (uniform) sy = sx; }
        if (IsKeyPressed(KEY_F))  aboutCentre = !aboutCentre;
        if (IsKeyPressed(KEY_R))  { sx = sy = 1.0f; }

        sx = clampf(sx, 0.2f, 2.2f);
        sy = clampf(sy, 0.2f, 2.2f);

        Vec2 original[NV], scaled[NV];
        getObject(original);

        Vec2 fixedPt = aboutCentre ? CENTRE : Vec2{ 0, 0 };
        for (int i = 0; i < NV; i++)
            scaled[i] = scale2D(original[i], sx, sy, fixedPt);

        BeginDrawing();
        ClearBackground(BLACK);
        drawAxes();

        drawObject(original, WHITE);
        drawObject(scaled, GREEN);
        drawCross(fixedPt, RED);

        DrawText("SCALING", 30, 20, 40, WHITE);
        DrawText("x' = xf + (x - xf) * sx        y' = yf + (y - yf) * sy", 30, 70, 28, YELLOW);
        DrawText(TextFormat("sx = %.2f     sy = %.2f     (%s)", sx, sy, uniform ? "uniform" : "non-uniform"), 30, 110, 28, GREEN);
        DrawText(aboutCentre ? "fixed point (red) = object centre -> shape grows in place"
                             : "fixed point (red) = ORIGIN -> shape also moves away from / towards the origin",
                 30, 150, 24, RED);
        DrawText("LEFT/RIGHT: sx    UP/DOWN: sy    U: uniform    F: fixed point    R: reset", 30, 945, 22, GRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
