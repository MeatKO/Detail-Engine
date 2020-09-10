#pragma once

// Virtual File System
// This is meant to replace FileSystem.hpp
// Not actually a virtual file system

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

	// splits by /
	// Expects a sanitized path !
	std::vector<std::string> vfsGetPathTokens(std::string path);

	int vfsStringContains(std::string input, char character);

	FilePathInfo GetFilePathInfo(std::string path);

	class VirtualFileSystem : public Publisher, public Subscriber
	{
	public:
		VirtualFileSystem() {}
	};
}