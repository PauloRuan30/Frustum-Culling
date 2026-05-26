#ifndef CULLING_MATH_H
#define CULLING_MATH_H

#include "raylib.h"

struct Plane {
    Vector3 normal;
    float distance;
};

enum CullState{     // COLORS
    OUTSIDE = 0,    // RED
    INSIDE = 1,     // GREEN
    INTERSECT = 2   // YELLOW
};

Plane NormalizePlane(Plane p);
void ExtractFrustumPlanes(Matrix vp, Plane planes[6]);
CullState BoxFrustumIntersect(BoundingBox box, Plane planes[6]);

#endif // CULLING_MATH_H