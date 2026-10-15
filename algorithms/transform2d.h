#ifndef TRANSFORM2D_H
#define TRANSFORM2D_H

// 2D point transformations - Assignment 3
// Angles are in RADIANS. With y pointing DOWN (screen space) a positive
// angle turns clockwise; with y pointing UP (maths space) it turns
// counter-clockwise.

struct Vec2
{
    float x, y;
};

// x' = x + tx ,  y' = y + ty
Vec2 translate2D(Vec2 p, float tx, float ty);

// x' = xf + (x - xf) * sx ,  y' = yf + (y - yf) * sy      (fixed point = pivot)
Vec2 scale2D(Vec2 p, float sx, float sy, Vec2 pivot);

// x' = xr + dx*cos - dy*sin ,  y' = yr + dx*sin + dy*cos   (dx = x - xr, dy = y - yr)
Vec2 rotate2D(Vec2 p, float angleRad, Vec2 pivot);

#endif
