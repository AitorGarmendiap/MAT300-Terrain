#include "terrain.hpp"
#include "bezierMesh.hpp"
#include "texture.hpp"

namespace mat300_terrain {

    // Create Patches with the height map 
    void Terrain::Create(int divCount, const char* heightname)
    {
        // Load the height map
        Texture input = Texture{ heightname };
        
        // Init the width and depth to the same size as the height map size
        mWidth = input.width();
        mHeight = input.height();
        //mDivCount = divCount;

        // Init the control points with the heights of the height map
        float steps = static_cast<float>(mWidth) / 3.f;
        Patch patch;
        for (int y = 0; y < 4; y++)
        {
            std::vector<glm::vec3> rowCtrlPoints;
            for (int x = 0; x < 4; x++)
            {
                // Retrieve the height given a X and Y
                int texX = x * steps >= mWidth ? mWidth - 1 : x * steps;
                int texY = y * steps >= mHeight ? mHeight - 1 : y * steps;
                float heightVal = static_cast<float>(*(input.data() + (texX + texY * mWidth)));
                
                // Transform the value from [0, 255] to [0, 1]
                float normHeightVal = heightVal / 255.f;

                // Multiply that value with width / 4 (temptative)
                float finalHeight = normHeightVal * (mWidth / 4.f);

                // Store the computed value to the y-component of the current control point
                rowCtrlPoints.push_back({ x * steps, finalHeight, y * steps });
            }
            patch.controlPoints.push_back(rowCtrlPoints);
        }
        mPatches.push_back(patch);
    }

    void Terrain::Update(float dt)
    {
        
    }

    std::vector<Patch> Terrain::GetPatches()
    {
        CalculateBezierMesh(mPatches.back());
        return mPatches;
    }
}