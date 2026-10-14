#include "raylib.h"
#include <cmath>
#include <algorithm>
#include <vector>
using namespace std;

// ============================================================
// SOFTWARE FRAME BUFFER
// Fill algorithms must READ pixels back, which raylib's
// DrawPixel cannot do. So every algorithm writes into our own
// pixel array, and the finished array is uploaded as a texture.
// ============================================================
const int W = 1900;
const int H = 1000;

vector<Color> fb(W * H);

inline bool sameColor(Color a, Color b)
{
    return a.r == b.r && a.g == b.g && a.b == b.b;
}

inline void putPixel(int x, int y, Color c)
{
    if (x < 0 || x >= W || y < 0 || y >= H)
        return;
    fb[y * W + x] = c;
}

inline Color getPixel(int x, int y)
{
    return fb[y * W + x];
}

// ============================================================
// LINE + CIRCLE ALGORITHMS (Assignments 1 & 2, now draw into fb)
// ============================================================
void ddaLine(int x1, int y1, int x2, int y2, Color color)
{
    int delta_x = x2 - x1;
    int delta_y = y2 - y1;

    int length = max(abs(delta_x), abs(delta_y));

    if (length == 0)
    {
        putPixel(x1, y1, color);
        return;
    }

    float x = x1;
    float y = y1;

    float dx = delta_x / (float)length;
    float dy = delta_y / (float)length;

    for (int i = 0; i <= length; i++)
    {
        putPixel(round(x), round(y), color);
        x += dx;
        y += dy;
    }
}

void bresenham_circle(int xc, int yc, int r, Color color)
{
    int x = 0;
    int y = r;

    int Limit = 0;
    int delta = 2 * (1 - r);

    while (x <= y)
    {
        putPixel(xc + x, yc + y, color);
        putPixel(xc - x, yc + y, color);
        putPixel(xc + x, yc - y, color);
        putPixel(xc - x, yc - y, color);

        putPixel(xc + y, yc + x, color);
        putPixel(xc - y, yc + x, color);
        putPixel(xc + y, yc - x, color);
        putPixel(xc - y, yc - x, color);

        int ro_delta;

        if (y <= Limit)
            break;

        if (delta < 0)
        {
            ro_delta = 2 * delta + 2 * y - 1;

            if (ro_delta <= 0)
            {
                x++;
                delta += 2 * x + 1;
            }
            else
            {
                x++;
                y--;
                delta += 2 * (x - y) + 2;
            }
        }
        else
        {
            ro_delta = 2 * delta - 2 * x - 1;

            if (ro_delta <= 0)
            {
                x++;
                y--;
                delta += 2 * (x - y) + 2;
            }
            else
            {
                y--;
                delta += -2 * y + 1;
            }
        }
    }
}

// ============================================================
// FILL ALGORITHMS (NEW - Assignment 4)
// All three are 4-connected and use an explicit stack / sorted
// span list, so there is no recursion and no stack overflow.
// ============================================================
struct Vec2
{
    float x, y;
};

// FLOOD FILL: replace every connected pixel of oldColor with newColor
void floodFill4(int sx, int sy, Color oldColor, Color newColor)
{
    if (sx < 0 || sx >= W || sy < 0 || sy >= H)
        return;
    if (!sameColor(getPixel(sx, sy), oldColor) || sameColor(oldColor, newColor))
        return;

    const int dx[4] = { 1, -1, 0, 0 };
    const int dy[4] = { 0, 0, 1, -1 };

    vector<int> stack;
    putPixel(sx, sy, newColor);
    stack.push_back(sy * W + sx);

    while (!stack.empty())
    {
        int p = stack.back();
        stack.pop_back();
        int x = p % W;
        int y = p / W;

        for (int k = 0; k < 4; k++)
        {
            int nx = x + dx[k];
            int ny = y + dy[k];

            if (nx < 0 || nx >= W || ny < 0 || ny >= H)
                continue;

            if (sameColor(getPixel(nx, ny), oldColor))
            {
                putPixel(nx, ny, newColor);
                stack.push_back(ny * W + nx);
            }
        }
    }
}

// BOUNDARY FILL: spread until the boundaryColor is reached.
// maxPixels is a safety net: if the outline leaks and the fill
// grows larger than maxPixels, everything is undone.
bool boundaryFill4(int sx, int sy, Color fillColor, Color boundaryColor, int maxPixels = 0)
{
    if (sx < 0 || sx >= W || sy < 0 || sy >= H)
        return false;

    Color start = getPixel(sx, sy);
    if (sameColor(start, boundaryColor) || sameColor(start, fillColor))
        return false;

    const int dx[4] = { 1, -1, 0, 0 };
    const int dy[4] = { 0, 0, 1, -1 };

    vector<int> stack;
    vector<int> changed;
    vector<Color> before;

    before.push_back(start);
    changed.push_back(sy * W + sx);
    putPixel(sx, sy, fillColor);
    stack.push_back(sy * W + sx);

    while (!stack.empty())
    {
        int p = stack.back();
        stack.pop_back();
        int x = p % W;
        int y = p / W;

        for (int k = 0; k < 4; k++)
        {
            int nx = x + dx[k];
            int ny = y + dy[k];

            if (nx < 0 || nx >= W || ny < 0 || ny >= H)
                continue;

            Color c = getPixel(nx, ny);
            if (sameColor(c, boundaryColor) || sameColor(c, fillColor))
                continue;

            before.push_back(c);
            changed.push_back(ny * W + nx);
            putPixel(nx, ny, fillColor);
            stack.push_back(ny * W + nx);

            if (maxPixels > 0 && (int)changed.size() > maxPixels)
            {
                for (size_t i = 0; i < changed.size(); i++)
                    fb[changed[i]] = before[i];
                return false;
            }
        }
    }
    return true;
}

// SCAN-LINE FILL: for every row, find where the polygon edges cross it,
// sort the crossings and fill between each pair. Works for concave polygons.
void scanLineFill(const vector<Vec2>& poly, Color color)
{
    int n = (int)poly.size();
    if (n < 3)
        return;

    float minY = poly[0].y, maxY = poly[0].y;
    for (const Vec2& p : poly)
    {
        minY = min(minY, p.y);
        maxY = max(maxY, p.y);
    }

    for (int y = (int)floorf(minY); y < (int)ceilf(maxY); y++)
    {
        vector<float> xs;

        for (int i = 0; i < n; i++)
        {
            Vec2 a = poly[i];
            Vec2 b = poly[(i + 1) % n];

            if (a.y == b.y)
                continue;
            if (a.y > b.y)
                swap(a, b);

            // half-open rule [a.y, b.y) so shared vertices are counted once
            if (y >= a.y && y < b.y)
                xs.push_back(a.x + (y - a.y) * (b.x - a.x) / (b.y - a.y));
        }

        sort(xs.begin(), xs.end());

        for (size_t k = 0; k + 1 < xs.size(); k += 2)
            for (int x = (int)ceilf(xs[k]); x <= (int)floorf(xs[k + 1]); x++)
                putPixel(x, y, color);
    }
}

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
    fill(fb.begin(), fb.end(), BLACK);
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

    Image img = GenImageColor(W, H, BLACK);
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);

    bool fillsOn = true;
    FillTimes ft = { 0, 0, 0 };

    buildScene(fillsOn, ft);
    UpdateTexture(tex, fb.data());

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_SPACE))
        {
            fillsOn = !fillsOn;
            buildScene(fillsOn, ft);
            UpdateTexture(tex, fb.data());
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(tex, 0, 0, WHITE);

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

    UnloadTexture(tex);
    CloseWindow();
    return 0;
}
