#include "terrain.hpp"
#include "bezierMesh.hpp"
#include "bmp.h"

namespace mat300_terrain {

    // Create Patches with the height map 
    void Terrain::Create(int divCount, const char* heightname)
    {
        // Load the height map
        Bitmap inputBMP = Bitmap{ heightname };
        
        // Init the width and depth to the same size as the height map size
        mWidth = inputBMP.width();
        mHeight = inputBMP.height();
        //mDivCount = divCount;

        // Init the control points with the heights of the height map
        int steps = mWidth / 5; // because it's a 10x10 height map that we are testing
        Patch patch;            // testing with only 1 patch
        for (int y = 0; y < 5; y++)
        {
            for (int x = 0; x < 5; x++)
            {
                // Retrieve the height given a X and Y
                float heightVal = static_cast<float>(*(inputBMP.raw_data() + (x * steps + y * mWidth)));
                
                // Transform the value from [0, 255] to [0, 1]
                float normHeightVal = heightVal / 255.f;

                // Multiply that value with width / 4 (temptative)
                float finalHeight = normHeightVal * (mWidth / 4.f);

                // Store the computed value to the y-component of the current control point
                glm::vec3 pos(x * steps, finalHeight, y * steps);
            }
        }
    }

    void Terrain::Update(float dt)
    {
        
    }

    std::vector<Patch> Terrain::GetPatches()
    {
        Patch patch;

        // WTF?!?!?
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