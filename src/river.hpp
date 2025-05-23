#pragma once
#include "patches.hpp"
namespace mat300_terrain
{
	struct Patch;

	class River
	{
	public:
		River(){}
		~River(){}
		void Create(int width, int depth);
		void UpdateMesh(const std::vector<Patch>& patches, int divCount);
		void Recalculate(const std::vector<Patch>& patches, int divCount);
		std::vector<glm::vec3> GetMesh() const { return mRiverMesh; }
		std::vector<glm::vec3> GetNormals() const { return mRiverNormals; }
		void Remove();

		bool start = false;
		bool end = false;
		int selectedCtrlPt = -1;
		int mThickness = 10, oldThickness = mThickness;
		float mDt = 0.01f, oldDt = mDt;
		std::vector<glm::vec3> mRiverCtrlPts;
	private:

		void ProjectLine(const std::vector<Patch>& patches, int divCount, std::vector<glm::vec3>& line);
		std::vector<glm::vec3> mRiverLine, mRiverMesh, mRiverNormals;
		int mWidth = 0, mDepth = 0, mDiv = 0;
	};
}