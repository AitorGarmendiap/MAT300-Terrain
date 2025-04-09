#include "river.hpp"
#include "bezierMesh.hpp"
#include "glm/gtc/epsilon.hpp"

namespace mat300_terrain
{
	void River::Create(int width, int depth)
	{
        if (mRiverLine.size() > 1)
        {
            mRiverLine.clear();
            mRiverNormals.clear();
            mRiverMesh.clear();
        }

        mWidth = width;
        mDepth = depth;

        std::vector<glm::vec3> tmpCtrlPts;
        for (int i = 0; i < mRiverCtrlPts.size(); i++)
            tmpCtrlPts.push_back({ mRiverCtrlPts[i].x, 0, mRiverCtrlPts[i].z });

        mRiverLine = CalculateBezierCurve(tmpCtrlPts, mDt);
        mRiverNormals = CalculateDerivativeBezierCurve(tmpCtrlPts, mDt);
	}

	void River::UpdateMesh(const std::vector<Patch>& patches, int divCount)
	{
        std::vector<glm::vec3> left, right;
        for (int i = 0; i < mRiverLine.size(); i++)
        {
            left.push_back(mRiverLine[i] - glm::normalize(mRiverNormals[i]) * static_cast<float>(mThickness));
            right.push_back(mRiverLine[i] + glm::normalize(mRiverNormals[i]) * static_cast<float>(mThickness));
        }

        ProjectLine(patches, divCount, left);
        ProjectLine(patches, divCount, right);

        for (int i = 0; i < mRiverLine.size() - 1; i++)
        {
            glm::vec3 p00{}, p01{}, p10{}, p11{};
            for (float t = 0; t < 1.f; t += 0.2f)
            {
                p00 = glm::mix(left[i], right[i], t);
                p10 = glm::mix(left[i], right[i], t + 0.2f);
                p01 = glm::mix(left[i + 1], right[i + 1], t);
                p11 = glm::mix(left[i + 1], right[i + 1], t + 0.2f);

                mRiverMesh.push_back(p00);
                mRiverMesh.push_back(p10);
                mRiverMesh.push_back(p11);

                mRiverMesh.push_back(p00);
                mRiverMesh.push_back(p11);
                mRiverMesh.push_back(p01);
            }
        }
    }

    void River::Recalculate(const std::vector<Patch>& patches, int divCount)
    {
        if (mRiverLine.size() > 1)
        {
            mRiverLine.clear();
            mRiverNormals.clear();
            mRiverMesh.clear();
        }

        std::vector<glm::vec3> tmpCtrlPts;
        for (int i = 0; i < mRiverCtrlPts.size(); i++)
            tmpCtrlPts.push_back({ mRiverCtrlPts[i].x, 0, mRiverCtrlPts[i].z });

        mRiverLine = CalculateBezierCurve(tmpCtrlPts, mDt);
        mRiverNormals = CalculateDerivativeBezierCurve(tmpCtrlPts, mDt);

        UpdateMesh(patches, divCount);
    }

    void River::Remove()
    {
        start = false;
        end = false;
        mRiverCtrlPts.clear();
        mRiverMesh.clear();
        mRiverNormals.clear();
    }

    void River::ProjectLine(const std::vector<Patch>& patches, int divCount, std::vector<glm::vec3>& line)
    {
        std::vector<glm::vec3> res;
        glm::vec2 patchSize = { mWidth / divCount, mDepth / divCount };
        for (int i = 0; i < line.size() - 1; i++)
        {
            Patch thisPatch;
            glm::vec3 pt = line[i];
            glm::vec3 min, max;
            int atX = 0, atZ = 0;

            // Select the patch that the point of the rivers' mesh is
            for (auto& patch : patches)
            {
                auto& ctrlPts = patch.controlPoints;
                min = ctrlPts[0][0];
                max = ctrlPts[3][3];
                if ((pt.x > min.x || glm::epsilonEqual(pt.x, min.x, FLT_EPSILON)) && (pt.x < max.x || glm::epsilonEqual(pt.x, max.x, FLT_EPSILON)))
                {
                    if ((pt.z > min.z || glm::epsilonEqual(pt.z, min.z, FLT_EPSILON)) && (pt.z < max.z || glm::epsilonEqual(pt.z, max.z, FLT_EPSILON)))
                    {
                        thisPatch = patch;
                        break;
                    }
                }
                atX++;
                if (atX == divCount)
                {
                    atZ++;
                    atX = 0;
                }
            }

            // Compute the new point using the Bezier patch and the position of the point as DT
            // dt = point position / size of the patch; where the point position is between [0, size of the patch]
            glm::vec3 dU = glm::vec3{ 0.0f };
            glm::vec3 dV = glm::vec3{ 0.0f };
            float localU = (pt.z - (atZ * patchSize.y)) / patchSize.y;
            float localV = (pt.x - (atX * patchSize.x)) / patchSize.x;
            glm::vec3 newPoint = { 0, 0, 0 };
            for (int k = 0; k < 4; k++)
            {
                for (int j = 0; j < 4; j++)
                {
                    float Bu = Bernstein(k, localU);
                    float Bv = Bernstein(j, localV);

                    newPoint += thisPatch.controlPoints[k][j] * Bv * Bu;
                }
            }

            line[i] = newPoint;
        }
    }
}