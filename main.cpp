#include "raylib.h"
#include <cmath>
#include <vector>
using namespace std;

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 700;

const Color CYAN       = {0,   255, 255, 255};
const Color LIGHTCYAN  = {180, 255, 255, 255};
const Color LIGHTBLUE  = {135, 206, 250, 255};
const Color LIGHTGREEN = {144, 238, 144, 255};

struct Point2D {
    int x, y;
    Point2D(int px = 0, int py = 0) : x(px), y(py) {}
};

// ============================================================
// CIRCLE ALGORITHMS
// ============================================================
void bresenham_circle(int xc, int yc, int r, Color color)
{
    int x = 0, y = r;
    int delta = 2 * (1 - r);

    while (x <= y)
    {
        DrawPixel(xc + x, yc + y, color); DrawPixel(xc - x, yc + y, color);
        DrawPixel(xc + x, yc - y, color); DrawPixel(xc - x, yc - y, color);
        DrawPixel(xc + y, yc + x, color); DrawPixel(xc - y, yc + x, color);
        DrawPixel(xc + y, yc - x, color); DrawPixel(xc - y, yc - x, color);

        if (y <= 0) break;

        int ro_delta;
        if (delta < 0) {
            ro_delta = 2 * delta + 2 * y - 1;
            if (ro_delta <= 0) { x++; delta += 2 * x + 1; }
            else { x++; y--; delta += 2 * (x - y) + 2; }
        } else {
            ro_delta = 2 * delta - 2 * x - 1;
            if (ro_delta <= 0) { x++; y--; delta += 2 * (x - y) + 2; }
            else { y--; delta += -2 * y + 1; }
        }
    }
}

void midpoint_circle(int xc, int yc, int r, Color color)
{
    int x = 0, y = r;
    int p = 1 - r;

    while (x <= y)
    {
        DrawPixel(xc + x, yc + y, color); DrawPixel(xc - x, yc + y, color);
        DrawPixel(xc + x, yc - y, color); DrawPixel(xc - x, yc - y, color);
        DrawPixel(xc + y, yc + x, color); DrawPixel(xc - y, yc + x, color);
        DrawPixel(xc + y, yc - x, color); DrawPixel(xc - y, yc - x, color);

        x++;
        if (p < 0) p += 2 * x + 1;
        else { y--; p += 2 * (x - y) + 1; }
    }
}

// ============================================================
// LINE ALGORITHMS (unchanged from File 2)
// ============================================================
void drawDDALine(int x1, int y1, int x2, int y2, Color color) {
    float dx = x2 - x1, dy = y2 - y1;
    int steps = max(abs((int)dx), abs((int)dy));
    if (steps == 0) { DrawPixel(x1, y1, color); return; }
    float xInc = dx / (float)steps, yInc = dy / (float)steps;
    float x = x1, y = y1;
    for (int i = 0; i <= steps; ++i) {
        DrawPixel((int)round(x), (int)round(y), color);
        x += xInc; y += yInc;
    }
}

void drawBresenhamLine(int x1, int y1, int x2, int y2, Color color) {
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1, sy = (y1 < y2) ? 1 : -1;
    int x = x1, y = y1;
    DrawPixel(x, y, color);
    if (dx >= dy) {
        int p = 2 * dy - dx;
        while (x != x2) {
            x += sx;
            if (p < 0) p += 2 * dy; else { y += sy; p += 2 * dy - 2 * dx; }
            DrawPixel(x, y, color);
        }
    } else {
        int p = 2 * dx - dy;
        while (y != y2) {
            y += sy;
            if (p < 0) p += 2 * dx; else { x += sx; p += 2 * dx - 2 * dy; }
            DrawPixel(x, y, color);
        }
    }
}

void drawDashedLine(int x1, int y1, int x2, int y2, int dashLen, int gapLen, Color color) {
    float dx = x2 - x1, dy = y2 - y1;
    int steps = max(abs((int)dx), abs((int)dy));
    if (steps == 0) { DrawPixel(x1, y1, color); return; }
    float xInc = dx / (float)steps, yInc = dy / (float)steps;
    float x = x1, y = y1;
    int cycle = dashLen + gapLen;
    for (int i = 0; i <= steps; ++i) {
        if ((i % cycle) < dashLen) DrawPixel((int)round(x), (int)round(y), color);
        x += xInc; y += yInc;
    }
}

void drawTrackLayout(const vector<Point2D>& outer, const vector<Point2D>& inner) {
    int outerCount = outer.size(), innerCount = inner.size();
    for (int i = 0; i < outerCount; ++i) {
        int next = (i + 1) % outerCount;
        drawBresenhamLine(outer[i].x, outer[i].y, outer[next].x, outer[next].y, YELLOW);
    }
    for (int i = 0; i < innerCount; ++i) {
        int next = (i + 1) % innerCount;
        drawBresenhamLine(inner[i].x, inner[i].y, inner[next].x, inner[next].y, WHITE);
    }
    for (int i = 0; i < outerCount; ++i) {
        int next = (i + 1) % outerCount;
        Point2D mid1((outer[i].x + inner[i].x) / 2, (outer[i].y + inner[i].y) / 2);
        Point2D mid2((outer[next].x + inner[next].x) / 2, (outer[next].y + inner[next].y) / 2);
        drawDashedLine(mid1.x, mid1.y, mid2.x, mid2.y, 10, 8, CYAN);
    }
}

void drawStartFinishGrid(Point2D outerP, Point2D innerP) {
    drawBresenhamLine(outerP.x, outerP.y, innerP.x, innerP.y, WHITE);
    int offset = 12;
    drawDDALine(outerP.x - offset, outerP.y, innerP.x - offset, innerP.y, LIGHTGRAY);
    drawDDALine(outerP.x + offset, outerP.y, innerP.x + offset, innerP.y, LIGHTGRAY);
    int segments = 8;
    float dx = (innerP.x - outerP.x) / (float)segments;
    float dy = (innerP.y - outerP.y) / (float)segments;
    for (int i = 0; i < segments; ++i) {
        int xStart = (int)round(outerP.x + i * dx);
        int yStart = (int)round(outerP.y + i * dy);
        if (i % 2 == 0) drawBresenhamLine(xStart - offset, yStart, xStart + offset, yStart, WHITE);
    }
}

// ============================================================
// KART CHASSIS — wheels now drawn via circle algorithms
// ============================================================
void drawKartChassis(int cx, int cy, float scale = 1.0f) {
    int w = 26 * scale;
    int h = 42 * scale;

    // Front wheels — Bresenham circles
    int flCx = cx - w * 0.6f, flCy = cy - h * 0.3f;
    int frCx = cx + w * 0.6f, frCy = cy - h * 0.3f;
    int frontR = (int)(w * 0.16f);
    bresenham_circle(flCx, flCy, frontR, LIGHTGRAY);
    bresenham_circle(frCx, frCy, frontR, LIGHTGRAY);
    drawDDALine(flCx + frontR, flCy, frCx - frontR, frCy, LIGHTGRAY); // front axle

    // Rear wheels — Midpoint circles
    int rlCx = cx - w * 0.675f, rlCy = cy + h * 0.26f;
    int rrCx = cx + w * 0.675f, rrCy = cy + h * 0.26f;
    int rearR = (int)(w * 0.19f);
    midpoint_circle(rlCx, rlCy, rearR, LIGHTGRAY);
    midpoint_circle(rrCx, rrCy, rearR, LIGHTGRAY);
    drawDDALine(rlCx + rearR, rlCy, rrCx - rearR, rrCy, LIGHTGRAY); // rear axle

    // Cockpit / monocoque
    int mx1 = cx - w * 0.35f, my1 = cy - h * 0.20f;
    int mx2 = cx + w * 0.35f, my2 = cy + h * 0.35f;
    drawBresenhamLine(mx1, my1, mx2, my1, LIGHTCYAN);
    drawBresenhamLine(mx2, my1, mx2, my2, LIGHTCYAN);
    drawBresenhamLine(mx2, my2, mx1, my2, LIGHTCYAN);
    drawBresenhamLine(mx1, my2, mx1, my1, LIGHTCYAN);

    // Side pods
    drawBresenhamLine(cx - w * 0.50f, cy - h * 0.05f, mx1, cy - h * 0.15f, CYAN);
    drawBresenhamLine(cx - w * 0.50f, cy - h * 0.05f, cx - w * 0.50f, cy + h * 0.25f, CYAN);
    drawBresenhamLine(cx - w * 0.50f, cy + h * 0.25f, mx1, cy + h * 0.30f, CYAN);
    drawBresenhamLine(cx + w * 0.50f, cy - h * 0.05f, mx2, cy - h * 0.15f, CYAN);
    drawBresenhamLine(cx + w * 0.50f, cy - h * 0.05f, cx + w * 0.50f, cy + h * 0.25f, CYAN);
    drawBresenhamLine(cx + w * 0.50f, cy + h * 0.25f, mx2, cy + h * 0.30f, CYAN);

    // Nosecone & front wing
    drawBresenhamLine(mx1, my1, cx, cy - h * 0.55f, GREEN);
    drawBresenhamLine(cx, cy - h * 0.55f, mx2, my1, GREEN);
    int fwx1 = cx - w * 0.85f, fwx2 = cx + w * 0.85f, fwy = cy - h * 0.52f;
    drawDDALine(fwx1, fwy, fwx2, fwy, GREEN);
    drawDDALine(fwx1, fwy - 4, fwx2, fwy - 4, GREEN);
    drawBresenhamLine(fwx1, fwy - 4, fwx1, fwy + 3, GREEN);
    drawBresenhamLine(fwx2, fwy - 4, fwx2, fwy + 3, GREEN);

    // Driver helmet + steering wheel
    drawDDALine(cx - 5, cy - 8, cx + 5, cy - 8, LIGHTBLUE);
    drawBresenhamLine(cx - 4, cy - 2, cx + 4, cy - 2, YELLOW);
    drawBresenhamLine(cx + 4, cy - 2, cx + 4, cy + 8, YELLOW);
    drawBresenhamLine(cx + 4, cy + 8, cx - 4, cy + 8, YELLOW);
    drawBresenhamLine(cx - 4, cy + 8, cx - 4, cy - 2, YELLOW);
    drawDDALine(cx - 3, cy, cx + 3, cy, WHITE);

    // Rear wing + spoiler
    int rwx1 = cx - w * 0.80f, rwx2 = cx + w * 0.80f;
    int rwy1 = cy + h * 0.44f, rwy2 = cy + h * 0.52f;
    drawDDALine(rwx1, rwy1, rwx2, rwy1, RED);
    drawDDALine(rwx1, rwy2, rwx2, rwy2, RED);
    drawBresenhamLine(rwx1, rwy1, rwx1, rwy2, RED);
    drawBresenhamLine(rwx2, rwy1, rwx2, rwy2, RED);
    drawBresenhamLine(cx - w * 0.25f, cy + h * 0.35f, cx - w * 0.25f, rwy1, LIGHTGRAY);
    drawBresenhamLine(cx + w * 0.25f, cy + h * 0.35f, cx + w * 0.25f, rwy1, LIGHTGRAY);
}

void drawHUD() {
    DrawText("RETRO KART CGG - ASSIGNMENT 2", 230, 15, 30, LIGHTGREEN);
    DrawText("Phase 2: Circular Wheels via Bresenham & Midpoint Circle Algorithms", 190, 48, 18, WHITE);

    int lx1 = 20, ly1 = 520, lx2 = 480, ly2 = 680;
    drawBresenhamLine(lx1, ly1, lx2, ly1, LIGHTGRAY);
    drawBresenhamLine(lx2, ly1, lx2, ly2, LIGHTGRAY);
    drawBresenhamLine(lx2, ly2, lx1, ly2, LIGHTGRAY);
    drawBresenhamLine(lx1, ly2, lx1, ly1, LIGHTGRAY);

    DrawText("ALGORITHM COLOR LEGEND:", lx1 + 15, ly1 + 10, 18, YELLOW);

    drawBresenhamLine(lx1 + 15, ly1 + 45, lx1 + 45, ly1 + 45, YELLOW);
    DrawText("Bresenham Line -> Outer Track & Kart", lx1 + 55, ly1 + 37, 16, WHITE);

    drawDDALine(lx1 + 15, ly1 + 80, lx1 + 45, ly1 + 80, CYAN);
    DrawText("DDA Line -> Dashed Lanes & Grid", lx1 + 55, ly1 + 72, 16, WHITE);

    bresenham_circle(lx1 + 30, ly1 + 115, 12, LIGHTGRAY);
    DrawText("Bresenham Circle -> Front Wheels", lx1 + 55, ly1 + 107, 16, WHITE);

    midpoint_circle(lx1 + 30, ly1 + 150, 12, LIGHTGRAY);
    DrawText("Midpoint Circle -> Rear Wheels", lx1 + 55, ly1 + 142, 16, WHITE);

    DrawText("Press ESC to Exit Application...", 600, 650, 18, LIGHTBLUE);
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Retro Kart CGG - Assignment 2: Circles (v0.2-circles)");
    SetTargetFPS(60);

    vector<Point2D> outerTrack = {
        Point2D(120, 500), Point2D(120, 200), Point2D(220, 100), Point2D(500, 100),
        Point2D(650, 180), Point2D(780, 120), Point2D(920, 220), Point2D(920, 480),
        Point2D(750, 600), Point2D(450, 600), Point2D(320, 500)
    };
    vector<Point2D> innerTrack = {
        Point2D(220, 460), Point2D(220, 230), Point2D(280, 180), Point2D(480, 180),
        Point2D(610, 250), Point2D(760, 200), Point2D(820, 270), Point2D(820, 430),
        Point2D(700, 510), Point2D(460, 510), Point2D(360, 440)
    };

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        drawTrackLayout(outerTrack, innerTrack);
        drawStartFinishGrid(outerTrack[0], innerTrack[0]);

        int kartX = (outerTrack[0].x + innerTrack[0].x) / 2;
        int kartY = (outerTrack[0].y + innerTrack[0].y) / 2 - 65;
        drawKartChassis(kartX, kartY, 1.3f);

        drawHUD();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}