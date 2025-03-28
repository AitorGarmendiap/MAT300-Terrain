#include "terrain.hpp"
#include "bezierMesh.hpp"

namespace mat300_terrain {

    Terrain::~Terrain()
    {
        delete mInput;
    }

    void Terrain::LoadHeightMap(int divCount, const char* heightname)
    {
        // Load the height map
        if (mInput)
            delete mInput;
        mInput = new Texture{ heightname };

        // Init the width and depth to the same size as the height map size
        mWidth = mInput->width();
        mHeight = mInput->height();
        Create(divCount);
    }

    // Create Patches with the height map 
    void Terrain::Create(int divCount)
    {
        mDivCount = prevDivCount = divCount; 
        mPatches.clear();
        mPatches.reserve(mDivCount * mDivCount);

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
                        float heightVal = static_cast<float>(*(mInput->data() + (texX + texY * mWidth)));

                        // Transform the value from [0, 255] to [0, 1]
                        float normHeightVal = heightVal / 255.f;

                        // Multiply that value with width / 4 (temptative)
                        float finalHeight = normHeightVal * (mWidth / 4.f);

                        // Store the computed value to the y-component of the current control point
                        rowCtrlPoints.push_back({ (x * steps) + (patchWidth * j), finalHeight, (y * steps) + (patchHeight * i) });
                    }
                    patch.controlPoints.push_back(rowCtrlPoints);
                }
                patch.ComputeNormal();
                mPatches.push_back(patch);
            }
        }
    }

    void Terrain::Update()
    {
        if (mDivCount != prevDivCount)
        {
            prevDivCount = mDivCount;
            Create(mDivCount);
        }
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