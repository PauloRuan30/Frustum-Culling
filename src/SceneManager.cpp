#include "SceneManager.h"
#include <cstdlib> // For rand()

// This will work? idk yet, just a simple grid of boxes with some random ones thrown in for good measure.
SceneManager::SceneManager() : defaultCubeSize(4.0f) {}

void SceneManager::InitializeGrid(int countX, int countZ, float spacing) {
    worldObjects.clear();
    
    // Center the grid around the origin
    float offsetX = (countX - 1) * spacing / 2.0f;
    float offsetZ = (countZ - 1) * spacing / 2.0f;

    for (int x = 0; x < countX; x++) {
        for (int z = 0; z < countZ; z++) {
            Vector3 center = { 
                (x * spacing) - offsetX, 
                2.0f, 
                (z * spacing) - offsetZ 
            };
            
            BoundingBox box;
            box.min = { center.x - defaultCubeSize/2, center.y - defaultCubeSize/2, center.z - defaultCubeSize/2 };
            box.max = { center.x + defaultCubeSize/2, center.y + defaultCubeSize/2, center.z + defaultCubeSize/2 };
            
            worldObjects.push_back(box);
        }
    }
}