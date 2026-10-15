#include "canvas.h"
#include <algorithm>
using namespace std;

static bool buffered = false;
static int  cw = 0, ch = 0;
static vector<Color> pixels;
static Texture2D tex = {};
static bool texReady = false;

void canvasEnable(int width, int height)
{
    if (texReady && (width != cw || height != ch))
    {
        UnloadTexture(tex);
        texReady = false;
    }

    cw = width;
    ch = height;
    pixels.assign((size_t)width * height, BLACK);

    if (!texReady)
    {
        Image img = GenImageColor(width, height, BLACK);
        tex = LoadTextureFromImage(img);
        UnloadImage(img);
        texReady = true;
    }

    buffered = true;
}

void canvasDisable()  { buffered = false; }

void canvasShutdown()
{
    if (texReady)
    {
        UnloadTexture(tex);
        texReady = false;
    }
    buffered = false;
}

bool  canvasEnabled() { return buffered; }
int   canvasWidth()   { return cw; }
int   canvasHeight()  { return ch; }

void canvasClear(Color color)
{
    if (buffered)
        fill(pixels.begin(), pixels.end(), color);
}

void plot(int x, int y, Color color)
{
    if (!buffered)
    {
        DrawPixel(x, y, color);
        return;
    }

    if (x < 0 || x >= cw || y < 0 || y >= ch)
        return;
    pixels[(size_t)y * cw + x] = color;
}

Color canvasGet(int x, int y)
{
    if (!buffered || x < 0 || x >= cw || y < 0 || y >= ch)
        return { 0, 0, 0, 0 };
    return pixels[(size_t)y * cw + x];
}

Color* canvasPixels() { return pixels.data(); }

void canvasPresent(int x, int y)
{
    if (!buffered || !texReady)
        return;
    UpdateTexture(tex, pixels.data());
    DrawTexture(tex, x, y, WHITE);
}

vector<Color> canvasSnapshot() { return pixels; }

void canvasRestore(const vector<Color>& snapshot)
{
    if (snapshot.size() == pixels.size())
        pixels = snapshot;
}

bool sameColor(Color a, Color b)
{
    return a.r == b.r && a.g == b.g && a.b == b.b;
}
