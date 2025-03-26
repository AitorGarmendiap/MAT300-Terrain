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
        mDivCount = divCount;

        // Init the control points with the heights of the height map
        float patchWidth = (float)mWidth / (float)mDivCount;
        float patchHeight = (float)mHeight / (float)mDivCount;
        float steps = static_cast<float>(mWidth) / (3.f * mDivCount);
        for (int i = 0; i < mDivCount; i++)
        {
            for (int j = 0; j < mDivCount; j++)
            {
                Patch patch;
                for (int y = 0; y < 4; y++)
                {
                    std::vector<glm::vec3> rowCtrlPoints;
                    for (int x = 0; x < 4; x++)
                    {
                        // Retrieve the height: (actual pos * amount of dist for next ctrl point) + (patch size depending on mDivCount * "which patch")
                        // "which patch" = if there is 2 mDivCounts, there are 4 patches in total where (0, 0) is the first one, (1, 0) the second one etc.
                        int texX = (x * steps) + (patchWidth * j) >= mWidth ? mWidth - 1 : (x * steps) + (patchWidth * j);
                        int texY = (y * steps) + (patchHeight * i) >= mHeight ? mHeight - 1 : (y * steps) + (patchHeight * i);
                        float heightVal = static_cast<float>(*(input.data() + (texX + texY * mWidth)));

                        // Transform the value from [0, 255] to [0, 1]
                        float normHeightVal = heightVal / 255.f;

                        // Multiply that value with width / 4 (temptative)
                        float finalHeight = normHeightVal * (mWidth / 4.f);

                        // Store the computed value to the y-component of the current control point
                        rowCtrlPoints.push_back({ (x * steps) + (patchWidth * j), finalHeight, (y * steps) + (patchHeight * i) });
                    }
                    patch.controlPoints.push_back(rowCtrlPoints);
                }
                mPatches.push_back(patch);
            }
        }
    }

    void Terrain::Update(float dt)
    {
        
    }

    std::vector<Patch> Terrain::GetPatches()
    {
        for (auto& patch : mPatches)
        {
            CalculateBezierMesh(patch);
        }

        return mPatches;
    }
}