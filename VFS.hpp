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
	class Pack;
	class vFile;
	class vDir;

	struct FilePathInfo
	{
		std::string path = "";
		std::string name = "";
		std::string type = "";

		friend std::ostream& operator <<(std::ostream& stream, const FilePathInfo& info);
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

	bool vfsPathExists(std::string path);
	bool vfsPhysicalFileExists(std::string filePath);
	time_t vfsLastModified(std::string path);

	bool vfsLoadFile(vFile& newFile, std::string path);

	// turns the "/" into "root/"
	// turns "~" into "root/detail"
	std::string vfsNormalizeVirtualFilePath(std::string path);

	struct FileRequest
	{
		std::string physicalPath;
		std::string virtualPath;
	};

	class Pack
	{
	public:
		Pack() {}
	};

	class vFile
	{
	public:
		vFile() {}
		vFile(std::string FileName)
		{
			fileName = FileName;
		}
		vFile(std::string FileName, std::string FileType, std::string FilePath, int fileID, unsigned char* Data, int ByteSize, int ParentID)
		{
			fileName = FileName;
			fileType = FileType;
			filePhysicalPath = FilePath;
			data = Data;
			byteSize = ByteSize;
			id = fileID;
			parentID = ParentID;
		}

		void Terminate()
		{
			if ((data != nullptr) && (byteSize >= 0))
			{
				byteSize = -1;
				delete[] data;
			}
		}

		int id = -1;
		int parentID = -1;
		bool deleted = false;
		std::string fileName = "";
		std::string fileType = "";
		std::string filePhysicalPath = "";
		std::string fileVirtualPath = "";
		time_t lastModified = 0;
		int byteSize = -1;
		unsigned char* data = nullptr;

	};

	class vDir
	{
	public:
		vDir() {}
		vDir(std::string Name)
		{
			name = Name;
		}
		vDir(std::string Name, int DirID, int ParentID)
		{
			name = Name;
			id = DirID;
			parentID = ParentID;
		}

		int id = -1;
		int parentID = -1;
		std::string name;
		std::vector<int> subDirIDs;
		std::vector<int> subFileIDs;
	};

	class VirtualFileSystem : public Publisher, public Subscriber
	{
	public:
		VirtualFileSystem();
		~VirtualFileSystem();

		void Update();
		void Terminate();
		void PrintFileTree();
		std::vector<int> vCheckForFileModifications(); // checks if the loaded files were modified on disk

		// Directory functions

		void MakeDir(std::string virtualDirPath);
		// void LoadDir(std::string physicalDirectoryPath, std::string virtualDirectoryPath); // loads the contents of the physical dir into the virtual dir
		// void RemoveDir(std::string virtualDirectoryPath); // removes only the last dir from the path + all of its children
		// void RenameDir(std::string virtualDirectoryPath, std::string newDirName); // renames the last dir from the path if possible
		// void MoveDir(std::string virtualirectoryPath, std::string newVirtualDirectoryPath); // changes the parent of the desired directory
		//
		std::string TraverseDirectory(int dirID); // Gives the directory path for a given directory ID

		// File functions

		// void LoadFile(std::string physicalFilePath, std::string virtualFilePath, std::string newFileName = "", std::string newFileType = "");
		// void RenameFile(std::string virtualFilePath, std::string newFileName, std::string newFileType = "");
		// void RemoveFile(std::string virtualFilePath);
		// void MoveFile(std::string virtualFilePath, std::string newVirtualFilePath); // moves the file if possible
		//
		// vFile GetFile(std::string virtualFilePath);

		// Hidden functions

		//bool DirContainsDir(std::string virtualDirPath, std::string subDirName, int& subDirID);
		//bool DirContainsDir(std::string virtualDirPath, int subDirID);
		bool DirContainsDir(int parentDirID, std::string subDirName);
		bool DirContainsDir(int parentDirID, std::string subDirName, int& subDirID);
		bool DirContainsDir(int parentDirID, int subDirID);
		int AddDirToList(std::string dirName, int dirParent = -1); // returns the directory id
		void AddDirToDir(int parentDirID, int subDirID);
		int GetDirID(std::string virtualDirPath);
		// bool DirExists(std::string virtualDirPath);
		//
		// bool DirContainsFile(std::string virtualDirPath, std::string subFileName, std::string subFileType, int& subFileID);
		// bool DirContainsFile(std::string virtualDirPath, std::string subFileName, std::string subFileType);
		int AddFileToList(std::string physicalPath, int fileParent = -1); // returns the file id
		// bool FileExists(std::string virtualFilePath);
		//
		// int GetDirID(std::string virtualDirPath);
		// int GetFileID(std::string virtualFilePath);
		//
		// void LoadFileAsync(std::string physicalFilePath, std::string virtualFilePath);
		// void LoadFileAwait(std::string physicalFilePath, std::string virtualFilePath);

	private:
		bool LoadFile(vFile& newFile, std::string path, std::string name, std::string type, int fileID = -1); // not the full path
		void PrintFileTreeRec(int currentDir, int depth);

		std::mutex fileIO;
		std::mutex requestLock;
		std::vector<vFile> virtualFileList;
		std::vector<vDir> virtualDirectoryList;
		std::queue<FileRequest> fileRequestList; // for async file loading

		// for functions that need to return objects but the searched object isnt found; ex : trying to get file that doenst exist
		vFile noFile{ "noFile" };
		vDir noDir{ "noDir" };
	};
}