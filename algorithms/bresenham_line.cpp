#include "bresenham_line.h"
#include "canvas.h"
#include <cstdlib>
#include <algorithm>
using namespace std;

void bresenhamLine(int x1, int y1, int x2, int y2, Color color)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int stepX = (x1 < x2) ? 1 : -1;
    int stepY = (y1 < y2) ? 1 : -1;

    // steep line: swap the roles of x and y
    bool steep = dy > dx;
    if (steep)
        swap(dx, dy);

    int p = 2 * dy - dx;
    int x = x1, y = y1;

    for (int i = 0; i <= dx; i++)
    {
        plot(x, y, color);

        if (p >= 0)
        {
            if (steep) x += stepX; else y += stepY;
            p -= 2 * dx;
        }

        if (steep) y += stepY; else x += stepX;
        p += 2 * dy;
    }
}
