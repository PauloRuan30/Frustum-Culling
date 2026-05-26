#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include "raylib.h"
#include "SceneManager.h"

// UI Helpers
bool DrawButton(Rectangle bounds, const char* text);

// Debug Visualizations
void DrawFrustumWireframe(Matrix viewProj, Color color);
void DrawSceneMinimap(SceneManager& scene, Camera3D mainCamera, Matrix currentFrustumVP, int screenWidth, int screenHeight, RenderTexture2D mapTexture);

#endif