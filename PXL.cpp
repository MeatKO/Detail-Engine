#include "PXL.hpp"

namespace detailEngine
{
	bool LoadTexture(Texture& Texture, std::string FileName, std::string FileType, unsigned char* FileData, unsigned int FileByteSize, std::string& error)
	{
		if (FileType == "tga" || FileType == ".tga")
		{
			if (LoadTGA(Texture, FileName, FileType, FileData, FileByteSize, error))
			{
				Texture.textureName = FileName;
				Texture.type = TEX_TGA;

				return true;
			}
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
			Texture.textureName = "UNSUPPORTED";
			Texture.type = TEX_UNSUPPORTED;

			return false;
		}

		return false;
	}

	bool LoadTGA(Texture& Texture, std::string FileName, std::string FileType, unsigned char* FileData, unsigned int FileByteSize, std::string& error)
	{
		unsigned int offset = 0;
		TGAHeader header;

		// The size of TGAHeader is 18 bytes
		if ((offset + 18) > FileByteSize)
		{
			error = "Unexpected end of File '" + FileName + "." + FileType + " at byte : " + std::to_string(offset + 18) + ".";
			return false;
		}

		memcpy((char*)&header, FileData + offset, 18);
		offset += 18;

		if (!(header.dataTypeCode == 2 || header.dataTypeCode == 10))
		{
			error = "Unsupported TGA Format '" + std::to_string(header.dataTypeCode) + "'. This loader only supports 32bit & 24bit RAW or RLE RGB Images.";
			return false;
		}

		Texture.width = header.width;
		Texture.height = header.height;
		Texture.bpp = header.bitsPerPixel;

		if ((Texture.width <= 0) || (Texture.height <= 0) || ((Texture.bpp != 24) && (Texture.bpp != 32)))
		{
			error = "Couldn't create an image with Width : " + std::to_string(Texture.width) + ", Height : " + std::to_string(Texture.height) +
				", and Bits Per Pixel : " + std::to_string(Texture.bpp) + ".";
			return false;
		}

		if(Texture.bpp == 24)
			Texture.format = TEX_RGB;
		else
			Texture.format = TEX_RGBA;

		int bytesPerPixel = Texture.bpp / 8;
		Texture.byteCount = Texture.width * Texture.height * bytesPerPixel;

		if (Texture.byteCount <= 0)
		{
			error = "Texture Size is less or equal to 0.";
			return false;
		}

		Texture.image = new unsigned char[Texture.byteCount];

		if (Texture.image == nullptr)
		{
			error = "Couldn't allocate " + std::to_string(Texture.byteCount) + " bytes for the image.";
			return false;
		}

		// Type 2 is for RAW, non-encoded images
		if (header.dataTypeCode == 2)
		{
			if ((offset + Texture.byteCount) > FileByteSize)
			{
				error = "Unexpected end of File '" + FileName + "." + FileType + " at byte : " + std::to_string(offset + 18) + ".";
				return false;
			}
			
			memcpy(Texture.image, FileData + offset, Texture.byteCount);

			for (unsigned int i = 0; i < (int)Texture.byteCount; i += bytesPerPixel)
			{
				// XOR 3 times the R and B from the RGB pixel to swap them
				Texture.image[i] ^= Texture.image[i + 2] ^= Texture.image[i] ^= Texture.image[i + 2];
			}

			return true;
		}
		else // For type 10, Type 10 is for RLE images
		{
			int pixelCount = Texture.height * Texture.width;
			unsigned int currentPixel = 0;
			unsigned int currentByte = 0;
			unsigned char* tempPixel = new unsigned char[bytesPerPixel];

			do
			{
				unsigned char chunkheader = 0;     // Variable To Store The Value Of The Id Chunk

				if (offset + 1 >= FileByteSize)
				{
					error = "Unexpected end of file at byte " + std::to_string(offset);
					return;
				}

				memcpy(&chunkheader, FileData + offset, 1);
				offset++; 

				if (chunkheader < 128)       // If The Chunk Is A 'RAW' Chunk
				{
					chunkheader++;           // Add 1 To The Value To Get Total Number Of Raw Pixels

					for (short counter = 0; counter < chunkheader; counter++)
					{
						if (offset >= FileByteSize)
						{
							error = "Unexpected end of file at byte " + std::to_string(offset);
							return;
						}

						memcpy(tempPixel, FileData + offset, bytesPerPixel);
						offset += bytesPerPixel;

						Texture.image[currentByte] = tempPixel[2];           // Red Channel
						Texture.image[currentByte + 1] = tempPixel[1];       // Green Channel
						Texture.image[currentByte + 2] = tempPixel[0];       // Blue Channel

						// If It's A 32bpp Image...
						if (bytesPerPixel == 4)                            
							Texture.image[currentByte + 3] = tempPixel[3];   // Alpha Channel


						currentByte += bytesPerPixel;
						currentPixel++;
					}
				}
				else                            // If It's An RLE Header
				{
					chunkheader -= 127;         // Subtract 127 To Get Rid Of The ID Bit

					if (offset >= FileByteSize)
					{
						error = "Unexpected end of file at byte " + std::to_string(offset);
						return;
					}

					memcpy(tempPixel, FileData + offset, bytesPerPixel);
					offset += bytesPerPixel;

					for (short counter = 0; counter < chunkheader; counter++)
					{
						Texture.image[currentByte] = tempPixel[2];               // Red Channel
						Texture.image[currentByte + 1] = tempPixel[1];           // Green Channel
						Texture.image[currentByte + 2] = tempPixel[0];           // Blue Channel

						// If It's A 32bpp Image
						if (bytesPerPixel == 4)                                
							Texture.image[currentByte + 3] = tempPixel[3];       // Alpha Channel

						currentByte += bytesPerPixel;
						currentPixel++;
					}
				}
			} while (currentpixel < pixelCount);

			return true;
		}

		return false;
	}
}