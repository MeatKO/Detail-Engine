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
	long int UnixTimestamp();

	bool PathExists(std::string path);

	ComponentAssetType StringToCAT(std::string type);

	class Asset;
	class AssetManager;

	// only stores a stringstream containing the file, will be operated upon by the FileSystem class
	class File 
	{
	public:
		File(long int currentTime) : creationTime(currentTime) {}

		void SetName(std::string Name) { name = Name; }

		std::string GetName(){return name;}

		void SetType(std::string Type) { type = Type; }

		std::string GetType(){return type;}

		void Fill(std::ifstream& file) { contents.clear(); contents << file.rdbuf(); }

		void Append(std::ifstream& file) { contents << file.rdbuf(); }

		void Erase() { contents.clear(); }

		std::stringstream& Data() { return contents; }

		void Dump()
		{
			std::string word;
			while (getline(contents, word))
			{
				std::cout << word;
			}
		}

	private:
		std::string name = "unnamed";
		std::string type = "unnamed";
		std::stringstream contents; // makes a lot of sense to be private i know...
		long int creationTime = 0;
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
		std::string GetSanitizedPath(std::string path); // Gets SanitizePath() output and assembles it with / in between the words;
		std::vector<std::string> SanitizePath(std::string path); // splits the path into words

		std::vector<File> files;
		std::mutex fileioMutex;
	};
}