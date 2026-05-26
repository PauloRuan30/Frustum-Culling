#include "raylib.h"
#include "raymath.h"
#include <vector>

#include "CullingMath.h"
#include "SceneManager.h"
#include "Visualization.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

// --- state variables ---
const int screenWidth = 1600;
const int screenHeight = 900;

SceneManager sceneManager;
Camera3D camera = { 0 };
RenderTexture2D minimapTarget;

bool frustumLocked = false;
Matrix lockedVP = { 0 }; 
Plane activePlanes[6];

void UpdateDrawFrame(void) {
    // 1. Input Processing
    UpdateCamera(&camera, CAMERA_FIRST_PERSON);

    if (IsKeyPressed(KEY_SPACE)) {
        frustumLocked = !frustumLocked;
    }

    // 2. Math & Culling State Updates
    Matrix view = GetCameraMatrix(camera);
    Matrix proj = MatrixPerspective(camera.fovy * DEG2RAD, (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f);
    Matrix currentVP = MatrixMultiply(view, proj);

    if (!frustumLocked) {
        lockedVP = currentVP; // Update locked matrix if active
    }

    ExtractFrustumPlanes(lockedVP, activePlanes);

    // 3. Render 3D Scene
    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode3D(camera);
        DrawGrid(50, 1.0f); // Local floor grid

        // If debugging is locked, draw the massive static frustum wireframe
        if (frustumLocked) {
            DrawFrustumWireframe(lockedVP, RED);
        }

        // Perform culling and draw objects
        int visibleCount = 0;
        auto& objects = sceneManager.GetObjects();
        for (const auto& box : objects) {
            CullState state = BoxFrustumIntersect(box, activePlanes);
            
            Color boxColor = RED;
            if (state == INSIDE) { boxColor = GREEN; visibleCount++; }
            else if (state == INTERSECT) { boxColor = YELLOW; visibleCount++; }

            Vector3 center = Vector3Scale(Vector3Add(box.min, box.max), 0.5f);
            Vector3 dim = Vector3Subtract(box.max, box.min);
            DrawCubeV(center, dim, { boxColor.r, boxColor.g, boxColor.b, 160 });
            DrawCubeWiresV(center, dim, boxColor);
        }
    EndMode3D();

    // 4. Render 2D UI Overlay
    DrawRectangle(10, 10, 380, 170, Fade(BLACK, 0.8f));

    DrawText("VISION CULLING DEBUGGER", 20, 20, 24, WHITE);
    DrawText(TextFormat("Total Objects: %d", sceneManager.GetObjects().size()), 20, 50, 20, LIGHTGRAY);
    DrawText(TextFormat("Objects Passing Render: %d", visibleCount), 20, 80, 20, LIME);
    
    DrawText("- WASD + Mouse: Move", 20, 110, 20, LIGHTGRAY);
    Color lockColor = frustumLocked ? YELLOW : LIGHTGRAY;
    DrawText(frustumLocked ? "- [SPACE] UNLOCK Frustum" : "- [SPACE] LOCK Frustum", 20, 140, 20, lockColor);

    if (frustumLocked) {
        DrawText("DEBUG MODE ACTIVE: FRUSTUM FROZEN", 420, 20, 30, RED);
    }

    // 5. Draw Minimap 
    DrawSceneMinimap(sceneManager, camera, lockedVP, screenWidth, screenHeight, minimapTarget);

    DrawFPS(10, screenHeight - 30);
    EndDrawing();
}

// --- Entry Point ---
int main() {
    InitWindow(screenWidth, screenHeight, "Vision Culling Debug Tool");

    // Initialize Systems
    sceneManager.InitializeGrid(3, 3, 10.0f); // Default 3x3

    // Initialize Main FPS Camera
    camera.position = { 0.0f, 2.0f, 25.0f };
    camera.target = { 0.0f, 2.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    minimapTarget = LoadRenderTexture(screenWidth / 4, screenHeight / 4);
    DisableCursor(); 

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        UpdateDrawFrame();
    }
#endif

    // Cleanup resources
    UnloadRenderTexture(minimapTarget);
    CloseWindow();
    return 0;
}