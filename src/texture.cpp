#include "texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace mat300_terrain
{
	Texture::Texture(const char* filename)
	{
		int comp;
		mData = stbi_load(filename, &mWidth, &mHeight, &comp, STBI_grey);
	}

	Texture::~Texture()
	{
		delete[] mData;
	}
}