#include "matrix3.h"
#include <cmath>

Mat3 identityMatrix()
{
    return { { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } } };
}

Mat3 translationMatrix(float tx, float ty)
{
    return { { { 1, 0, tx }, { 0, 1, ty }, { 0, 0, 1 } } };
}

Mat3 scalingMatrix(float sx, float sy)
{
    return { { { sx, 0, 0 }, { 0, sy, 0 }, { 0, 0, 1 } } };
}

Mat3 rotationMatrix(float angleRad)
{
    float c = cosf(angleRad);
    float s = sinf(angleRad);
    return { { { c, -s, 0 }, { s, c, 0 }, { 0, 0, 1 } } };
}

Mat3 multiply(const Mat3& a, const Mat3& b)
{
    Mat3 r = { { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } };

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < 3; k++)
                r.m[i][j] += a.m[i][k] * b.m[k][j];

    return r;
}

Vec2 applyMatrix(const Mat3& M, Vec2 p)
{
    return { M.m[0][0] * p.x + M.m[0][1] * p.y + M.m[0][2],
             M.m[1][0] * p.x + M.m[1][1] * p.y + M.m[1][2] };
}
