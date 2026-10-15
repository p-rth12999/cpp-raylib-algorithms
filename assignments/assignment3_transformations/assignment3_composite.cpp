#include "raylib.h"
#include "dda.h"
#include "transform2d.h"
#include "matrix3.h"
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
// ASSIGNMENT 3E - COMPOSITE TRANSFORMATION (3x3 homogeneous matrix)
// Instead of three passes over the vertices, build ONE matrix
//        M = T(t) * T(c) * R(theta) * S(s) * T(-c)
// (scale and rotate about the object centre c, then translate)
// and apply it once. Column-vector convention: p' = M * p, so the
// matrix on the RIGHT is applied FIRST.
// The step-by-step result is computed too and compared with M.
//   ARROWS translate   Q/E rotate   Z/X scale   R reset
// ============================================================
int main()
{
    InitWindow(SW, SH, "Assignment 3E - Composite Transformation (Matrix)");
    SetTargetFPS(60);

    float scale = 1.0f, angleDeg = 0, tx = 0, ty = 0;

    while (!WindowShouldClose())
    {
        if (IsKeyDown(KEY_E)) angleDeg += 1.0f;
        if (IsKeyDown(KEY_Q)) angleDeg -= 1.0f;
        if (IsKeyDown(KEY_X)) scale += 0.01f;
        if (IsKeyDown(KEY_Z)) scale -= 0.01f;
        if (IsKeyDown(KEY_RIGHT)) tx += 3;
        if (IsKeyDown(KEY_LEFT))  tx -= 3;
        if (IsKeyDown(KEY_UP))    ty += 3;
        if (IsKeyDown(KEY_DOWN))  ty -= 3;
        if (IsKeyPressed(KEY_R)) { scale = 1; angleDeg = 0; tx = ty = 0; }
        scale = clampf(scale, 0.3f, 1.8f);
        tx = clampf(tx, -600, 600);
        ty = clampf(ty, -300, 300);

        // ---- build the composite matrix ----
        Mat3 M = translationMatrix(tx, ty);
        M = multiply(M, translationMatrix(CENTRE.x, CENTRE.y));
        M = multiply(M, rotationMatrix(angleDeg * DEG2RAD));
        M = multiply(M, scalingMatrix(scale, scale));
        M = multiply(M, translationMatrix(-CENTRE.x, -CENTRE.y));

        Vec2 original[NV], viaMatrix[NV];
        getObject(original);

        float maxError = 0;
        for (int i = 0; i < NV; i++)
        {
            viaMatrix[i] = applyMatrix(M, original[i]);

            // same thing step by step
            Vec2 p = scale2D(original[i], scale, scale, CENTRE);
            p = rotate2D(p, angleDeg * DEG2RAD, CENTRE);
            p = translate2D(p, tx, ty);

            maxError = max(maxError, max(fabsf(p.x - viaMatrix[i].x), fabsf(p.y - viaMatrix[i].y)));
        }

        BeginDrawing();
        ClearBackground(BLACK);
        drawAxes();

        drawObject(original, WHITE);
        drawObject(viaMatrix, GREEN);
        drawCross(CENTRE, RED);

        DrawText("COMPOSITE TRANSFORMATION", 30, 20, 40, WHITE);
        DrawText("M = T(t) . T(c) . R(theta) . S(s) . T(-c)        p' = M p", 30, 70, 28, YELLOW);
        DrawText(TextFormat("scale = %.2f     angle = %.0f deg     t = (%.0f, %.0f)", scale, angleDeg, tx, ty), 30, 110, 28, GREEN);

        for (int r = 0; r < 3; r++)
            DrawText(TextFormat("[ %8.3f  %8.3f  %8.2f ]", M.m[r][0], M.m[r][1], M.m[r][2]), 30, 160 + r * 32, 28, WHITE);

        DrawText(TextFormat("max difference vs step-by-step result: %.5f", maxError), 30, 275, 24, SKYBLUE);
        DrawText("ARROWS: translate    Q/E: rotate    Z/X: scale    R: reset", 30, 945, 22, GRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
