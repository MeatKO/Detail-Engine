#pragma once

#include <GL/glew.h>
#include <iostream>
#include "SOIL2/SOIL2.h"

namespace detailEngine
{
	int LoadTexture(std::string directory, bool nearest = false);
	GLuint64 LoadBindlessTexture(std::string directory);
	GLuint64 MakeBindless(GLuint textureID);
}