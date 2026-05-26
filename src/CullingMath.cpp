#include "CullingMath.h"
#include "raymath.h"
#include <cmath>

Plane NormalizePlane(Plane p) {
    float length = sqrtf(p.normal.x * p.normal.x + p.normal.y * p.normal.y + p.normal.z * p.normal.z);
    return {
        { p.normal.x / length, p.normal.y / length, p.normal.z / length },
        p.distance / length
    };
}

void ExtractFrustumPlanes(Matrix vp, Plane planes[6]) {
    planes[0] = NormalizePlane({ {vp.m3 + vp.m0, vp.m7 + vp.m4, vp.m11 + vp.m8}, vp.m15 + vp.m12 }); // Left
    planes[1] = NormalizePlane({ {vp.m3 - vp.m0, vp.m7 - vp.m4, vp.m11 - vp.m8}, vp.m15 - vp.m12 }); // Right
    planes[2] = NormalizePlane({ {vp.m3 + vp.m1, vp.m7 + vp.m5, vp.m11 + vp.m9}, vp.m15 + vp.m13 }); // Bottom
    planes[3] = NormalizePlane({ {vp.m3 - vp.m1, vp.m7 - vp.m5, vp.m11 - vp.m9}, vp.m15 - vp.m13 }); // Top
    planes[4] = NormalizePlane({ {vp.m3 + vp.m2, vp.m7 + vp.m6, vp.m11 + vp.m10}, vp.m15 + vp.m14 }); // Near
    planes[5] = NormalizePlane({ {vp.m3 - vp.m2, vp.m7 - vp.m6, vp.m11 - vp.m10}, vp.m15 - vp.m14 }); // Far
}

CullState BoxFrustumIntersect(BoundingBox box, Plane planes[6]) {
    CullState result = INSIDE; 

    for (int i = 0; i < 6; i++) {
        Vector3 p = box.min;
        Vector3 n = box.max;

        if (planes[i].normal.x >= 0) { p.x = box.max.x; n.x = box.min.x; }
        if (planes[i].normal.y >= 0) { p.y = box.max.y; n.y = box.min.y; }
        if (planes[i].normal.z >= 0) { p.z = box.max.z; n.z = box.min.z; }

        float distP = Vector3DotProduct(planes[i].normal, p) + planes[i].distance;
        float distN = Vector3DotProduct(planes[i].normal, n) + planes[i].distance;

        if (distP < 0) { return OUTSIDE; }
        if (distN < 0) { result = INTERSECT; }
    }
    return result;
}