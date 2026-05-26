#include "Visualization.h"
#include "raymath.h"
#include "CullingMath.h"

// Helper function to properly invert perspective (The missing W divide!)
Vector3 TransformWithPerspective(Vector3 v, Matrix mat) {
    float x = v.x*mat.m0 + v.y*mat.m4 + v.z*mat.m8 + mat.m12;
    float y = v.x*mat.m1 + v.y*mat.m5 + v.z*mat.m9 + mat.m13;
    float z = v.x*mat.m2 + v.y*mat.m6 + v.z*mat.m10 + mat.m14;
    float w = v.x*mat.m3 + v.y*mat.m7 + v.z*mat.m11 + mat.m15;
    
    // The crucial perspective divide
    if (w != 0.0f) {
        return { x/w, y/w, z/w };
    }
    return { x, y, z };
}

void DrawFrustumWireframe(Matrix viewProj, Color color) {
    Matrix invVP = MatrixInvert(viewProj);
    Vector3 corners[8];
    
    // The 8 corners of the Clip Space Cube
    Vector3 clipCorners[8] = {
        {-1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, -1.0f}, {-1.0f, 1.0f, -1.0f},
        {-1.0f, -1.0f,  1.0f}, {1.0f, -1.0f,  1.0f}, {1.0f, 1.0f,  1.0f}, {-1.0f, 1.0f,  1.0f} 
    };

    // Transform using our new function
    for (int i = 0; i < 8; i++) {
        corners[i] = TransformWithPerspective(clipCorners[i], invVP);
    }

    // Draw Near Plane
    DrawLine3D(corners[0], corners[1], color);
    DrawLine3D(corners[1], corners[2], color);
    DrawLine3D(corners[2], corners[3], color);
    DrawLine3D(corners[3], corners[0], color);

    // Draw Far Plane
    DrawLine3D(corners[4], corners[5], color);
    DrawLine3D(corners[5], corners[6], color);
    DrawLine3D(corners[6], corners[7], color);
    DrawLine3D(corners[7], corners[4], color);

    // Connect Planes
    DrawLine3D(corners[0], corners[4], color);
    DrawLine3D(corners[1], corners[5], color);
    DrawLine3D(corners[2], corners[6], color);
    DrawLine3D(corners[3], corners[7], color);
}

void DrawSceneMinimap(SceneManager& scene, Camera3D mainCamera, Matrix currentFrustumVP, int screenWidth, int screenHeight, RenderTexture2D mapTexture) {
    // Made it a tiny version at the bottom right
    float mapWidth = 250.0f;
    float mapHeight = 150.0f;
    Rectangle mapDestRect = { screenWidth - mapWidth - 10, screenHeight - mapHeight - 10, mapWidth, mapHeight };

    Camera3D orthoCamera = { 0 };
    orthoCamera.position = { 0.0f, 50.0f, 0.0f }; 
    orthoCamera.target = { 0.0f, 0.0f, 0.0f };
    orthoCamera.up = { 0.0f, 0.0f, -1.0f }; 
    orthoCamera.fovy = 80.0f;              
    orthoCamera.projection = CAMERA_ORTHOGRAPHIC;

    BeginTextureMode(mapTexture);
        ClearBackground(GetColor(0x181818FF)); 
        BeginMode3D(orthoCamera);
            DrawGrid(50, 5.0f); 

            auto& objects = scene.GetObjects();
            for (const auto& box : objects) {
                Vector3 center = Vector3Scale(Vector3Add(box.min, box.max), 0.5f);
                Vector3 dim = Vector3Subtract(box.max, box.min);
                DrawCubeV(center, dim, GRAY);
                DrawCubeWiresV(center, dim, DARKGRAY);
            }

            DrawCube(mainCamera.position, 2.0f, 2.0f, 2.0f, BLUE);
            DrawFrustumWireframe(currentFrustumVP, LIME);
        EndMode3D();
    EndTextureMode();

    DrawTexturePro(mapTexture.texture, 
        { 0, 0, (float)mapTexture.texture.width, (float)-mapTexture.texture.height },
        mapDestRect, { 0, 0 }, 0.0f, WHITE);
    
    DrawRectangleLinesEx(mapDestRect, 2.0f, BLACK);
}