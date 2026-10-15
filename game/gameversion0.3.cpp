#include "raylib.h"
#include "dda.h"
#include "bresenham_circle.h"
#include "transform2d.h"
#include <cmath>
#include <algorithm>
using namespace std;

// Algorithms used here live in algorithms/:
//   dda, bresenham_circle (Assignments 1-2)   transform2d (Assignment 3)

void drawSeg(Vec2 a, Vec2 b, Color color)
{
    ddaLine((int)roundf(a.x), (int)roundf(a.y), (int)roundf(b.x), (int)roundf(b.y), color);
}

// ============================================================
// CAR MODEL  (local coordinates, origin = centre of the car)
// Same shape as Assignment 1 / 2, just re-expressed around (0,0)
// ============================================================
enum { A, B, C, D, E, F, G, H, NUM_VERTS };

const Vec2 carModel[NUM_VERTS] =
{
    { -110,  125 },   // A  front-left
    {  110,  125 },   // B  front-right
    {  -90,  -75 },   // C  side-left top
    {   90,  -75 },   // D  side-right top
    {  -50,  -75 },   // E  roof-left
    {   50,  -75 },   // F  roof-right
    {  -50, -125 },   // G  top-left
    {   50, -125 }    // H  top-right
};

const int NUM_EDGES = 9;
const int carEdges[NUM_EDGES][2] =
{
    { A, B }, { A, C }, { B, D },
    { E, F }, { C, G }, { D, H },
    { G, H }, { G, A }, { H, B }
};

const Vec2 wheelModel[2] = { { -70, 125 }, { 70, 125 } };
const float WHEEL_RADIUS = 40.0f;

struct Car
{
    Vec2  pos;      // translation
    float angle;    // rotation (degrees)
    float scale;    // uniform scale
    float roll;     // wheel spin (radians)
};

// SEQUENTIAL transformation:  Scale -> Rotate -> Translate
Vec2 modelToScreen(Vec2 local, const Car& car)
{
    Vec2 origin = { 0, 0 };
    Vec2 p = scale2D(local, car.scale, car.scale, origin);
    p = rotate2D(p, car.angle * DEG2RAD, origin);
    return translate2D(p, car.pos.x, car.pos.y);
}

void drawCar(const Car& car)
{
    Vec2 v[NUM_VERTS];
    for (int i = 0; i < NUM_VERTS; i++)
        v[i] = modelToScreen(carModel[i], car);

    for (int i = 0; i < NUM_EDGES; i++)
        drawSeg(v[carEdges[i][0]], v[carEdges[i][1]], WHITE);

    // Wheels: transformed centre + scaled radius + rotating spokes
    for (int w = 0; w < 2; w++)
    {
        Vec2 c = modelToScreen(wheelModel[w], car);
        float r = WHEEL_RADIUS * car.scale;

        bresenham_circle((int)roundf(c.x), (int)roundf(c.y), (int)r, RED);

        for (int k = 0; k < 2; k++)
        {
            float spin = car.roll + car.angle * DEG2RAD + k * (PI / 2);
            Vec2 tip = rotate2D({ c.x + r * 0.8f, c.y }, spin, c);
            Vec2 opp = { 2 * c.x - tip.x, 2 * c.y - tip.y };
            drawSeg(opp, tip, RED);
        }
    }
}

// Coin: bobs (translate), pulses (scale) and its slot spins (rotate)
void drawCoin(Vec2 base, float phase, float t)
{
    Vec2 c = translate2D(base, 0, 15 * sinf(t * 3 + phase));
    float s = 1.0f + 0.2f * sinf(t * 4 + phase);
    int r = (int)(25 * s);

    bresenham_circle((int)c.x, (int)c.y, r, YELLOW);

    Vec2 tip = rotate2D({ c.x, c.y - r * 0.6f }, t * 2 + phase, c);
    Vec2 opp = { 2 * c.x - tip.x, 2 * c.y - tip.y };
    drawSeg(opp, tip, ORANGE);
}

int main()
{
    InitWindow(1900, 1000, "Game Version 0.3 - 2D Transformations");
    SetTargetFPS(60);

    const Vec2 startPos = { 950, 765 };
    Car car = { startPos, 0.0f, 1.0f, 0.0f };

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        float t = (float)GetTime();

        /////////////////////////////////////////////////
        // INPUT  ->  TRANSLATE / ROTATE / SCALE the car
        /////////////////////////////////////////////////
        float dx = 0, dy = 0;
        if (IsKeyDown(KEY_RIGHT)) dx += 360 * dt;
        if (IsKeyDown(KEY_LEFT))  dx -= 360 * dt;
        if (IsKeyDown(KEY_DOWN))  dy += 360 * dt;
        if (IsKeyDown(KEY_UP))    dy -= 360 * dt;

        car.pos = translate2D(car.pos, dx, dy);
        car.pos.x = max(100.0f, min(1800.0f, car.pos.x));
        car.pos.y = max(200.0f, min(840.0f, car.pos.y));
        car.roll += (dx - dy) / (WHEEL_RADIUS * car.scale);

        if (IsKeyDown(KEY_Q)) car.angle -= 90 * dt;
        if (IsKeyDown(KEY_E)) car.angle += 90 * dt;
        if (IsKeyDown(KEY_Z)) car.scale -= 0.8f * dt;
        if (IsKeyDown(KEY_X)) car.scale += 0.8f * dt;
        car.scale = max(0.4f, min(1.8f, car.scale));

        if (IsKeyPressed(KEY_R))
            car = { startPos, 0.0f, 1.0f, 0.0f };

        BeginDrawing();
        ClearBackground(BLACK);

        /////////////////////////////////////////////////
        // TRACK
        /////////////////////////////////////////////////
        ddaLine(0, 970, 520, 50, WHITE);
        ddaLine(1900, 970, 1380, 50, WHITE);
        ddaLine(520, 50, 1380, 50, WHITE);

        /////////////////////////////////////////////////
        // START / FINISH GATE
        /////////////////////////////////////////////////
        ddaLine(520, 120, 1380, 120, WHITE);

        for (int x = 560; x <= 1340; x += 80)
            ddaLine(x, 120, x, 180, WHITE);

        ddaLine(520, 150, 1380, 150, WHITE);
        ddaLine(520, 180, 1380, 180, WHITE);

        /////////////////////////////////////////////////
        // RAINBOW ARCH - pulses by SCALING about its centre
        /////////////////////////////////////////////////
        Vec2 archPivot = { 950, 600 };
        float pulse = 1.0f + 0.05f * sinf(t * 2);

        for (int i = 0; i < 5; i++)
        {
            int halfWidth = 60 + i * 25;
            int vertexY   = 560 - i * 25;
            int legDrop   = 40;

            Vec2 v = scale2D({ 950.0f, (float)vertexY }, pulse, pulse, archPivot);
            Vec2 l = scale2D({ (float)(950 - halfWidth), (float)(vertexY + legDrop) }, pulse, pulse, archPivot);
            Vec2 r = scale2D({ (float)(950 + halfWidth), (float)(vertexY + legDrop) }, pulse, pulse, archPivot);

            drawSeg(v, l, WHITE);
            drawSeg(v, r, WHITE);
        }

        /////////////////////////////////////////////////
        // COINS  (translate + scale + rotate animation)
        /////////////////////////////////////////////////
        drawCoin({ 700, 300 }, 0.0f, t);
        drawCoin({ 1200, 300 }, 1.5f, t);

        /////////////////////////////////////////////////
        // CAR  (Scale -> Rotate -> Translate)
        /////////////////////////////////////////////////
        drawCar(car);

        /////////////////////////////////////////////////
        // HUD
        /////////////////////////////////////////////////
        DrawText("SCORE", 90, 20, 50, WHITE);
        DrawText("100000", 70, 90, 45, WHITE);
        DrawText("1ST / 10", 1600, 20, 50, WHITE);

        DrawText(TextFormat("POS (%.0f, %.0f)   ANGLE %.0f deg   SCALE %.2f",
                            car.pos.x, car.pos.y, car.angle, car.scale),
                 20, 935, 24, YELLOW);
        DrawText("ARROWS: translate    Q/E: rotate    Z/X: scale    R: reset", 20, 968, 24, GRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
