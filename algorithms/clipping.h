#ifndef CLIPPING_H
#define CLIPPING_H

// ============================================================
// LINE CLIPPING - Assignment 5
// Window is in SCREEN coordinates: ymin is the TOP edge, ymax the bottom.
// Both functions shorten (x1,y1)-(x2,y2) to the part inside the window.
// ============================================================

struct ClipWindow
{
    float xmin, ymin, xmax, ymax;
};

enum ClipResult
{
    CLIP_REJECTED,   // completely outside: nothing to draw
    CLIP_ACCEPTED,   // completely inside:  unchanged
    CLIP_CLIPPED     // partly inside:      endpoints were moved onto the window edge
};

// Cohen-Sutherland region code bits (T B R L)
enum { CLIP_LEFT = 1, CLIP_RIGHT = 2, CLIP_BOTTOM = 4, CLIP_TOP = 8 };

int clipOutcode(float x, float y, const ClipWindow& w);

// steps (optional) = number of edge intersections that were calculated
ClipResult cohenSutherland(float& x1, float& y1, float& x2, float& y2,
                           const ClipWindow& w, int* steps = nullptr);

// tEnter / tExit (optional) = the final parameters t0 and t1 of P(t) = P1 + t (P2 - P1)
ClipResult liangBarsky(float& x1, float& y1, float& x2, float& y2,
                       const ClipWindow& w, float* tEnter = nullptr, float* tExit = nullptr);

#endif
