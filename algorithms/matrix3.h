#ifndef MATRIX3_H
#define MATRIX3_H

#include "transform2d.h"

// 3x3 homogeneous matrices - composite transformations (Assignment 3)
// Column-vector convention: p' = M * p, so the matrix on the RIGHT is applied FIRST.
// Example (rotate about c, then move):  M = T(t) * T(c) * R * T(-c)

struct Mat3
{
    float m[3][3];
};

Mat3 identityMatrix();
Mat3 translationMatrix(float tx, float ty);
Mat3 scalingMatrix(float sx, float sy);
Mat3 rotationMatrix(float angleRad);

Mat3 multiply(const Mat3& a, const Mat3& b);   // a * b
Vec2 applyMatrix(const Mat3& M, Vec2 p);

#endif
