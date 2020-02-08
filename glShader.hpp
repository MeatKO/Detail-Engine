#pragma once

#include <Windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>
#include <map>

namespace detailEngine
{
	class Shader
	{
	public:
		GLuint Program;

		Shader(const std::string shaderName, const std::string geometryPath = "");

		void Use();
		void Delete();

		std::string name;
	};
}