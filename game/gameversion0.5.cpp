#include "raylib.h"
#include <cmath>
#include <algorithm>
#include <vector>
using namespace std;

// ============================================================
// SOFTWARE FRAME BUFFER
// fb = what is shown this frame, bg = static background (sky, grass,
// track, gate) built once. Every frame: fb = bg, draw moving things, upload.
// ============================================================
const int W = 1900;
const int H = 1000;

vector<Color> fb(W * H);
vector<Color> bg(W * H);

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
// LINE + CIRCLE ALGORITHMS (Assignments 1 & 2)
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
// 2D TRANSFORMATIONS (Assignment 3)
// ============================================================
struct Vec2
{
    float x, y;
};

Vec2 translate2D(Vec2 p, float tx, float ty)
{
    return { p.x + tx, p.y + ty };
}

Vec2 scale2D(Vec2 p, float sx, float sy, Vec2 pivot)
{
    return { pivot.x + (p.x - pivot.x) * sx,
             pivot.y + (p.y - pivot.y) * sy };
}

Vec2 rotate2D(Vec2 p, float angleRad, Vec2 pivot)
{
    float c = cosf(angleRad);
    float s = sinf(angleRad);
    float dx = p.x - pivot.x;
    float dy = p.y - pivot.y;
    return { pivot.x + dx * c - dy * s,
             pivot.y + dx * s + dy * c };
}

// ============================================================
// FILL ALGORITHMS (Assignment 4)
// ============================================================
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

// maxPixels: if the outline leaks and the fill grows bigger than this, it is undone
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
// LINE CLIPPING (NEW - Assignment 5)
// Both algorithms clip a segment against the clip window and
// report what happened, so the HUD can count the results.
// ============================================================
struct ClipWindow
{
    float xmin, ymin, xmax, ymax;
};

enum ClipResult { CLIP_REJECTED, CLIP_ACCEPTED, CLIP_CLIPPED };
enum ClipMode   { MODE_COHEN, MODE_LIANG, MODE_OFF };

// --- Cohen-Sutherland: 4-bit region codes (L R T B) ---
int outcode(float x, float y, const ClipWindow& w)
{
    int code = 0;
    if (x < w.xmin) code |= 1;        // LEFT
    else if (x > w.xmax) code |= 2;   // RIGHT
    if (y < w.ymin) code |= 4;        // TOP    (screen y grows downward)
    else if (y > w.ymax) code |= 8;   // BOTTOM
    return code;
}

ClipResult cohenSutherland(float& x1, float& y1, float& x2, float& y2, const ClipWindow& w)
{
    int c1 = outcode(x1, y1, w);
    int c2 = outcode(x2, y2, w);

    if ((c1 | c2) == 0)
        return CLIP_ACCEPTED;           // trivially inside

    while (true)
    {
        if ((c1 | c2) == 0)
            return CLIP_CLIPPED;        // both ends now inside
        if (c1 & c2)
            return CLIP_REJECTED;       // both on the same outside side

        int out = c1 ? c1 : c2;
        float x, y;

        if (out & 8)      { x = x1 + (x2 - x1) * (w.ymax - y1) / (y2 - y1); y = w.ymax; }
        else if (out & 4) { x = x1 + (x2 - x1) * (w.ymin - y1) / (y2 - y1); y = w.ymin; }
        else if (out & 2) { y = y1 + (y2 - y1) * (w.xmax - x1) / (x2 - x1); x = w.xmax; }
        else              { y = y1 + (y2 - y1) * (w.xmin - x1) / (x2 - x1); x = w.xmin; }

        if (out == c1) { x1 = x; y1 = y; c1 = outcode(x1, y1, w); }
        else           { x2 = x; y2 = y; c2 = outcode(x2, y2, w); }
    }
}

// --- Liang-Barsky: parametric  P(t) = P1 + t (P2 - P1),  t in [0,1] ---
ClipResult liangBarsky(float& x1, float& y1, float& x2, float& y2, const ClipWindow& w)
{
    float dx = x2 - x1;
    float dy = y2 - y1;

    float p[4] = { -dx, dx, -dy, dy };
    float q[4] = { x1 - w.xmin, w.xmax - x1, y1 - w.ymin, w.ymax - y1 };

    float t0 = 0.0f, t1 = 1.0f;

    for (int i = 0; i < 4; i++)
    {
        if (p[i] == 0)
        {
            if (q[i] < 0)
                return CLIP_REJECTED;   // parallel and outside
        }
        else
        {
            float r = q[i] / p[i];
            if (p[i] < 0) { if (r > t1) return CLIP_REJECTED; if (r > t0) t0 = r; }
            else          { if (r < t0) return CLIP_REJECTED; if (r < t1) t1 = r; }
        }
    }

    if (t0 == 0.0f && t1 == 1.0f)
        return CLIP_ACCEPTED;

    float nx1 = x1 + t0 * dx, ny1 = y1 + t0 * dy;
    float nx2 = x1 + t1 * dx, ny2 = y1 + t1 * dy;
    x1 = nx1; y1 = ny1; x2 = nx2; y2 = ny2;
    return CLIP_CLIPPED;
}

ClipMode   clipMode = MODE_COHEN;
ClipWindow clipWin  = { 40, 140, 1860, 950 };
int        clipStats[3] = { 0, 0, 0 };   // indexed by ClipResult

// Every effect line (kerbs, lane dashes, sparks) goes through here
void drawLineClipped(float x1, float y1, float x2, float y2, Color color)
{
    ClipResult r = CLIP_ACCEPTED;

    if (clipMode == MODE_COHEN)      r = cohenSutherland(x1, y1, x2, y2, clipWin);
    else if (clipMode == MODE_LIANG) r = liangBarsky(x1, y1, x2, y2, clipWin);

    clipStats[r]++;

    if (r == CLIP_REJECTED)
        return;

    ddaLine((int)roundf(x1), (int)roundf(y1), (int)roundf(x2), (int)roundf(y2), color);
}

// ============================================================
// ROAD GEOMETRY
// d = distance ahead of the car (0 = at the car, big = far away)
// u = position across the road (0 = left edge, 1 = right edge)
// ============================================================
const float CAR_D = 86.0f;   // the car sits at this depth (screen y = 765)

float depthToT(float d)
{
    return 1.0f / (1.0f + d / 300.0f);   // 1 = bottom of screen, 0 = horizon
}

Vec2 roadPoint(float u, float d)
{
    d = max(d, -200.0f);
    float t  = depthToT(d);
    float xl = 520 - 520 * t;
    float xr = 1380 + 520 * t;
    return { xl + u * (xr - xl), 50 + 920 * t };
}

// ============================================================
// CAR MODEL (same as version 0.4, drawn through the 0.3 transforms)
// ============================================================
enum { A, B, C, D, E, F, G, H_, P, Q, NUM_VERTS };

const Vec2 carModel[NUM_VERTS] =
{
    { -110,  125 }, {  110,  125 }, {  -90,  -75 }, {   90,  -75 },
    {  -50,  -75 }, {   50,  -75 }, {  -50, -125 }, {   50, -125 },
    {  -62,  -75 }, {   62,  -75 }
};

const int NUM_EDGES = 13;
const int carEdges[NUM_EDGES][2] =
{
    { A, B }, { A, C }, { B, D }, { C, G }, { D, H_ },
    { G, H_ }, { G, P }, { P, A }, { H_, Q }, { Q, B },
    { E, F }, { P, E }, { F, Q }
};

const Vec2 wheelModel[2] = { { -70, 125 }, { 70, 125 } };
const float WHEEL_RADIUS = 40.0f;

struct Car
{
    Vec2  pos;
    float angle;   // degrees, tilts when steering
    float roll;    // wheel spin
};

Vec2 modelToScreen(Vec2 local, const Car& car)
{
    Vec2 origin = { 0, 0 };
    Vec2 p = rotate2D(local, car.angle * DEG2RAD, origin);
    return translate2D(p, car.pos.x, car.pos.y);
}

void drawCar(const Car& car)
{
    Vec2 v[NUM_VERTS];
    for (int i = 0; i < NUM_VERTS; i++)
        v[i] = modelToScreen(carModel[i], car);

    // windshield (scan-line fill), then outlines
    scanLineFill({ v[G], v[P], v[E], v[F], v[Q], v[H_] }, SKYBLUE);

    for (int i = 0; i < NUM_EDGES; i++)
        ddaLine((int)roundf(v[carEdges[i][0]].x), (int)roundf(v[carEdges[i][0]].y),
                (int)roundf(v[carEdges[i][1]].x), (int)roundf(v[carEdges[i][1]].y), WHITE);

    Vec2 wc[2];
    for (int w = 0; w < 2; w++)
    {
        wc[w] = modelToScreen(wheelModel[w], car);
        bresenham_circle((int)roundf(wc[w].x), (int)roundf(wc[w].y), (int)WHEEL_RADIUS, WHITE);
    }

    // boundary fills
    Vec2 body   = modelToScreen({ 0, 35 }, car);
    Vec2 panelL = modelToScreen({ -84, -25 }, car);
    Vec2 panelR = modelToScreen({ 84, -25 }, car);

    boundaryFill4((int)roundf(body.x), (int)roundf(body.y), RED, WHITE, 80000);
    boundaryFill4((int)roundf(panelL.x), (int)roundf(panelL.y), MAROON, WHITE, 80000);
    boundaryFill4((int)roundf(panelR.x), (int)roundf(panelR.y), MAROON, WHITE, 80000);

    for (int w = 0; w < 2; w++)
    {
        Vec2 up = modelToScreen({ wheelModel[w].x, wheelModel[w].y - 18 }, car);
        Vec2 dn = modelToScreen({ wheelModel[w].x, wheelModel[w].y + 18 }, car);
        boundaryFill4((int)roundf(up.x), (int)roundf(up.y), DARKGRAY, WHITE, 20000);
        boundaryFill4((int)roundf(dn.x), (int)roundf(dn.y), DARKGRAY, WHITE, 20000);

        // rotating spokes (rotation transform)
        for (int k = 0; k < 2; k++)
        {
            float spin = car.roll + car.angle * DEG2RAD + k * (PI / 2);
            Vec2 tip = rotate2D({ wc[w].x + WHEEL_RADIUS * 0.8f, wc[w].y }, spin, wc[w]);
            Vec2 opp = { 2 * wc[w].x - tip.x, 2 * wc[w].y - tip.y };
            ddaLine((int)roundf(opp.x), (int)roundf(opp.y), (int)roundf(tip.x), (int)roundf(tip.y), LIGHTGRAY);
        }
    }
}

// ============================================================
// STATIC BACKGROUND (built once)
// ============================================================
void buildBackground()
{
    fill(fb.begin(), fb.end(), BLACK);

    // scan-line fills
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

    // outlines
    ddaLine(0, 970, 520, 50, WHITE);
    ddaLine(1900, 970, 1380, 50, WHITE);
    ddaLine(520, 50, 1380, 50, WHITE);
    ddaLine(0, 970, 1899, 970, WHITE);

    ddaLine(520, 120, 1380, 120, WHITE);
    ddaLine(520, 150, 1380, 150, WHITE);
    ddaLine(520, 180, 1380, 180, WHITE);
    ddaLine(520, 120, 520, 180, WHITE);
    ddaLine(1380, 120, 1380, 180, WHITE);
    for (int x = 560; x <= 1340; x += 80)
        ddaLine(x, 120, x, 180, WHITE);

    // flood fill the road surface
    floodFill4(950, 950, BLACK, GRAY);

    bg = fb;
}

// ============================================================
// GAME OBJECTS
// ============================================================
struct Entity
{
    float d;       // distance ahead
    float u;       // lane position 0..1
    bool  coin;    // true = coin, false = cone
    float phase;
};

struct Spark
{
    Vec2  p, v;
    float life;
};

void drawEntity(const Entity& e, float time)
{
    float t = depthToT(e.d);
    Vec2 p = roadPoint(e.u, e.d);

    if (e.coin)
    {
        int r  = max(3, (int)(38 * t));
        int cx = (int)roundf(p.x);
        int cy = (int)roundf(p.y) - r;

        bresenham_circle(cx, cy, r, WHITE);

        if (r >= 5)
        {
            boundaryFill4(cx, cy, YELLOW, WHITE, 4 * r * r + 50);

            Vec2 c = { (float)cx, (float)cy };
            Vec2 tip = rotate2D({ c.x, c.y - r * 0.6f }, time * 3 + e.phase, c);
            Vec2 opp = { 2 * c.x - tip.x, 2 * c.y - tip.y };
            ddaLine((int)roundf(opp.x), (int)roundf(opp.y), (int)roundf(tip.x), (int)roundf(tip.y), ORANGE);
        }
    }
    else
    {
        float w = 34 * t, h = 80 * t;
        Vec2 apex = { p.x, p.y - h };
        Vec2 bl   = { p.x - w, p.y };
        Vec2 br   = { p.x + w, p.y };

        scanLineFill({ apex, bl, br }, ORANGE);

        ddaLine((int)roundf(apex.x), (int)roundf(apex.y), (int)roundf(bl.x), (int)roundf(bl.y), WHITE);
        ddaLine((int)roundf(apex.x), (int)roundf(apex.y), (int)roundf(br.x), (int)roundf(br.y), WHITE);
        ddaLine((int)roundf(bl.x), (int)roundf(bl.y), (int)roundf(br.x), (int)roundf(br.y), WHITE);
    }
}

void drawRoadMarkings(float distance)
{
    // lane dashes: two dashed lines down the road
    float offset = fmodf(distance, 400.0f);
    for (int k = 0; k < 10; k++)
    {
        float d0 = k * 400.0f - offset;
        float d1 = d0 + 200.0f;
        for (int lane = 1; lane <= 2; lane++)
        {
            float u = lane / 3.0f;
            Vec2 a = roadPoint(u, d0);
            Vec2 b = roadPoint(u, d1);
            drawLineClipped(a.x, a.y, b.x, b.y, WHITE);
        }
    }

    // kerbs: red / white stripes just outside both road edges
    float off2 = fmodf(distance, 200.0f);
    int base = (int)(distance / 200.0f);
    for (int k = 0; k < 16; k++)
    {
        float d0 = k * 200.0f - off2;
        float d1 = d0 + 200.0f;
        Color col = ((k + base) % 2 == 0) ? RED : WHITE;

        for (int s = 0; s < 3; s++)
        {
            float uL = -0.012f - 0.012f * s;
            float uR = 1.012f + 0.012f * s;

            Vec2 a = roadPoint(uL, d0), b = roadPoint(uL, d1);
            Vec2 c = roadPoint(uR, d0), e = roadPoint(uR, d1);
            drawLineClipped(a.x, a.y, b.x, b.y, col);
            drawLineClipped(c.x, c.y, e.x, e.y, col);
        }
    }
}

const char* ordinal(int n)
{
    if (n == 1) return "ST";
    if (n == 2) return "ND";
    if (n == 3) return "RD";
    return "TH";
}

int main()
{
    InitWindow(W, H, "Game Version 0.5 - Line Clipping + Gameplay");
    SetTargetFPS(60);

    Image img = GenImageColor(W, H, BLACK);
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);

    buildBackground();

    // ---- game state ----
    vector<Entity> entities;
    vector<Spark>  sparks;

    float uCar = 0.5f;
    float tilt = 0.0f;
    float roll = 0.0f;
    float speed = 800.0f;
    float distance = 0.0f;
    float spawnTimer = 0.5f;
    float invuln = 0.0f;
    int   coins = 0;
    int   lives = 3;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        float time = (float)GetTime();
        bool over = (lives <= 0);

        /////////////////////////////////////////////////
        // INPUT
        /////////////////////////////////////////////////
        float steer = 0;
        if (!over)
        {
            if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) steer -= 1;
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) steer += 1;

            if (IsKeyDown(KEY_UP)   || IsKeyDown(KEY_W)) speed += 700 * dt;
            else if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) speed -= 1100 * dt;
            else speed += (800 - speed) * min(1.0f, 1.5f * dt);
            speed = max(300.0f, min(2200.0f, speed));
        }
        else
            speed = max(0.0f, speed - 1200 * dt);

        if (IsKeyPressed(KEY_C))
            clipMode = (ClipMode)((clipMode + 1) % 3);

        if (IsKeyPressed(KEY_LEFT_BRACKET))   // shrink clip window
        {
            clipWin.xmin = min(clipWin.xmin + 40, 800.0f);
            clipWin.xmax = max(clipWin.xmax - 40, 1100.0f);
            clipWin.ymin = min(clipWin.ymin + 30, 400.0f);
            clipWin.ymax = max(clipWin.ymax - 30, 600.0f);
        }
        if (IsKeyPressed(KEY_RIGHT_BRACKET))  // grow clip window
        {
            clipWin.xmin = max(clipWin.xmin - 40, 0.0f);
            clipWin.xmax = min(clipWin.xmax + 40, 1899.0f);
            clipWin.ymin = max(clipWin.ymin - 30, 140.0f);
            clipWin.ymax = min(clipWin.ymax + 30, 999.0f);
        }

        if (over && IsKeyPressed(KEY_ENTER))
        {
            entities.clear();
            sparks.clear();
            uCar = 0.5f; tilt = 0; speed = 800; distance = 0;
            coins = 0; lives = 3; invuln = 0; spawnTimer = 0.5f;
        }

        /////////////////////////////////////////////////
        // UPDATE
        /////////////////////////////////////////////////
        uCar += steer * 0.9f * dt;
        uCar = max(0.08f, min(0.92f, uCar));

        tilt += (steer * 8.0f - tilt) * min(1.0f, 10 * dt);
        roll += speed * dt * 0.01f;
        distance += speed * dt;
        invuln = max(0.0f, invuln - dt);

        if (!over)
        {
            spawnTimer -= dt;
            if (spawnTimer <= 0)
            {
                Entity e;
                e.d = 3000;
                e.u = 0.12f + GetRandomValue(0, 76) / 100.0f;
                e.coin = GetRandomValue(0, 99) < 65;
                e.phase = GetRandomValue(0, 628) / 100.0f;
                entities.push_back(e);
                spawnTimer = (350 + GetRandomValue(0, 350)) / speed;
            }
        }

        Vec2 carBase = roadPoint(uCar, CAR_D);

        for (Entity& e : entities)
            e.d -= speed * dt;

        // collisions
        for (size_t i = 0; i < entities.size(); )
        {
            Entity& e = entities[i];
            bool remove = false;

            if (!over && fabsf(e.d - CAR_D) < 60 && fabsf(e.u - uCar) < 0.12f)
            {
                if (e.coin)
                {
                    coins++;
                    remove = true;
                }
                else if (invuln <= 0)
                {
                    lives--;
                    invuln = 1.5f;
                    speed = max(300.0f, speed * 0.5f);
                    remove = true;

                    for (int k = 0; k < 24; k++)
                    {
                        float a = GetRandomValue(0, 628) / 100.0f;
                        float s = 200 + GetRandomValue(0, 500);
                        sparks.push_back({ { carBase.x, carBase.y - 60 }, { cosf(a) * s, sinf(a) * s }, 0.6f });
                    }
                }
            }

            if (e.d < -150) remove = true;

            if (remove) entities.erase(entities.begin() + i);
            else i++;
        }

        for (Spark& s : sparks)
        {
            s.p = translate2D(s.p, s.v.x * dt, s.v.y * dt);
            s.life -= dt;
        }
        sparks.erase(remove_if(sparks.begin(), sparks.end(),
                               [](const Spark& s) { return s.life <= 0; }), sparks.end());

        /////////////////////////////////////////////////
        // RENDER INTO THE FRAME BUFFER
        /////////////////////////////////////////////////
        fb = bg;
        clipStats[0] = clipStats[1] = clipStats[2] = 0;

        drawRoadMarkings(distance);

        sort(entities.begin(), entities.end(),
             [](const Entity& a, const Entity& b) { return a.d > b.d; });
        for (const Entity& e : entities)
            drawEntity(e, time);

        for (const Spark& s : sparks)
            drawLineClipped(s.p.x, s.p.y, s.p.x - s.v.x * 0.06f, s.p.y - s.v.y * 0.06f, YELLOW);

        Car car = { carBase, tilt, roll };
        bool blink = invuln > 0 && ((int)(invuln * 10) % 2 == 0);
        if (!blink)
            drawCar(car);

        // clip window frame
        ddaLine((int)clipWin.xmin, (int)clipWin.ymin, (int)clipWin.xmax, (int)clipWin.ymin, YELLOW);
        ddaLine((int)clipWin.xmax, (int)clipWin.ymin, (int)clipWin.xmax, (int)clipWin.ymax, YELLOW);
        ddaLine((int)clipWin.xmax, (int)clipWin.ymax, (int)clipWin.xmin, (int)clipWin.ymax, YELLOW);
        ddaLine((int)clipWin.xmin, (int)clipWin.ymax, (int)clipWin.xmin, (int)clipWin.ymin, YELLOW);

        UpdateTexture(tex, fb.data());

        /////////////////////////////////////////////////
        // HUD
        /////////////////////////////////////////////////
        int score = coins * 100 + (int)(distance / 100);
        int rank  = max(1, 10 - (int)(distance / 6000));
        const char* modeName[3] = { "COHEN-SUTHERLAND", "LIANG-BARSKY", "OFF" };

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(tex, 0, 0, WHITE);

        DrawText("SCORE", 90, 20, 50, WHITE);
        DrawText(TextFormat("%06d", score), 70, 90, 45, WHITE);
        DrawText(TextFormat("%d%s / 10", rank, ordinal(rank)), 1600, 20, 50, WHITE);
        DrawText(TextFormat("LIVES %d", lives), 70, 150, 35, lives > 1 ? WHITE : RED);
        DrawText(TextFormat("%d KM/H", (int)(speed / 5)), 1600, 90, 40, WHITE);

        DrawText(TextFormat("CLIP: %s", modeName[clipMode]), 1500, 150, 28, YELLOW);
        DrawText(TextFormat("inside %d | clipped %d | rejected %d",
                            clipStats[CLIP_ACCEPTED], clipStats[CLIP_CLIPPED], clipStats[CLIP_REJECTED]),
                 1400, 185, 24, YELLOW);

        DrawText("LEFT/RIGHT steer   UP/DOWN speed   C clip mode   [ ] clip window   ENTER restart",
                 20, 975, 22, WHITE);

        if (over)
        {
            DrawText("GAME OVER", 700, 400, 100, RED);
            DrawText("press ENTER to restart", 740, 520, 40, WHITE);
        }

        EndDrawing();
    }

    UnloadTexture(tex);
    CloseWindow();
    return 0;
}
