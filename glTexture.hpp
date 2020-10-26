#pragma once

#include <GL/glew.h>
#include <iostream>
//#include "SOIL2/SOIL2.h"
#include "PXL.hpp"

namespace detailEngine
{
	int LoadTexture(std::string directory, bool nearest = false);
	GLuint64 LoadBindlessTexture(std::string directory);
	GLuint64 MakeBindless(GLuint textureID);
	unsigned char* TextureImage(std::string path, int& width, int& height);
	void DeleteTextureImage(unsigned char* image);
	int GenerateTexture(std::string data, int width, int height);
	int InitTexture(Texture& texture, bool releaseTextureMemory = true);
}