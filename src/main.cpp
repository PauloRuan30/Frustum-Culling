#include "raylib.h"
#include "raymath.h"
#include <cmath>

// Define our 6 Frustum planes
struct Plane {
    Vector3 normal;
    float distance;
};

// Helper to normalize plane equations
Plane NormalizePlane(Plane p) {
    float length = sqrtf(p.normal.x * p.normal.x + p.normal.y * p.normal.y + p.normal.z * p.normal.z);
    return {
        { p.normal.x / length, p.normal.y / length, p.normal.z / length },
        p.distance / length
    };
}

// State Enum for our Culling Status
enum CullState {
    OUTSIDE = 0, // Red
    INSIDE = 1,  // Green
    INTERSECT = 2 // Yellow
};

// Extracts the 6 planes from a View-Projection Matrix
void ExtractFrustumPlanes(Matrix vp, Plane planes[6]) {
    // Left
    planes[0] = NormalizePlane({ {vp.m3 + vp.m0, vp.m7 + vp.m4, vp.m11 + vp.m8}, vp.m15 + vp.m12 });
    // Right
    planes[1] = NormalizePlane({ {vp.m3 - vp.m0, vp.m7 - vp.m4, vp.m11 - vp.m8}, vp.m15 - vp.m12 });
    // Bottom
    planes[2] = NormalizePlane({ {vp.m3 + vp.m1, vp.m7 + vp.m5, vp.m11 + vp.m9}, vp.m15 + vp.m13 });
    // Top
    planes[3] = NormalizePlane({ {vp.m3 - vp.m1, vp.m7 - vp.m5, vp.m11 - vp.m9}, vp.m15 - vp.m13 });
    // Near
    planes[4] = NormalizePlane({ {vp.m3 + vp.m2, vp.m7 + vp.m6, vp.m11 + vp.m10}, vp.m15 + vp.m14 });
    // Far
    planes[5] = NormalizePlane({ {vp.m3 - vp.m2, vp.m7 - vp.m6, vp.m11 - vp.m10}, vp.m15 - vp.m14 });
}

// Tests an AABB against the 6 Frustum planes
CullState BoxFrustumIntersect(BoundingBox box, Plane planes[6]) {
    CullState result = INSIDE; 

    for (int i = 0; i < 6; i++) {
        // Find the positive (p) and negative (n) vertices along the plane normal
        Vector3 p = box.min;
        Vector3 n = box.max;

        if (planes[i].normal.x >= 0) { p.x = box.max.x; n.x = box.min.x; }
        if (planes[i].normal.y >= 0) { p.y = box.max.y; n.y = box.min.y; }
        if (planes[i].normal.z >= 0) { p.z = box.max.z; n.z = box.min.z; }

        // Distance formula: Dot(Normal, Vertex) + Distance
        float distP = Vector3DotProduct(planes[i].normal, p) + planes[i].distance;
        float distN = Vector3DotProduct(planes[i].normal, n) + planes[i].distance;

        if (distP < 0) {
            return OUTSIDE; // The most positive vertex is outside, so the whole box is outside
        }
        if (distN < 0) {
            result = INTERSECT; // The negative vertex is outside, but positive is inside (Partial)
        }
    }
    return result;
}

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Vision Culling Showcase - C++ & Raylib");

    // Initialize First Person Camera
    Camera3D camera = { 0 };
    camera.position = { 0.0f, 2.0f, 15.0f };
    camera.target = { 0.0f, 2.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    DisableCursor(); 

    // Create a 3x3 Grid of 9 Cubes
    BoundingBox grid[9];
    int index = 0;
    float cubeSize = 4.0f;
    for (int x = -1; x <= 1; x++) {
        for (int z = -1; z <= 1; z++) {
            Vector3 center = { x * 10.0f, 2.0f, z * 10.0f };
            grid[index].min = { center.x - cubeSize/2, center.y - cubeSize/2, center.z - cubeSize/2 };
            grid[index].max = { center.x + cubeSize/2, center.y + cubeSize/2, center.z + cubeSize/2 };
            index++;
        }
    }

    Plane frustumPlanes[6];
    bool debugLock = false; // Toggles freezing the culling frustum
    Matrix lockedVP = { 0 }; // Stores the VP matrix when locked

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // 1. Input & Updates
        UpdateCamera(&camera, CAMERA_FIRST_PERSON);

        if (IsKeyPressed(KEY_SPACE)) {
            debugLock = !debugLock;
        }

        // 2. Math: Build View and Projection Matrices
        Matrix view = GetCameraMatrix(camera);
        float aspect = (float)screenWidth / (float)screenHeight;
        Matrix proj = MatrixPerspective(camera.fovy * DEG2RAD, aspect, 0.1f, 1000.0f);
        Matrix viewProj = MatrixMultiply(view, proj);

        // Update culling matrix ONLY if not debugging (locked)
        if (!debugLock) {
            lockedVP = viewProj;
        }

        // Extract planes from the active (or locked) View-Projection matrix
        ExtractFrustumPlanes(lockedVP, frustumPlanes);

        // 3. Render Setup
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
            DrawGrid(50, 1.0f); // Base floor grid

            // 4. Test Culling and Draw Grid Objects
            int visibleCount = 0;
            for (int i = 0; i < 9; i++) {
                CullState state = BoxFrustumIntersect(grid[i], frustumPlanes);
                
                Color boxColor = RED;
                if (state == INSIDE) { boxColor = GREEN; visibleCount++; }
                else if (state == INTERSECT) { boxColor = YELLOW; visibleCount++; }

                // Calculate center and dimensions for drawing
                Vector3 center = {
                    (grid[i].max.x + grid[i].min.x) / 2.0f,
                    (grid[i].max.y + grid[i].min.y) / 2.0f,
                    (grid[i].max.z + grid[i].min.z) / 2.0f
                };
                float width = grid[i].max.x - grid[i].min.x;
                float height = grid[i].max.y - grid[i].min.y;
                float length = grid[i].max.z - grid[i].min.z;

                // Make the colors semi-transparent so they look cool
                Color solidColor = { boxColor.r, boxColor.g, boxColor.b, 150 };
                DrawCubeV(center, {width, height, length}, solidColor);
                DrawCubeWiresV(center, {width, height, length}, boxColor);
            }

        EndMode3D();

        // 5. 2D UI Overlay
        DrawRectangle(10, 10, 320, 140, Fade(BLACK, 0.8f));
        DrawText("VISION CULLING SHOWCASE", 20, 20, 20, WHITE);
        DrawText(TextFormat("Objects Passing Render: %d / 9", visibleCount), 20, 50, 20, LIME);
        DrawText("- WASD + Mouse to Move", 20, 80, 20, LIGHTGRAY);
        
        if (debugLock) {
            DrawText("- [SPACE] UNLOCK Frustum", 20, 110, 20, YELLOW);
            DrawText("FRUSTUM FROZEN: Walk around to see culled (RED) objects!", 350, 20, 20, RED);
        } else {
            DrawText("- [SPACE] LOCK Frustum", 20, 110, 20, LIGHTGRAY);
        }

        DrawFPS(screenWidth - 100, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}