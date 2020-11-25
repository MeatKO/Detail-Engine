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
		vDir(std::string Name, int DirID, int ParentID);

		int id = -1;
		int parentID = -1;
		std::string name;
		std::vector<int> directoriesIDList;
		std::vector<int> filesIDList;
	};

	class VirtualFileSystem : public Publisher, public Subscriber
	{
	public:
		VirtualFileSystem();
		~VirtualFileSystem();

		void Update();

		void Terminate();
		std::vector<int> CheckForFileModifications(); // checks if the loaded files were modified on disk
		void PrintFileTree();
		
		int vMakeDir(std::string virtualDirectory); // creates the directory and returns the ID of the last directory in the path
		int DirContainsDir(int DirID, std::string DirName); // returns the ID of the contained dir, returns -1 if the dir is not found
		int DirContainsFile(int DirID, std::string FileName, std::string FileType); // returns the ID of the contained file, returns -1 if the file is not found
		int vGetDirID(std::string virtualPath); // returns the ID of the last directory in the path

		void vLoadFile(std::string physicalPath, std::string virtualPath);
		vFile vGetFile(std::string fullVirtualPath);
		bool vFileExists(std::string fullVirtualPath);

		void vLoadFileAsync(std::string physicalPath, std::string virtualPath); // a file load request
		void ProcessFileRequests();

	private:
		bool LoadFile(vFile& newFile, std::string path, std::string name, std::string type, int fileID = -1); // not the full path
		int AddDirToList(std::string dirName, int dirParent);
		int AddFileToList(std::string physicalPath);
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