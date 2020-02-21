#include "glShader.hpp"

namespace detailEngine
{
	void Shader::Use()
	{
		glUseProgram(this->Program);
	}

	void Shader::Delete()
	{
		glDeleteProgram(this->Program);
	}

	Shader::Shader(const std::string shaderName, const std::string geometryName)
	{
		name = shaderName;

		bool allowGeometry = false;

		if (geometryName != "")
		{
			allowGeometry = true;
		}

		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;

		std::ifstream vertexShaderFile;
		std::ifstream fragmentShaderFile;

		vertexShaderFile.exceptions(std::ifstream::badbit);
		fragmentShaderFile.exceptions(std::ifstream::badbit);

		try
		{
			vertexShaderFile.open("detail/shaders/" + shaderName + "/" + shaderName + ".vs");
			fragmentShaderFile.open("detail/shaders/" + shaderName + "/" + shaderName + ".fs");

			std::stringstream vertexShaderStream, fragmentShaderStream, geometryShaderStream;

			// Read file's buffer contents into streams
			vertexShaderStream << vertexShaderFile.rdbuf();
			fragmentShaderStream << fragmentShaderFile.rdbuf();

			vertexShaderFile.close();
			fragmentShaderFile.close();

			// Convert stream into string
			vertexCode = vertexShaderStream.str();
			fragmentCode = fragmentShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			//Log(u8"Couldn't open Fragment or Vertex shader files!");
			std::cout << u8"Couldn't open Fragment or Vertex shader files!" << std::endl;
		}

		if (allowGeometry)
		{
			std::ifstream geometryShaderFile;
			// Check for string char mismatch
			geometryShaderFile.exceptions(std::ifstream::badbit);
			try
			{
				geometryShaderFile.open("detail/shaders/" + geometryName + "/" + geometryName + ".gs");
				std::stringstream geometryShaderStream;
				geometryShaderStream << geometryShaderFile.rdbuf();
				geometryShaderFile.close();
				geometryCode = geometryShaderStream.str();
			}
			catch (std::ifstream::failure e)
			{
				//Log(u8"Couldn't open Geometry shader file!");
				std::cout << u8"Couldn't open Geometry shader file!" << std::endl;
			}
		}

		const GLchar* vertexShaderCode = vertexCode.c_str();
		const GLchar* fragmentShaderCode = fragmentCode.c_str();
		const GLchar* geometryShaderCode = geometryCode.c_str();

		GLuint vertex, fragment, geometry;

		GLint success;
		GLchar infoLog[512];

		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertexShaderCode, NULL);
		glCompileShader(vertex);

		// Get shader compile status
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			//Log(u8"Vertex Shader compilation failed!");
			std::cout << u8"Vertex Shader compilation failed!" << std::endl;
			std::cout << infoLog << std::endl;
		}

		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fragmentShaderCode, NULL);
		glCompileShader(fragment);

		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			//Log(u8"Fragment Shader compilation failed!");
			std::cout << u8"Fragment Shader compilation failed!" << std::endl;
			std::cout << infoLog << std::endl;
		}

		this->Program = glCreateProgram();
		glAttachShader(this->Program, vertex);
		glAttachShader(this->Program, fragment);

		if (allowGeometry)
		{
			const GLchar* geometryShaderCode = geometryCode.c_str();
			GLuint geometry;

			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &geometryShaderCode, NULL);
			glCompileShader(geometry);

			glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);

			if (!success)
			{
				glGetShaderInfoLog(fragment, 512, NULL, infoLog);
				//Log(u8"Geometry Shader compilation failed!");
				std::cout << u8"Geometry Shader compilation failed!" << std::endl;
				std::cout << infoLog << std::endl;
			}

			glAttachShader(this->Program, geometry);
		}

		glLinkProgram(this->Program);
		glGetProgramiv(this->Program, GL_LINK_STATUS, &success);

		if (!success)
		{
			glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
			//Log(u8"Shader Program linking failed!");
			std::cout << u8"Shader Program linking failed!" << std::endl;
		}

		// Delete shader objects after they are linked in a program
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		glDeleteShader(geometry);
	}
}