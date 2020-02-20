#pragma once

#include <GL/glew.h>
#include <iostream>
#include "SOIL2/SOIL2.h"

namespace detailEngine
{
	int LoadTexturePath(std::string directory)
	{
		//GLuint textureID;
		//glGenTextures(1, &textureID);
		//
		//GLint width, height, nrComponents;
		//unsigned char* data = SOIL_load_image(directory.c_str(), &width, &height, &nrComponents, 0);
		//if (data)
		//{
		//	GLenum format = 0;
		//	if (nrComponents == 1)
		//		format = GL_RED;
		//	else if (nrComponents == 3)
		//		format = GL_RGB;
		//	else if (nrComponents == 4)
		//		format = GL_RGBA;
		//
		//	glBindTexture(GL_TEXTURE_2D, textureID);
		//	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		//	glGenerateMipmap(GL_TEXTURE_2D);
		//
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//}
		//else
		//{
		//	return -1;
		//}
		//
		//SOIL_free_image_data(data);
		//
		//return textureID;

		// Initialize empty value for our texture ID
		GLuint textureID;

		// Generate our Texture 
		glGenTextures(1, &textureID);

		// Initialize empty values
		int width, height;

		// Create char* to store the image and set width and height from the loaded image
		unsigned char* image = SOIL_load_image(directory.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);

		// Assign texture to ID
		glBindTexture(GL_TEXTURE_2D, textureID);

		// Load texture image in the position of textureID
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Unallocate the texture after we have set all the parameters
		glBindTexture(GL_TEXTURE_2D, 0);

		// Delete the image after its no longer useful
		SOIL_free_image_data(image);

		// Return just the ID of the texture that we have created
		return textureID;
	}
}