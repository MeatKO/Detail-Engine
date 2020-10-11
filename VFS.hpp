#pragma once

// Virtual File System
// This is meant to replace FileSystem.hpp

#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include <filesystem>
#include <iterator>
#include <bitset>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include "ECS.hpp"

namespace fs = std::filesystem;

namespace detailEngine
{
	struct FilePathInfo
	{
		std::string path = "";
		std::string name = "";
		std::string type = "";
	};

	// returns true if the input string contains only containedChar
	bool vfsStringContainsOnly(std::string input, char containedChar);

	// removes excess \ and /, also removes invalid symbols
	std::string vfsRemovePathSlashes(std::string path);

	// will turn things like "   name one " -> "name_one"
	std::string vfsStandardisePathToken(std::string token);

	// Removes characters that aren't allowed in the windows file system
	std::string vfsRemovePathInvalidSymbols(std::string path);

	// Combines the two options above and replaces the \ with /
	std::string vfsSanitizeFilePath(std::string path);

	// returns a path string that seperates all tokens with /
	std::string vfsAssemblePath(std::vector<std::string> tokens);

	void vfsStandardisePathTokens(std::vector<std::string>& tokens);

	// splits by /
	// Expects a sanitized path !
	std::vector<std::string> vfsGetPathTokens(std::string path);

	// returns the place where CHARACTER is found within INPUT
	// returns -1 if the CHARACTER is not found anywhere
	int vfsStringContains(std::string input, char character);

	// Since the file IO doesn't automatically generate directories
	// This function will try to create the path if it doesn't exist
	// will return false if the directory cannot be created
	bool vfsEnsureDirectory(std::string path);

	FilePathInfo vfsGetFilePathInfo(std::string path);

	class Pack
	{
	public: 
		Pack() {}
	};

	class vFile
	{
	public:
		vFile() {}

		vFile(std::string FileName, std::string FileType)
		{
			fileName = FileName;
			fileType = FileType;
		}

		vFile(std::string FileName, std::string FileType, char* Data, int ByteSize)
		{
			fileName = FileName;
			fileType = FileType;
			data = Data;
			byteSize = ByteSize;
		}

		// for manual destruction of the data when the engine terminates
		// i do it this way because i might copy the pointer into other objects and i dont want it to get deleted
		// i might've used smart pointers but eh
		void Terminate()
		{
			if (data && byteSize)
			{
				delete[] data;
			}
		}

		std::string GetName()
		{
			return fileName;
		}

		std::string GetType()
		{
			return fileType;
		}

		char* Data()
		{
			return data;
		}

	private:
		std::string fileName = "";
		std::string fileType = "";
		int byteSize = -1;
		char* data = nullptr;
	};

	class VirtualFileSystem : public Publisher, public Subscriber
	{
	public:
		VirtualFileSystem() {}
	};
}