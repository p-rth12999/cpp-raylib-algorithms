#include "bresenham_line.h"
#include <algorithm>
#include <cmath>

using namespace std;

void bresenhamLine(int x1, int y1, int x2, int y2, Color color)
{
    // Always draw from left to right
    if (x1 > x2)
    {
        swap(x1, x2);
        swap(y1, y2);
    }

    int delta_x = x2 - x1;
    int delta_y = y2 - y1;

    // Determine Y direction
    int yStep;

    if (delta_y >= 0)
        yStep = 1;
    else
        yStep = -1;

    // Use magnitude of delta_y
    delta_y = abs(delta_y);

    int X, Y, Xend;
    int P;

    if (x1 < x2)
    {
        X = x1;
        Y = y1;
        Xend = x2;
    }
    else
    {
        X = x2;
        Y = y2;
        Xend = x1;
    }

    P = 2 * delta_y - delta_x;

    while (X <= Xend)
    {
        DrawPixel(X, Y, color);

        if (P < 0)
        {
            P = P + 2 * delta_y;
        }
        else
        {
            Y += yStep;
            P = P + 2 * delta_y - 2 * delta_x;
        }

        X++;
    }
}