#include "terrain.hpp"

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

        // Init the river. PLEASE DELETE THIS ONCE THE CREATION IN DEMO IS DONE
        mRiver.Create(mWidth, mHeight, mDivCount);

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
                        patch.controlPoints[y][x] = glm::vec3({(x * steps) + (patchWidth * j), finalHeight, (y * steps) + (patchHeight * i)});
                    }
                }
                patch.ComputeNormal();
                mPatches.push_back(patch);
            }
        }
        for (auto& patch : mPatches)
        {
            CalculateBezierMesh(patch);
        }

        // Init the river mesh. PLEASE DELETE THIS ONCE THE CREATION IN DEMO IS DONE
        mRiver.CreateMesh(mPatches);
    }

    std::vector<Patch>& Terrain::GetPatches()
    {
        return mPatches;
    }

    void Terrain::Update(const glm::vec3& camPos, float far)
    {
        //if not camera
        if (!detailedPatch)
        {
            if (mDivCount != prevDivCount)
            {
                prevDivCount = mDivCount;
                Create(mDivCount);
            }
            if (updateDetails)
            {
                updateDetails = detailedPatch;
                for (auto& patch : mPatches)
                {
                    patch.t = 0.1f;
                    patch.mesh.clear();

                    // calculate bezier
                    CalculateBezierMesh(patch);
                }
            }
        }
        //if camera
        else
        {
            if(!updateDetails)
                updateDetails = detailedPatch;

            // go over all patches calculate dt by distance
            for (auto& patch : mPatches)
            {
                // dist = length2 cam - patch.getCenter
                float dist = glm::distance(camPos, patch.GetCenterPoint());

                // calculate dt. lerp(0.075, 0.3, dist/ (far-near));
                patch.t = 1.f / glm::mix(10, 2, dist / far);
                patch.mesh.clear();

                // calculate bezier
                CalculateBezierMesh(patch);
            }
        }
    }

    void Terrain::Recalculate(int patch, int controlPoint, glm::vec3 prevPos)
    {
        if (sharpEdges)
        {
            int pointY = controlPoint / 4,  pointX = controlPoint % 4;
            int patchY = patch / mDivCount, patchX = patch % mDivCount;

            // move adjacent point in next patch
            if (pointY == 0) // top
            {
                if (patchY > 0)
                    mPatches[patch - mDivCount].controlPoints[3][pointX] = mPatches[patch].controlPoints[pointY][pointX];
            }
            else if (pointY == 3) // bottom
            {
                if ((patchY + 1) < mDivCount)
                    mPatches[patch + mDivCount].controlPoints[0][pointX] = mPatches[patch].controlPoints[pointY][pointX];
            }
            else
            {
                if (pointX == 0) // left
                {
                    if (patchX > 0)
                        mPatches[patch - 1].controlPoints[pointY][3] = mPatches[patch].controlPoints[pointY][pointX];
                }
                else if (pointX == 3) // right
                {
                    if ((patchX + 1) < mDivCount)
                        mPatches[patch + 1].controlPoints[pointY][0] = mPatches[patch].controlPoints[pointY][pointX];
                }
            }


        }
        else   //smooth edges
        {
            // +=====+=====+=====+=====+
            // | 3,0 | 3,1 | 3,2 | 3,3 |
            // +=====+=====+=====+=====+
            // | 2,0 | 2,1 | 2,2 | 2,3 |
            // +-----+-----+-----+-----+
            // | 1,0 | 1,1 | 1,2 | 1,3 |
            // +-----+-----+-----+-----+
            // | 0,0 | 0,1 | 0,2 | 0,3 |
            // +-----+-----+-----+-----+
            //get the control point coordinate
            int pointY = controlPoint / 4, pointX = controlPoint % 4;
            int patchY = patch / mDivCount, patchX = patch % mDivCount;


            float deltaY = (mPatches[patch].controlPoints[pointY][pointX].y - prevPos.y);

            //Center Control Points
            if (pointY == 2 && pointX == 1)  //
            {
              //  //get the edges near the control point
              //  glm::vec3 northEdge = mPatches[patch].controlPoints[3][1];
              //  glm::vec3 westEdge = mPatches[patch].controlPoints[2][0];
              //  
              //  //get the height diff between them
              //  float northDiff = (northEdge.y - mPatches[patch].controlPoints[pointY][pointX].y);
              //  float westDiff = (westEdge.y - mPatches[patch].controlPoints[pointY][pointX].y);
              //  
              //  //move the control points from the following patches so they collinear
              //  MoveControlPointY(patch - 1, 2, 2, westDiff,westEdge.y, false);
              //  MoveControlPointY(patch + mDivCount, 1, 1,  northDiff,northEdge.y, false);
              //
              //  //get the difference between the edges and diagonal control point
              //  glm::vec3 westPoint = mPatches[patch - 1].controlPoints[2][2];
              //  glm::vec3 northPoint = mPatches[patch + mDivCount].controlPoints[1][1];
              //
              //  glm::vec3 WestPathcNorthEdge = mPatches[patch - 1].controlPoints[3][2];
              //  glm::vec3 NorthPathcWestEdge = mPatches[patch + mDivCount].controlPoints[1][0];
              //
              //  float diffY1 = westPoint.y - WestPathcNorthEdge.y;
              //  float diffY2 = northPoint.y - NorthPathcWestEdge.y;
              //
              //  // height for the diagonal control point
              //  float finalYDiff = (diffY1 + WestPathcNorthEdge.y + diffY2 + NorthPathcWestEdge.y) / 2;
              //
              //  MoveControlPointY(patch + mDivCount - 1, 1, 2, -finalYDiff, 0, false);

                //get the north and east center control points
                glm::vec3 northCP = mPatches[patch + mDivCount].controlPoints[1][1];
                glm::vec3 eastCP = mPatches[patch - 1].controlPoints[2][2];
                
                
                glm::vec3 currCP = mPatches[patch].controlPoints[pointY][pointX];
                
                float eastDiff = (currCP.y + eastCP.y) / 2;
                float northDiff = (currCP.y + northCP.y) / 2;
                
                
                //apply height to the edges
                
                MoveControlPointY(patch, 3, 1, northDiff, 0, false);
                MoveControlPointY(patch + mDivCount, 0, 1, northDiff, 0, false);
                
                MoveControlPointY(patch, 2, 0, eastDiff, 0, false);
                MoveControlPointY(patch - 1, 2, 3, eastDiff, 0, false);

            }
            if (pointY == 2 && pointX == 2)  //
            {
               // //get the edges near the control point
               // glm::vec3 northEdge = mPatches[patch].controlPoints[3][2];
               // glm::vec3 eastEdge = mPatches[patch].controlPoints[2][3];
               // 
               // //get the height diff between them
               // float northDiff = (northEdge.y - mPatches[patch].controlPoints[pointY][pointX].y);
               // float eastDiff = (eastEdge.y - mPatches[patch].controlPoints[pointY][pointX].y);
               // 
               // //move the control points from the following patches so they collinear
               // MoveControlPointY(patch + 1, 2, 1, eastDiff,eastEdge.y, false);
               // MoveControlPointY(patch + mDivCount, 1, 2, northDiff, northEdge.y, false);
               //
               // //get the difference between the edges and diagonal control point
               // glm::vec3 eastPoint = mPatches[patch + 1].controlPoints[2][1];
               // glm::vec3 northPoint = mPatches[patch + mDivCount].controlPoints[1][2]; 
               //
               // glm::vec3 EastPathcNorthEdge = mPatches[patch + 1].controlPoints[3][1];
               // glm::vec3 NorthPathcEastEdge = mPatches[patch + mDivCount].controlPoints[1][3];
               //
               // float diffY1 = eastPoint.y - EastPathcNorthEdge.y;
               // float diffY2 = northPoint.y - NorthPathcEastEdge.y;
               //
               // // height for the diagonal control point
               // float finalYDiff = (diffY1 + EastPathcNorthEdge.y + diffY2 + NorthPathcEastEdge.y) / 2;
               // 
               // MoveControlPointY(patch + mDivCount + 1, 1, 1, -finalYDiff, 0, false);

                //get the north and east center control points
                glm::vec3 northCP = mPatches[patch + mDivCount].controlPoints[1][2];
                glm::vec3 eastCP = mPatches[patch + 1].controlPoints[2][1];
              
              
                glm::vec3 currCP = mPatches[patch].controlPoints[pointY][pointX];
              
                float eastDiff = (currCP.y + eastCP.y) / 2;
                float northDiff = (currCP.y + northCP.y) / 2;
              
              
                //apply height to the edges
              
                MoveControlPointY(patch, 3, 2, northDiff, 0, false);
                MoveControlPointY(patch + mDivCount, 0, 2, northDiff, 0, false);
              
                MoveControlPointY(patch, 2, 3, eastDiff, 0, false);
                MoveControlPointY(patch + 1, 2, 0, eastDiff, 0, false);
            }
            if (pointY == 1 && pointX == 1)  //
            {
                ////get the edges near the control point
                //glm::vec3 westEdge = mPatches[patch].controlPoints[1][0];
                //glm::vec3 southEdge = mPatches[patch].controlPoints[0][1];
                //
                ////get the height diff between them
                //float westDiff = (westEdge.y - mPatches[patch].controlPoints[pointY][pointX].y);
                //float southDiff = (southEdge.y - mPatches[patch].controlPoints[pointY][pointX].y);
                //
                ////move the control points from the following patches so they collinear
                //MoveControlPointY(patch - mDivCount, 2, 1, southDiff,southEdge.y, false);
                //MoveControlPointY(patch - 1 , 1, 2, westDiff, westEdge.y, false);
                //
                ////get the difference between the edges and diagonal control point
                //glm::vec3 eastPoint = mPatches[patch - 1].controlPoints[1][2];
                //glm::vec3 southPoint = mPatches[patch - mDivCount].controlPoints[2][1];
                //
                //glm::vec3 EastPathcSouthEdge = mPatches[patch - 1].controlPoints[0][2];
                //glm::vec3 SouthPathcEastEdge = mPatches[patch - mDivCount].controlPoints[2][0];
                //
                //float diffY1 = eastPoint.y - EastPathcSouthEdge.y;
                //float diffY2 = southPoint.y - SouthPathcEastEdge.y;
                //
                //// height for the diagonal control point
                //float finalYDiff = (diffY1 + EastPathcSouthEdge.y + diffY2 + SouthPathcEastEdge.y) / 2;
                //
                //
                //MoveControlPointY(patch - mDivCount - 1, 2, 2, -finalYDiff, 0, false);

                //get the north and east center control points
                glm::vec3 northCP = mPatches[patch - mDivCount].controlPoints[2][1];
                glm::vec3 eastCP = mPatches[patch - 1].controlPoints[1][2];


                glm::vec3 currCP = mPatches[patch].controlPoints[pointY][pointX];

                float eastDiff = (currCP.y + eastCP.y) / 2;
                float northDiff = (currCP.y + northCP.y) / 2;


                //apply height to the edges

                MoveControlPointY(patch, 0, 1, northDiff, 0, false);
                MoveControlPointY(patch - mDivCount, 3, 1, northDiff, 0, false);

                MoveControlPointY(patch, 1, 0, eastDiff, 0, false);
                MoveControlPointY(patch - 1, 1, 3, eastDiff, 0, false);
            }
            if (pointY == 1 && pointX == 2)  //
            {
                ////get the edges near the control point
                //glm::vec3 eastEdge = mPatches[patch].controlPoints[1][3];
                //glm::vec3 southEdge = mPatches[patch].controlPoints[0][2];
                //
                ////get the height diff between them
                //float eastDiff = (eastEdge.y - mPatches[patch].controlPoints[pointY][pointX].y);
                //float southDiff = (southEdge.y - mPatches[patch].controlPoints[pointY][pointX].y);
                //
                ////move the control points from the following patches so they collinear
                //MoveControlPointY(patch - mDivCount, 2, 2, southDiff, southEdge.y, false);
                //MoveControlPointY(patch + 1, 1, 1, eastDiff, eastEdge.y, false);
                //
                ////get the difference between the edges and diagonal control point
                //glm::vec3 eastPoint = mPatches[patch + 1].controlPoints[1][1];
                //glm::vec3 southPoint = mPatches[patch - mDivCount].controlPoints[2][2];
                //
                //glm::vec3 EastPathcSouthEdge = mPatches[patch + 1].controlPoints[0][1];
                //glm::vec3 SouthPathcEastEdge = mPatches[patch - mDivCount].controlPoints[2][3];
                //
                //float diffY1 = eastPoint.y - EastPathcSouthEdge.y;
                //float diffY2 = southPoint.y - SouthPathcEastEdge.y;
                //
                //// height for the diagonal control point
                //float finalYDiff = (diffY1 + EastPathcSouthEdge.y + diffY2 + SouthPathcEastEdge.y) / 2;
                //MoveControlPointY(patch - mDivCount + 1, 2, 2, -finalYDiff, 0, false);

                 //get the north and east center control points
                glm::vec3 northCP = mPatches[patch - mDivCount].controlPoints[2][2];
                glm::vec3 eastCP = mPatches[patch + 1].controlPoints[1][1];


                glm::vec3 currCP = mPatches[patch].controlPoints[pointY][pointX];

                float eastDiff = (currCP.y + eastCP.y) / 2;
                float northDiff = (currCP.y + northCP.y) / 2;


                //apply height to the edges

                MoveControlPointY(patch, 0, 2, northDiff, 0, false);
                MoveControlPointY(patch - mDivCount, 3, 2, northDiff, 0, false);

                MoveControlPointY(patch, 1, 3, eastDiff, 0, false);
                MoveControlPointY(patch + 1, 1, 0, eastDiff, 0, false);
            }

            //edge Control points
            if (pointY == 3 && pointX == 1)  //
            {
                glm::vec3 movingPoint = mPatches[patch].controlPoints[pointY][pointX];
                // move the same control point from the other path
                MoveControlPointY(patch + mDivCount, 0, 1, movingPoint.y, 0, false);
                
                //move the mid control points near by the same delta
                MoveControlPointY(patch, 2, 1, deltaY, mPatches[patch].controlPoints[2][1].y, false);
                MoveControlPointY(patch + mDivCount, 1, 1, deltaY, mPatches[patch + mDivCount].controlPoints[1][1].y, false);
                
                glm::vec3 pivotPoint = mPatches[patch].controlPoints[3][0];
                
                float diffY = movingPoint.y - pivotPoint.y;
                
                MoveControlPointY(patch - 1, 3, 2, -diffY, pivotPoint.y, false);
                MoveControlPointY(patch - 1 + mDivCount, 0, 2, -diffY, pivotPoint.y, false);

            }
            if (pointY == 3 && pointX == 2)  //
            {
                glm::vec3 movingPoint = mPatches[patch].controlPoints[pointY][pointX];
                // move the same control point from the other path
                MoveControlPointY(patch + mDivCount, 0, 2, movingPoint.y, 0, false);
               
                //move the mid control points near by the same delta
                MoveControlPointY(patch, 2, 2, deltaY, mPatches[patch].controlPoints[2][2].y, false);
                MoveControlPointY(patch + mDivCount, 1, 2, deltaY, mPatches[patch + mDivCount].controlPoints[1][2].y, false);
               
               
                glm::vec3 pivotPoint = mPatches[patch].controlPoints[3][3];
               
                float diffY = movingPoint.y - pivotPoint.y;
               
                MoveControlPointY(patch + 1, 3, 1, -diffY, pivotPoint.y, false);
                MoveControlPointY(patch + 1 + mDivCount, 0, 1, -diffY, pivotPoint.y, false);

            }

            if (pointY == 2 && pointX == 3)  //
            {
                glm::vec3 movingPoint = mPatches[patch].controlPoints[pointY][pointX];
                // move the same control point from the other path
                MoveControlPointY(patch + 1, 2, 0, movingPoint.y, 0, false);
                
                //move the mid control points near by the same delta
                MoveControlPointY(patch, 2, 2, deltaY, mPatches[patch].controlPoints[2][2].y, false);
                MoveControlPointY(patch + 1, 2, 1, deltaY, mPatches[patch + 1].controlPoints[2][1].y, false);
                
                glm::vec3 pivotPoint = mPatches[patch].controlPoints[3][3];
                
                float diffY = movingPoint.y - pivotPoint.y;
                
                MoveControlPointY(patch + mDivCount, 1, 3, -diffY, pivotPoint.y, false);
                MoveControlPointY(patch + 1 + mDivCount, 1, 0, -diffY, pivotPoint.y, false);

            }
            if (pointY == 1 && pointX == 3)  //
            {
                glm::vec3 movingPoint = mPatches[patch].controlPoints[pointY][pointX];
                // move the same control point from the other path
                MoveControlPointY(patch + 1, 1, 0, movingPoint.y, 0, false);
               
                //move the mid control points near by the same delta
                MoveControlPointY(patch, 1, 2, deltaY, mPatches[patch].controlPoints[1][2].y, false);
                MoveControlPointY(patch + 1, 1, 1, deltaY, mPatches[patch + 1].controlPoints[1][1].y, false);
               
                glm::vec3 pivotPoint = mPatches[patch].controlPoints[0][3];
               
                float diffY = movingPoint.y - pivotPoint.y;
               
                MoveControlPointY(patch - mDivCount, 2, 3, -diffY, pivotPoint.y, false);
                MoveControlPointY(patch + 1 - mDivCount, 2, 0, -diffY, pivotPoint.y, false);

            }

            if (pointY == 0 && pointX == 1)  //
            {
                glm::vec3 movingPoint = mPatches[patch].controlPoints[pointY][pointX];
                // move the same control point from the other path
                MoveControlPointY(patch - mDivCount, 3, 1, movingPoint.y, 0, false);
               
                //move the mid control points near by the same delta
                MoveControlPointY(patch, 1, 1, deltaY, mPatches[patch].controlPoints[1][1].y, false);
                MoveControlPointY(patch - mDivCount, 2, 1, deltaY, mPatches[patch - mDivCount].controlPoints[2][1].y, false);
               
                glm::vec3 pivotPoint = mPatches[patch].controlPoints[0][0];
               
                float diffY = movingPoint.y - pivotPoint.y;
               
                MoveControlPointY(patch - 1, 0, 2, -diffY, pivotPoint.y, false);
                MoveControlPointY(patch - 1 - mDivCount, 3, 2, -diffY, pivotPoint.y, false);
            }
            if (pointY == 0 && pointX == 2)  //
            {
                glm::vec3 movingPoint = mPatches[patch].controlPoints[pointY][pointX];
                // move the same control point from the other path
                MoveControlPointY(patch - mDivCount, 3, 2, movingPoint.y, 0, false);
               
                //move the mid control points near by the same delta
                MoveControlPointY(patch, 1, 2, deltaY, mPatches[patch].controlPoints[1][2].y, false);
                MoveControlPointY(patch - mDivCount, 2, 2, deltaY, mPatches[patch - mDivCount].controlPoints[2][2].y, false);
               
                glm::vec3 pivotPoint = mPatches[patch].controlPoints[0][3];
               
                float diffY = movingPoint.y - pivotPoint.y;
               
                MoveControlPointY(patch + 1, 0, 1, -diffY, pivotPoint.y, false);
                MoveControlPointY(patch + 1 - mDivCount, 3, 1, -diffY, pivotPoint.y, false);

            }

            if (pointY == 2 && pointX == 0)  //
            {
                glm::vec3 movingPoint = mPatches[patch].controlPoints[pointY][pointX];
                // move the same control point from the other path
                MoveControlPointY(patch - 1, 2, 3, movingPoint.y, 0, false);
               
                //move the mid control points near by the same delta
                MoveControlPointY(patch, 2, 1, deltaY, mPatches[patch].controlPoints[2][1].y, false);
                MoveControlPointY(patch - 1, 2, 2, deltaY, mPatches[patch - 1].controlPoints[2][2].y, false);
               
                glm::vec3 pivotPoint = mPatches[patch].controlPoints[3][0];
               
                float diffY = movingPoint.y - pivotPoint.y;
               
                MoveControlPointY(patch + mDivCount, 1, 0, -diffY, pivotPoint.y, false);
                MoveControlPointY(patch - 1 + mDivCount, 1, 3, -diffY, pivotPoint.y, false);
               

            }
            if (pointY == 1 && pointX == 0)  //
            {
                glm::vec3 movingPoint = mPatches[patch].controlPoints[pointY][pointX];
                // move the same control point from the other path
                MoveControlPointY(patch - 1, 1, 3, movingPoint.y, 0, false);

                //move the mid control points near by the same delta
                MoveControlPointY(patch, 1, 1, deltaY, mPatches[patch].controlPoints[1][1].y, false);
                MoveControlPointY(patch - 1, 1, 2, deltaY, mPatches[patch - 1].controlPoints[1][2].y, false);

                glm::vec3 pivotPoint = mPatches[patch].controlPoints[0][0];

                float diffY = movingPoint.y - pivotPoint.y;

                MoveControlPointY(patch - mDivCount, 2, 0, -diffY, pivotPoint.y, false);
                MoveControlPointY(patch - 1 - mDivCount, 2, 3, -diffY, pivotPoint.y, false);
            }
        }
        for (auto& patch : mPatches)
        {
            CalculateBezierMesh(patch);
        }
    }
    void Terrain::MoveControlPointY(int patch, int controlPointY, int controlPointX, float deltaY, float reffY, bool positiveDiff)
    {
        if (patch >= 0)
        {
            mPatches[patch].controlPoints[controlPointY][controlPointX].y = reffY + deltaY;

        }
       // else
        {
           // mPatches[patch].controlPoints[controlPointY][controlPointX].y = -deltaY;
        }
    }
}