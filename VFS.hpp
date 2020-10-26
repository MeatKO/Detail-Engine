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

	class Pack
	{
	public:
		Pack() {}
	};

	class vFile
	{
	public:
		vFile() {}
		vFile(std::string FileName, std::string FileType, std::string FilePath = "", int fileID = -1)
		{
			fileName = FileName;
			fileType = FileType;
			filePhysicalPath = FilePath;
			id = fileID;
		}
		vFile(std::string FileName, std::string FileType, std::string FilePath, int fileID, unsigned char* Data, int ByteSize)
		{
			fileName = FileName;
			fileType = FileType;
			filePhysicalPath = FilePath;
			data = Data;
			byteSize = ByteSize;
			id = fileID;
		}

		// for manual destruction of the data when the engine terminates
		// i do it this way because i might copy the pointer into other objects and i dont want it to get deleted
		// i might've used smart pointers but eh
		void Terminate()
		{
			if ((data != nullptr) && (byteSize >= 0))
			{
				byteSize = -1;
				delete[] data;
			}
		}

		int id = -1;
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
		vDir(std::string Name);
		vDir(std::string Name, int DirID, int ParentID);

		int dirID = -1, parentID = -1;
		std::string name;
		std::vector<int> subDirs;
		std::vector<int> fileIDs;
	};

	class vFileTree
	{
	public:
		vFileTree()
		{
			virtualDirectoryList.push_back(vDir("root", 0, -1));
		}

		std::vector<vFile> virtualFileList;
		std::vector<vDir> virtualDirectoryList;
	};

	class VirtualFileSystem : public Publisher, public Subscriber
	{
	public:
		VirtualFileSystem();
		~VirtualFileSystem();

		void vPrintTree();
		bool vLoadFile(std::string fullPath, std::string virtualPath);
		bool vFreeFile(std::string virtualPath);
		bool vReloadFile(std::string virtualPath);

		vFile vGetVirtualFile(std::string fullVirtualPath);

		// will return the dirID
		int vAddDir(int parentID, std::string newDirName);
		// false - file already exists
		void vAddFile(int parentID, int fileID);
		int vIsDirValid(std::string virtualPath);
		int vDirContainsDir(int dirID, std::string subDirName);
		int vEnsurePath(std::string virtualPath);
		int vGetPathTargetID(std::string virtualPath); // Returns the dirID of the last dir in the chain
		std::string vTraversePath(int dirID);
		bool vCheckParents(int dirID, int searchedID); // Check if the searched ID is the dir's parent

		void Terminate();
		void CheckFileModifications(); // checks if any of the loaded files were modified on the disk

	private:
		bool LoadFile(vFile& newFile, std::string path, std::string name, std::string type);
		void RecPrintTree(int index, int depth);

		vFile defaultFile { "DEFAULT", "DEFAULT", "DEFAULT", -1 };
		std::mutex fileIO;
		vFileTree fileTree;
		FilePathInfo defaultFilePathInfo; // all fields will be set to "DEFAULT"
	};
}