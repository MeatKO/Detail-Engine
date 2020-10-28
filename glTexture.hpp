#pragma once

#include <GL/glew.h>
#include <iostream>
//#include "SOIL2/SOIL2.h"
#include "PXL.hpp"

namespace detailEngine
{
	int GenerateTexture(std::string data, int width, int height);
	int InitTexture(Texture& texture, bool releaseTextureMemory = true);
}