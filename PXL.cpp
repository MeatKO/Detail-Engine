#include "PXL.hpp"

namespace detailEngine
{
	bool LoadTexture(Texture& Texture, unsigned char* FileData, std::string FileName, std::string FileType)
	{
		if (FileType == "tga" || FileType == ".tga")
		{
			// load tga
			Texture = LoadTGA(FileData);
		}
		else if (FileType == "png" || FileType == ".png")
		{
			// load png
		}
		else if (FileType == "bmp" || FileType == ".bmp")
		{
			// load bmp
		}
		else
		{
			return false;
		}

		Texture.textureName = FileName;

		return true;
	}

	Texture LoadTGA(unsigned char* FileData)
	{
		unsigned char uTGA[12] = { 0,0, 2,0,0,0,0,0,0,0,0,0 }; // uncompressed tga header
		unsigned char cTGA[12] = { 0,0,10,0,0,0,0,0,0,0,0,0 }; // compressed tga header

		Texture outTex;

		// Better safe than sorry eh
		if (FileData)
		{
			// do magic stuff

			// Check if the file header matches the uncompressed header
			if (memcmp(FileData, uTGA, 12) == 0)
			{
				LoadUncompressedTGA(outTex, FileData);
			}
			else if (memcmp(FileData, cTGA, 12) == 0)
			{
				LoadCompressedTGA(outTex, FileData);
			}
			else
			{
				std::cout << "No match for compressed or uncompressed TGA ! \n";

				return outTex;
			}
		}

		return outTex;
	}

	void LoadUncompressedTGA(Texture& Texture, unsigned char* FileData)
	{
		TGA tga;

		int offset = 12; // since we read 12 bytes previously 

		memcpy(tga.header, FileData + offset, 6);

		Texture.width = tga.header[1] * 256 + tga.header[0];    // Calculate Height
		Texture.height = tga.header[3] * 256 + tga.header[2];   // Calculate The Width
		Texture.bpp = tga.header[4];                            // Calculate Bits Per Pixel
		tga.width = Texture.width;                              // Copy Width Into Local Structure
		tga.height = Texture.height;                            // Copy Height Into Local Structure
		tga.bpp = Texture.bpp;                                  // Copy Bpp Into Local Structure



	}

	void LoadCompressedTGA(Texture& Texture, unsigned char* FileData)
	{
		
	}
}