#include "midpoint_circle.h"
#include "canvas.h"

void midpointCircle(int xc, int yc, int r, Color color)
{
    int x = 0;
    int y = r;
    int p = 1 - r;

    while (x <= y)
    {
        plot(xc + x, yc + y, color);
        plot(xc - x, yc + y, color);
        plot(xc + x, yc - y, color);
        plot(xc - x, yc - y, color);

        plot(xc + y, yc + x, color);
        plot(xc - y, yc + x, color);
        plot(xc + y, yc - x, color);
        plot(xc - y, yc - x, color);

        x++;

        if (p < 0)
            p += 2 * x + 1;
        else
        {
            y--;
            p += 2 * (x - y) + 1;
        }
    }
}
