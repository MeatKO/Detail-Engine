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
#include "glTexture.hpp"

namespace fs = std::filesystem;

namespace detailEngine
{
	long int UnixTimestamp();
	std::string GetPathFileName(std::string path); // returns the file name ONLY
	std::string GetPathFileType(std::string path); // returns the file type ONLY
	std::string GetPathFullFilename(std::string path); // returns the file name and its type
	std::string GetSanitizedPath(std::string path); // Gets SanitizePath() output and assembles it with \ in between the words;
	std::vector<std::string> SanitizePath(std::string path); // splits the path into words
	bool StringContainsOnly(std::string input, char character);
	std::string GetPathNoFile(std::string path); // receives a path to a file and gives a path to the directory
	void SplitFileNameType(std::string fileNamePath, std::string& file, std::string& type); // splits aaa.bbb into aaa and bbb
	bool PathExists(std::string path);
	std::string ReplaceSpaces(std::string input, char replaceWith);

	class Asset;
	class AssetManager;
	class File;

	// The directory will only hold the names / references to the other dirs and files
	class Directory
	{
	public:
		Directory(std::string DirName) : name (DirName) {}

		bool ContainsDir(int dirID)
		{
			for (int i = 0; i < directoryIDs.size(); ++i)
			{
				if (directoryIDs[i] == dirID)
					return true;
			}
			return false;
		}

		bool ContainsFile(int fileID)
		{
			for (int i = 0; i < fileIDs.size(); ++i)
			{
				if (fileIDs[i] == fileID)
					return true;
			}
			return false;
		}

		std::string name;
		std::vector<int> fileIDs;
		std::vector<int> directoryIDs;
		int parentDirID = 0;
	};

	// only stores a stringstream containing the file, will be operated upon by the FileSystem class
	class File 
	{
	public:
		File(long int currentTime) : creationTime(currentTime) {}

		virtual void Load() {}

		void SetName(std::string Name);
		std::string GetName();
		void SetType(std::string Type);
		std::string GetType();
		void Fill(std::ifstream& file);
		void Fill(std::string& string);
		void Append(std::ifstream& file);
		void Erase();
		std::stringstream& Data();
		long int GetCreationTime();
		int GetSize(); // in bytes
		void Dump();
		void PrintInfo();

	private:
		std::string name = ""; // the file name
		std::string type = ""; // shaders can be SHADER, models can be MODEL etc...
		std::string extension = ""; // .sh .vs .fs .mp3 .obj...
		std::stringstream data; // the file contents
		long int creationTime = 0;
	};

	class FileSystem : public Publisher, public Subscriber
	{
	public:
		FileSystem();

		int GetDirIndex(std::string DirName);
		// all paths are assumed to start from root, so adding / and /root/ is not recommended as it will give invalid paths
		int FileTreeValidatePath(std::string path);
		// returns last directory id
		// depth must start from 0 (root index)
		int FileTreeValidatePathRec(int parentID, int depth, std::vector<std::string> directoryNames, std::string validPath);
		int AddDirectory(std::string DirName, std::string path);
		void AddFile(int FileID, std::string path);

		void PrintDirContents(Directory& dir);
		void PrintFileTree();

		void Update(EntityController* entityController, AssetManager* assetManager);
		void ExecuteMessage(Message message);

		bool IsLoaded(std::string filename); // makes no sense
		void Debug();

		File* GetFile(std::string fileName, std::string fileType);
		File* GetFile(std::string fileNameType);

		void LoadOBJModel(std::string path); // must contain the file name and stuff as well
		void LoadTextureFile(std::string path);

	private:
		bool FileExists(std::string fileName, std::string fileType);
		void LoadFile(std::string path);
		void LoadDir(std::string path); // Loads all the files from a directory
		std::vector<std::string> DirGetAllFileNames(std::string path);

		std::vector<File> files;
		std::vector<Directory> directories;
		std::mutex fileioMutex;
	};
}