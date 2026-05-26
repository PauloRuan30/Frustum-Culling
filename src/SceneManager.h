#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "raylib.h"
#include <vector>

class SceneManager {
public:
    SceneManager();
    void InitializeGrid(int countX, int countZ, float spacing);
    
    std::vector<BoundingBox>& GetObjects() { return worldObjects; }

private:
    std::vector<BoundingBox> worldObjects;
    float defaultCubeSize;
};

#endif