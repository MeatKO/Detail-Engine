#pragma once

#include <Windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>
#include <map>
#include "PCS.hpp"

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

	// The name will be stored in the asset, this will only hold a program ID
	class glShader
	{
	public:
		void Use();
		void Delete();

		bool isEnabled = false;
		unsigned int program;
	};

	// Will inherit the file class and will override the load function
	class glShaderFile
	{
	public:
		glShaderFile();
		std::string vertexSource;
		std::string fragmentSource;
		std::string geometrySource;
	};
	 
	void ProcessShader(glShader& shader, glShaderFile& shaderFile, Publisher* assetMgr, std::string ShaderAssetName); // the AssetManager* is used for pSendMessage Errors
}