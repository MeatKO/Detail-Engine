#pragma once

#include <iostream>
#include <string>
#include <vector>

// PiXeL header
// includes : 
// TGA, PNG and BMP loaders

// The TGA code is basically copied from NeHe
// But since i didn't use FILE* but instead just byte arrays since I use my own strange file system, i will read the files differently

namespace detailEngine
{
	enum TextureType
	{
		TEX_TGA,
		TEX_PNG,
		TEX_BMP,
		TEX_LAST
	};

	enum TextureFormat
	{
		TEX_RGB,
		TEX_RGBA
	};

	struct Texture
	{
		std::string textureName = "";
		TextureType type = TEX_TGA;
		TextureFormat format = TEX_RGB;
		unsigned char* image = nullptr;
		int width = -1, height = -1, bpp = -1, textureID = -1; // bpp - bits per pixel, textureID is used in the graphics API
	};

	struct TGA
	{
		unsigned char header[6];         // Holds The First 6 Useful Bytes Of The File
		unsigned int bytesPerPixel = 0;  // Number Of BYTES Per Pixel (3 Or 4)
		unsigned int imageSize = 0;      // Amount Of Memory Needed To Hold The Image
		unsigned int type = 0;           // The Type Of Image, GL_RGB Or GL_RGBA
		unsigned int height = 0;         // Height Of Image                 
		unsigned int width = 0;          // Width Of Image              
		unsigned int bpp = 0;            // Number Of BITS Per Pixel (24 Or 32)
	};

	bool LoadTexture(Texture& Texture, unsigned char* FileData, std::string FileName, std::string FileType);

	Texture LoadTGA(unsigned char* FileData);
	//Texture LoadPNG(unsigned char* fileData);
	//Texture LoadBMP(unsigned char* fileData);

	void LoadUncompressedTGA(Texture& Texture, unsigned char* FileData);
	void LoadCompressedTGA(Texture& Texture, unsigned char* FileData);

}