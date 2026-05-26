#include "Visualization.h"
#include "raymath.h"
#include "CullingMath.h"

// Structure to hold our 2D UI Button State
struct ButtonState {
    Rectangle bounds;
    bool hovered;
    bool clicked;
};

// Simple Immediate-Mode GUI Button
bool DrawButton(Rectangle bounds, const char* text) {
    Vector2 mousePoint = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mousePoint, bounds);
    bool clicked = false;

    if (hovered) {
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            clicked = true;
        }
    }

    // Assign colors based on state (hover/normal)
    Color baseColor = hovered ? DARKGRAY : GRAY;
    DrawRectangleRec(bounds, baseColor);
    DrawRectangleLinesEx(bounds, 2.0f, hovered ? LIGHTGRAY : BLACK);
    
    // Center the text
    int fontSize = 20;
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, bounds.x + (bounds.width/2 - textWidth/2), bounds.y + (bounds.height/2 - fontSize/2), fontSize, WHITE);

    return clicked;
}

// Draw the 3D wireframe of a specific View-Projection Matrix
void DrawFrustumWireframe(Matrix viewProj, Color color) {
    // Invert the VP Matrix to map clip-space back to world-space
    Matrix invVP = MatrixInvert(viewProj);
    Vector3 corners[8];
    
    // Define the 8 corners of the Clip Space Cube (-1 to 1)
    Vector3 clipCorners[8] = {
        {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1}, // Near Plane
        {-1, -1,  1}, {1, -1,  1}, {1, 1,  1}, {-1, 1,  1}  // Far Plane
    };

    // Transform all 8 corners back to world space
    for (int i = 0; i < 8; i++) {
        // Multiplies the vector by the inverse VP matrix, handling W-division internally
        corners[i] = Vector3Transform(clipCorners[i], invVP);
    }

    // Draw the Near Plane
    DrawLine3D(corners[0], corners[1], color);
    DrawLine3D(corners[1], corners[2], color);
    DrawLine3D(corners[2], corners[3], color);
    DrawLine3D(corners[3], corners[0], color);

    // Draw the Far Plane
    DrawLine3D(corners[4], corners[5], color);
    DrawLine3D(corners[5], corners[6], color);
    DrawLine3D(corners[6], corners[7], color);
    DrawLine3D(corners[7], corners[4], color);

    // Connect Near and Far Planes (Edges)
    DrawLine3D(corners[0], corners[4], color);
    DrawLine3D(corners[1], corners[5], color);
    DrawLine3D(corners[2], corners[6], color);
    DrawLine3D(corners[3], corners[7], color);
}

// Sets up and draws the Picture-in-Picture top-down minimap
void DrawSceneMinimap(SceneManager& scene, Camera3D mainCamera, Matrix currentFrustumVP, int screenWidth, int screenHeight, RenderTexture2D mapTexture) {
    // Define Minimap Area (e.g., top-right corner)
    float mapWidth = screenWidth / 4.0f;
    float mapHeight = screenHeight / 4.0f;
    Rectangle mapDestRect = { screenWidth - mapWidth - 10, 10, mapWidth, mapHeight };

    // Set up a second Top-Down Orthographic Camera
    Camera3D orthoCamera = { 0 };
    orthoCamera.position = { 0.0f, 50.0f, 0.0f }; // Height above scene
    orthoCamera.target = { 0.0f, 0.0f, 0.0f };
    orthoCamera.up = { 0.0f, 0.0f, -1.0f }; // Camera looks "north" along Z
    orthoCamera.fovy = 80.0f;              // Zoom level for ortho view
    orthoCamera.projection = CAMERA_ORTHOGRAPHIC;

    // 1. Render Scene to Texture
    BeginTextureMode(mapTexture);
        ClearBackground(GetColor(0x181818FF)); // Dark gray background

        BeginMode3D(orthoCamera);
            DrawGrid(50, 5.0f); // Draw a larger ground grid

            // Draw all world objects with a uniform color in the map
            auto& objects = scene.GetObjects();
            for (const auto& box : objects) {
                Vector3 center = Vector3Scale(Vector3Add(box.min, box.max), 0.5f);
                Vector3 dim = Vector3Subtract(box.max, box.min);
                DrawCubeV(center, dim, GRAY);
                DrawCubeWiresV(center, dim, DARKGRAY);
            }

            // Visualize the MAIN camera position on the map
            DrawCube(mainCamera.position, 2.0f, 2.0f, 2.0f, BLUE);
            DrawCubeWires(mainCamera.position, 2.0f, 2.0f, 2.0f, SKYBLUE);

            // Visualize the active or locked Frustum boundary on the map
            DrawFrustumWireframe(currentFrustumVP, Fade(LIME, 0.5f));

        EndMode3D();
    EndTextureMode();

    // 2. Draw the Rendered Texture to Screen
    // Raylib textures are upside down in OpenGL, so we flip the Y source coordinate
    DrawTexturePro(mapTexture.texture, 
        { 0, 0, (float)mapTexture.texture.width, (float)-mapTexture.texture.height },
        mapDestRect, { 0, 0 }, 0.0f, WHITE);
    
    // Draw Border and Label
    DrawRectangleLinesEx(mapDestRect, 2.0f, BLACK);
    DrawText("MINIMAP (TOP-DOWN)", mapDestRect.x + 10, mapDestRect.y + mapDestRect.height - 25, 20, BLACK);
}