#include "clipping.h"

// ---------------- Cohen-Sutherland ----------------
int clipOutcode(float x, float y, const ClipWindow& w)
{
    int code = 0;

    if (x < w.xmin)      code |= CLIP_LEFT;
    else if (x > w.xmax) code |= CLIP_RIGHT;

    if (y < w.ymin)      code |= CLIP_TOP;      // screen y grows downward
    else if (y > w.ymax) code |= CLIP_BOTTOM;

    return code;
}

ClipResult cohenSutherland(float& x1, float& y1, float& x2, float& y2,
                           const ClipWindow& w, int* steps)
{
    if (steps) *steps = 0;

    int c1 = clipOutcode(x1, y1, w);
    int c2 = clipOutcode(x2, y2, w);

    if ((c1 | c2) == 0)
        return CLIP_ACCEPTED;               // both ends inside

    while (true)
    {
        if ((c1 | c2) == 0)
            return CLIP_CLIPPED;            // both ends are inside now
        if (c1 & c2)
            return CLIP_REJECTED;           // both ends outside on the same side

        int out = c1 ? c1 : c2;             // pick an endpoint that is outside
        float x, y;

        if (out & CLIP_BOTTOM)
        {
            x = x1 + (x2 - x1) * (w.ymax - y1) / (y2 - y1);
            y = w.ymax;
        }
        else if (out & CLIP_TOP)
        {
            x = x1 + (x2 - x1) * (w.ymin - y1) / (y2 - y1);
            y = w.ymin;
        }
        else if (out & CLIP_RIGHT)
        {
            y = y1 + (y2 - y1) * (w.xmax - x1) / (x2 - x1);
            x = w.xmax;
        }
        else
        {
            y = y1 + (y2 - y1) * (w.xmin - x1) / (x2 - x1);
            x = w.xmin;
        }

        if (steps) (*steps)++;

        if (out == c1) { x1 = x; y1 = y; c1 = clipOutcode(x1, y1, w); }
        else           { x2 = x; y2 = y; c2 = clipOutcode(x2, y2, w); }
    }
}

// ---------------- Liang-Barsky ----------------
ClipResult liangBarsky(float& x1, float& y1, float& x2, float& y2,
                       const ClipWindow& w, float* tEnter, float* tExit)
{
    float dx = x2 - x1;
    float dy = y2 - y1;

    // edges in the order: left, right, top, bottom
    float p[4] = { -dx, dx, -dy, dy };
    float q[4] = { x1 - w.xmin, w.xmax - x1, y1 - w.ymin, w.ymax - y1 };

    float t0 = 0.0f, t1 = 1.0f;

    if (tEnter) *tEnter = t0;
    if (tExit)  *tExit  = t1;

    for (int i = 0; i < 4; i++)
    {
        if (p[i] == 0)
        {
            if (q[i] < 0)
                return CLIP_REJECTED;       // parallel to this edge and outside it
        }
        else
        {
            float r = q[i] / p[i];

            if (p[i] < 0)                   // line enters the window across this edge
            {
                if (r > t1) return CLIP_REJECTED;
                if (r > t0) t0 = r;
            }
            else                            // line leaves the window across this edge
            {
                if (r < t0) return CLIP_REJECTED;
                if (r < t1) t1 = r;
            }
        }
    }

    if (tEnter) *tEnter = t0;
    if (tExit)  *tExit  = t1;

    if (t0 == 0.0f && t1 == 1.0f)
        return CLIP_ACCEPTED;

    float nx1 = x1 + t0 * dx, ny1 = y1 + t0 * dy;
    float nx2 = x1 + t1 * dx, ny2 = y1 + t1 * dy;

    x1 = nx1; y1 = ny1;
    x2 = nx2; y2 = ny2;
    return CLIP_CLIPPED;
}
