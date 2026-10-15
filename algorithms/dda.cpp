#include "dda.h"
#include "canvas.h"
#include <cmath>
#include <algorithm>
using namespace std;

void ddaLine(int x1, int y1, int x2, int y2, Color color)
{
    int delta_x = x2 - x1;
    int delta_y = y2 - y1;

    int length = max(abs(delta_x), abs(delta_y));

    if (length == 0)
    {
        plot(x1, y1, color);
        return;
    }

    float x = x1;
    float y = y1;

    float dx = delta_x / (float)length;
    float dy = delta_y / (float)length;

    for (int i = 0; i <= length; i++)
    {
        plot((int)round(x), (int)round(y), color);
        x += dx;
        y += dy;
    }
}
