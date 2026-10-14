#include "raylib.h"
#include <iostream>
#include <cmath>
using namespace std;

// ============================================================
// 2D Affine Transformations (Translation / Scaling / Rotation)
// ============================================================

// Reads number of edges and their coordinates into a[][]
void acceptShape(int a[20][20], int &m)
{
    cout << "\nEnter the Number Of Edges: ";
    cin >> m;
    cout << "\nEnter The Coordinates:\n";
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (j >= 2)
                a[i][j] = 1;
            else
                cin >> a[i][j];
        }
    }
}

// Multiplies shape matrix a[][] by transform matrix b[][], result in c[][]
void applyTransform(int a[20][20], float b[20][20], int c[20][20], int m)
{
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < m; j++)
        {
            c[i][j] = 0;
            for (int k = 0; k < m; k++)
            {
                c[i][j] += (a[i][k] * (int)b[k][j]);
            }
        }
    }
}

// Draws original shape (RED) and transformed shape (GREEN)
void drawShapes(int a[20][20], int c[20][20], int m)
{
    InitWindow(600, 600, "2D Transformations - Translation / Scaling / Rotation");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawLine(300, 0, 300, 600, GRAY);
        DrawLine(0, 300, 600, 300, GRAY);

        int i;
        for (i = 0; i < m - 1; i++)
        {
            DrawLine(300 + a[i][0], 300 - a[i][1],
                     300 + a[i + 1][0], 300 - a[i + 1][1], RED);
        }
        DrawLine(300 + a[0][0], 300 - a[0][1],
                 300 + a[i][0], 300 - a[i][1], RED);

        for (i = 0; i < m - 1; i++)
        {
            DrawLine(300 + c[i][0], 300 - c[i][1],
                     300 + c[i + 1][0], 300 - c[i + 1][1], GREEN);
        }
        DrawLine(300 + c[0][0], 300 - c[0][1],
                 300 + c[i][0], 300 - c[i][1], GREEN);

        DrawText("RED = Original   GREEN = Transformed", 20, 20, 18, WHITE);
        DrawText("Press ESC to exit", 20, 570, 16, LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();
}

int main()
{
    int m;
    int a[20][20], c[20][20];
    float b[20][20];
    int ch, tx, ty, sx, sy;
    float deg, theta;

    acceptShape(a, m);

    cout << "\nEnter your choice"
         << "\n1.Translation"
         << "\n2.Scaling"
         << "\n3.Rotation\n";
    cin >> ch;

    switch (ch)
    {
    case 1:
        cout << "\nTRANSLATION OPERATION\n";
        cout << "Enter value for tx and ty: ";
        cin >> tx >> ty;
        b[0][0] = b[2][2] = b[1][1] = 1;
        b[0][1] = b[0][2] = b[1][0] = b[1][2] = 0;
        b[2][0] = tx;
        b[2][1] = ty;
        applyTransform(a, b, c, m);
        drawShapes(a, c, m);
        break;

    case 2:
        cout << "\nSCALING OPERATION\n";
        cout << "Enter value for sx, sy: ";
        cin >> sx >> sy;
        b[0][0] = sx;
        b[1][1] = sy;
        b[0][1] = b[0][2] = b[1][0] = b[1][2] = 0;
        b[2][0] = b[2][1] = 0;
        b[2][2] = 1;
        applyTransform(a, b, c, m);
        drawShapes(a, c, m);
        break;

    case 3:
        cout << "\nROTATION OPERATION\n";
        cout << "Enter value for angle: ";
        cin >> deg;
        theta = deg * (3.14f / 180.0f);
        b[0][0] = b[1][1] = cos(theta);
        b[0][1] = sin(theta);
        b[1][0] = sin(-theta);
        b[0][2] = b[1][2] = b[2][0] = b[2][1] = 0;
        b[2][2] = 1;
        applyTransform(a, b, c, m);
        drawShapes(a, c, m);
        break;

    default:
        cout << "\nInvalid choice";
    }

    return 0;
}