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

	void Texture::operator=(const Texture& text)
	{
		mWidth = text.width();
		mHeight = text.height();
		//size_t length = strlen(reinterpret_cast<const char*>(text.data())) + 1;
		size_t length = sizeof(text.data());
		mData = new unsigned char[length];
		memcpy(mData, text.data(), length);
	}
}