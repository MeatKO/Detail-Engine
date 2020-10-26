#include "glTexture.hpp"

namespace detailEngine
{
	//GLuint64 LoadBindlessTexture(std::string directory)
	//{
	//	GLuint textureID;
	//	textureID = LoadTexture(directory);
	//
	//	GLuint64 handle = glGetTextureHandleARB(textureID);
	//	glMakeTextureHandleResidentARB(handle);
	//
	//	return handle;
	//}
	//GLuint64 MakeBindless(GLuint textureID)
	//{
	//	GLuint64 handle = glGetTextureHandleARB(textureID);
	//	glMakeTextureHandleResidentARB(handle);
	//
	//	return handle;
	//}

	int GenerateTexture(std::string data, int width, int height)
	{
		GLuint textureID;
	
		glGenTextures(1, &textureID);
	
		glBindTexture(GL_TEXTURE_2D, textureID);
	
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.c_str());
		glGenerateMipmap(GL_TEXTURE_2D);
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
		glBindTexture(GL_TEXTURE_2D, 0);
	
		return textureID;
	}
	
	int InitTexture(Texture& texture, bool releaseTextureMemory)
	{
		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		
		// I wont trust this code below too much
		if (texture.format == TEX_RGB)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, texture.width, texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture.image);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.image);
		
		glGenerateMipmap(GL_TEXTURE_2D);
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
		glBindTexture(GL_TEXTURE_2D, 0);
	
		if (releaseTextureMemory)
			texture.Release();
	
		return textureID;
	}
}
