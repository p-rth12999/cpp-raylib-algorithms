#include "bresenham_circle.h"
#include "canvas.h"

void bresenham_circle(int xc, int yc, int r, Color color)
{
    int x = 0;
    int y = r;

    int Limit = 0;
    int delta = 2 * (1 - r);

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
