#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "VFS.hpp"

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
		TEX_UNSUPPORTED,
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
		int width = -1;
		int height = -1;
		int byteCount = -1;
		int bpp = -1;    // bpp - bits per pixel
		int textureID = -1;

		void Release();
	};

	struct TGAHeader
	{
		unsigned char  idLength;
		unsigned char  colorMapType;
		unsigned char  dataTypeCode;
		short int colorMapOrigin;
		short int colorMapLength;
		unsigned char  colorMapDepth;
		short int xOrigin;
		short int yOrigin;
		short int width;
		short int height;
		unsigned char  bitsPerPixel;
		unsigned char  imageDescriptor;
	};

	struct BMPHeader
	{
		unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	};

	bool LoadTexture(Texture& Texture, vFile& vfile, std::string& error);

	void FlipTextureVertically(Texture& Texture);
	void FlipTextureHorizontally(Texture& Texture);
	void FlipTextureReverse(Texture& Texture); // flips both horizontally and vertically

	bool LoadTexture(Texture& Texture, std::string FileName, std::string FileType, unsigned char* FileData, unsigned int FileByteSize, std::string& error);

	// Supports 24 and 32 bit, RAW and RLE, RGB TGA files only
	bool LoadTGA(Texture& Texture, std::string FileName, std::string FileType, unsigned char* FileData, unsigned int FileByteSize, std::string& error);
	bool LoadBMP(Texture& Texture, std::string FileName, std::string FileType, unsigned char* FileData, unsigned int FileByteSize, std::string& error);
}