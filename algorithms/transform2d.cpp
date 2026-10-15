#include "transform2d.h"
#include <cmath>

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
