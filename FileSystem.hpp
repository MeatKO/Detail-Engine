#pragma once

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
	enum FileOpenMode
	{
		OPEN_TEXT,
		OPEN_BINARY,
		OPEN_UNSUPPORTED
	};

	long int UnixTimestamp();

	bool PathExists(std::string path);

	FileOpenMode TypeToMode(std::string fileType);

	class Asset;
	class AssetManager;

	// only stores a stringstream containing the file, will be operated upon by the FileSystem class
	class File 
	{
	public:
		File(long int currentTime) : creationTime(currentTime) {}

		void SetName(std::string Name);
		std::string GetName();
		void SetType(std::string Type);
		std::string GetType();
		void SetMode(FileOpenMode Mode);
		FileOpenMode GetMode();
		void Fill(std::ifstream& file);
		void Append(std::ifstream& file);
		void Erase();
		std::stringstream& Data();
		long int GetCreationTime();
		int GetSize(); // in bytes
		void Dump();
		void PrintInfo();

	private:
		std::string name = "unnamed";
		std::string type = "unnamed";
		std::stringstream contents; // makes a lot of sense to be private i know...
		long int creationTime = 0;
		FileOpenMode mode = OPEN_UNSUPPORTED; // default
	};

	class FileSystem : public Publisher, public Subscriber
	{
	public:
		FileSystem();

		void Update(EntityController* entityController, AssetManager* assetManager);
		bool IsLoaded(std::string filename);
		void Debug();

	private:
		void LoadFile(std::string path);
		void LoadDir(std::string path); // Loads all the files from a directory
		std::vector<std::string> DirGetAllFileNames(std::string path);
		std::string GetPathFileName(std::string path); // returns the file name ONLY
		std::string GetPathFileType(std::string path); // returns the file type ONLY
		std::string GetPathFullFilename(std::string path); // returns the file name and its type
		std::string GetSanitizedPath(std::string path); // Gets SanitizePath() output and assembles it with \ in between the words;
		std::vector<std::string> SanitizePath(std::string path); // splits the path into words
		bool StringContainsOnly(std::string input, char character);

		std::vector<File> files;
		std::mutex fileioMutex;
	};
}