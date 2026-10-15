#include "fill.h"
#include "canvas.h"
#include <algorithm>
#include <cmath>
using namespace std;

static const int DX[8] = { 1, -1, 0, 0, 1, 1, -1, -1 };
static const int DY[8] = { 0, 0, 1, -1, 1, -1, 1, -1 };

// ---------------- flood fill ----------------
static int floodFillImpl(int sx, int sy, Color oldColor, Color newColor, int dirs)
{
    if (!canvasEnabled())
        return 0;

    int W = canvasWidth();
    int H = canvasHeight();
    if (sx < 0 || sx >= W || sy < 0 || sy >= H)
        return 0;

    Color* px = canvasPixels();
    if (!sameColor(px[sy * W + sx], oldColor) || sameColor(oldColor, newColor))
        return 0;

    vector<int> stack;
    px[sy * W + sx] = newColor;
    stack.push_back(sy * W + sx);
    int count = 1;

    while (!stack.empty())
    {
        int p = stack.back();
        stack.pop_back();
        int x = p % W;
        int y = p / W;

        for (int k = 0; k < dirs; k++)
        {
            int nx = x + DX[k];
            int ny = y + DY[k];

            if (nx < 0 || nx >= W || ny < 0 || ny >= H)
                continue;

            if (sameColor(px[ny * W + nx], oldColor))
            {
                px[ny * W + nx] = newColor;
                stack.push_back(ny * W + nx);
                count++;
            }
        }
    }
    return count;
}

int floodFill4(int x, int y, Color oldColor, Color newColor) { return floodFillImpl(x, y, oldColor, newColor, 4); }
int floodFill8(int x, int y, Color oldColor, Color newColor) { return floodFillImpl(x, y, oldColor, newColor, 8); }

// ---------------- boundary fill ----------------
static int boundaryFillImpl(int sx, int sy, Color fillColor, Color boundaryColor, int maxPixels, int dirs)
{
    if (!canvasEnabled())
        return 0;

    int W = canvasWidth();
    int H = canvasHeight();
    if (sx < 0 || sx >= W || sy < 0 || sy >= H)
        return 0;

    Color* px = canvasPixels();
    Color start = px[sy * W + sx];
    if (sameColor(start, boundaryColor) || sameColor(start, fillColor))
        return 0;

    vector<int> stack;
    vector<int> changed;     // kept so a leaking fill can be undone
    vector<Color> before;

    changed.push_back(sy * W + sx);
    before.push_back(start);
    px[sy * W + sx] = fillColor;
    stack.push_back(sy * W + sx);

    while (!stack.empty())
    {
        int p = stack.back();
        stack.pop_back();
        int x = p % W;
        int y = p / W;

        for (int k = 0; k < dirs; k++)
        {
            int nx = x + DX[k];
            int ny = y + DY[k];

            if (nx < 0 || nx >= W || ny < 0 || ny >= H)
                continue;

            Color c = px[ny * W + nx];
            if (sameColor(c, boundaryColor) || sameColor(c, fillColor))
                continue;

            changed.push_back(ny * W + nx);
            before.push_back(c);
            px[ny * W + nx] = fillColor;
            stack.push_back(ny * W + nx);

            if (maxPixels > 0 && (int)changed.size() > maxPixels)
            {
                for (size_t i = 0; i < changed.size(); i++)
                    px[changed[i]] = before[i];
                return -1;
            }
        }
    }
    return (int)changed.size();
}

int boundaryFill4(int x, int y, Color fillColor, Color boundaryColor, int maxPixels)
{
    return boundaryFillImpl(x, y, fillColor, boundaryColor, maxPixels, 4);
}

int boundaryFill8(int x, int y, Color fillColor, Color boundaryColor, int maxPixels)
{
    return boundaryFillImpl(x, y, fillColor, boundaryColor, maxPixels, 8);
}

// ---------------- scan-line fill ----------------
vector<float> scanLineCrossings(const vector<Vec2>& poly, int y)
{
    vector<float> xs;
    int n = (int)poly.size();

    for (int i = 0; i < n; i++)
    {
        Vec2 a = poly[i];
        Vec2 b = poly[(i + 1) % n];

        if (a.y == b.y)
            continue;                       // horizontal edges are skipped
        if (a.y > b.y)
            swap(a, b);

        // half-open rule [a.y, b.y): a vertex shared by two edges is counted once
        if (y >= a.y && y < b.y)
            xs.push_back(a.x + (y - a.y) * (b.x - a.x) / (b.y - a.y));
    }

    sort(xs.begin(), xs.end());
    return xs;
}

void scanLineFill(const vector<Vec2>& poly, Color color, int yLimit)
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

    int yEnd = min((int)ceilf(maxY), yLimit);

    for (int y = (int)floorf(minY); y < yEnd; y++)
    {
        vector<float> xs = scanLineCrossings(poly, y);

        // fill between pairs of crossings
        for (size_t k = 0; k + 1 < xs.size(); k += 2)
            for (int x = (int)ceilf(xs[k]); x <= (int)floorf(xs[k + 1]); x++)
                plot(x, y, color);
    }
}
