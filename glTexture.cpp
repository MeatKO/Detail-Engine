#include "glTexture.hpp"

namespace detailEngine
{
	int LoadTexture(std::string directory, bool nearest)
	{
		GLuint textureID;
		int width, height;

		glGenTextures(1, &textureID);

		unsigned char* image = SOIL_load_image(directory.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);

		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		if (nearest)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		glBindTexture(GL_TEXTURE_2D, 0);

		SOIL_free_image_data(image);

		return textureID;
	}

	GLuint64 LoadBindlessTexture(std::string directory)
	{
		GLuint textureID;
		textureID = LoadTexture(directory);

		GLuint64 handle = glGetTextureHandleARB(textureID);
		glMakeTextureHandleResidentARB(handle);

		return handle;
	}
	GLuint64 MakeBindless(GLuint textureID)
	{
		GLuint64 handle = glGetTextureHandleARB(textureID);
		glMakeTextureHandleResidentARB(handle);

		return handle;
	}
}