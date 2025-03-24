#include "terrain.hpp"
#include "bezierMesh.hpp"

namespace mat300_terrain {

    // Create Patches with the height map 
    void Terrain::Create(int patchCount, const char* heightname)
    {

    }

    void Terrain::Update(float dt)
    {

    }

    std::vector<Patch> Terrain::GetPatches()
    {
        Patch patch;

        std::vector<std::vector<glm::vec3>> controlPoints = { {{0,0,0}, {0,1,0}, {0,2,0}, {0,3,0}},
                                                      {{1,0,0}, {1,1,1}, {1,2,1}, {1,3,0}},
                                                      {{2,0,0}, {2,1,2}, {2,2,2}, {2,3,0}},
                                                      {{3,0,0}, {3,1,0}, {3,2,0}, {3,3,0}} };

        
        for (int i = 0; i < 4; ++i)
        {
            std::vector<glm::vec3> temp;

            for (int j = 0; j < 4; j++)
            {
                temp.push_back(controlPoints[i][j]);

            }
            patch.controlPoints.push_back(temp);
        }

        CalculateBezierMesh(patch);

        mPatches.push_back(patch);

        return mPatches;
    }

}