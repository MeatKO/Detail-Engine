#include "VFS.hpp"

namespace detailEngine
{
	std::ostream& operator<<(std::ostream& stream, const FilePathInfo& info)
	{
		stream << "FilePathInfo : PATH '" << info.path << "' NAME '" << info.name << "' TYPE '" << info.type << "'.";
		return stream;
	}

	bool vfsStringContainsOnly(std::string input, char containedChar)
	{
		for (int i = 0; i < input.size(); ++i)
		{
			if (input[i] != containedChar)
				return false;
		}

		return true;
	}

	std::string vfsRemovePathSlashes(std::string path)
	{
		std::string outPathString = "";
		bool previousCharSlash = true; // to remove the first slash

		for (int i = 0; i < path.size(); ++i)
		{
			if (previousCharSlash)
			{
				if (path[i] != '\\' && path[i] != '/')
				{
					outPathString += path[i];
					previousCharSlash = false;
				}
			}
			else
			{
				outPathString += path[i];

				if (path[i] == '\\' || path[i] == '/')
					previousCharSlash = true;
			}
		}

		return outPathString;
	}

	std::string vfsStandardisePathToken(std::string token)
	{
		std::string outToken = "";
		bool previousSpace = true; // to remove the first space as well

		for (int i = 0; i < token.size(); ++i)
		{
			if (previousSpace)
			{
				if (token[i] != ' ')
				{
					outToken += token[i];
					previousSpace = false;
				}
			}
			else
			{
				outToken += token[i];

				if (token[i] == ' ')
					previousSpace = true;
			}
		}

		if (outToken.back() == ' ')
			outToken.pop_back();

		for (int i = 0; i < outToken.size(); ++i)
		{
			if (outToken[i] == ' ')
				outToken[i] = '_';
		}

		return outToken;
	}

	std::string vfsRemovePathInvalidSymbols(std::string path)
	{
		std::string outPathString = "";

		for (int i = 0; i < path.size(); ++i)
		{
			// if the character isnt equal to ? & | < > : or "
			if (!(path[i] == '?' || path[i] == '*' || path[i] == '|' || path[i] == '<' || path[i] == '>' || path[i] == ':' || path[i] == '"'))
				outPathString += path[i];
		}
		return outPathString;
	}

	std::string vfsSanitizeFilePath(std::string path)
	{
		std::string outPathString = vfsRemovePathInvalidSymbols(path);
		outPathString = vfsRemovePathSlashes(path);

		for (int i = 0; i < outPathString.size(); ++i)
		{
			if (outPathString[i] == '\\')
				outPathString[i] = '/';
		}

		return outPathString;
	}

	std::string vfsAssemblePath(std::vector<std::string> tokens)
	{
		std::string out;

		for (int i = 0; i < tokens.size(); ++i)
		{
			out += tokens[i] + '/';
		}

		return out;
	}

	void vfsStandardisePathTokens(std::vector<std::string>& tokens)
	{
		for (int i = 0; i < tokens.size(); ++i)
		{
			tokens[i] = vfsStandardisePathToken(tokens[i]);
		}
	}

	std::vector<std::string> vfsGetPathTokens(std::string path)
	{
		std::vector<std::string> tokens;
		std::string currentToken = "";

		for (int i = 0; i < path.size(); ++i)
		{
			// check if the current char is / and push the currentToken to the tokens list
			if (path[i] == '/')
			{
				// check if the currentToken string is not empty
				if (currentToken.size() > 0)
				{
					tokens.push_back(currentToken);
					currentToken = "";
				}
			}
			else
			{
				currentToken += path[i];
			}
		}

		// if the last character of the path is not / then the last token wont get pushed by the loop above
		if (path.back() != '/')
		{
			if (currentToken.size() > 0)
			{
				tokens.push_back(currentToken);
				currentToken = "";
			}
		}

		for (int i = tokens.size() - 1; i >= 0; --i)
		{
			std::string filteredToken = vfsStandardisePathToken(tokens[i]);

			if (filteredToken == "")
			{
				tokens.erase(tokens.begin() + i);
			}
			else
			{
				tokens[i] = filteredToken;
			}
		}

		return tokens;
	}

	int vfsStringContains(std::string input, char character)
	{
		for (int i = 0; i < input.size(); ++i)
		{
			if (input[i] == character)
				return i;
		}

		return -1;
	}

	bool vfsEnsureDirectory(std::string path)
	{
		path = vfsSanitizeFilePath(path);
		std::vector<std::string> tokens = vfsGetPathTokens(path);

		// since we ensure only the dir, the file should be removed
		if (vfsStringContains(tokens.back(), '.') != -1)
			tokens.pop_back();

		std::string currentPath = "";

		// for whatever reason create_directories breaks miserably so i have to loop em
		for (int i = 0; i < tokens.size(); ++i)
		{
			// gradually add the directories and create them one by one
			currentPath += tokens[i] + '/';

			if (!fs::exists(currentPath))
			{
				if (!fs::create_directory(currentPath))
					return false;
			}
		}

		return true;
	}

	FilePathInfo vfsGetFilePathInfo(std::string path)
	{
		FilePathInfo info;

		path = vfsSanitizeFilePath(path);
		std::vector<std::string> pathTokens = vfsGetPathTokens(path);

		if (pathTokens.size() > 0)
		{
			// if the path string contains a file and filetype then the last token will be file.filetype
			if (vfsStringContains(pathTokens.back(), '.') != -1)
			{
				int dotIndex = 0;

				for (int i = 0; i < pathTokens.back().size(); ++i)
				{
					if (pathTokens.back()[i] == '.')
					{
						dotIndex = i;
						break;
					}

					info.name += pathTokens.back()[i];
				}

				for (int i = dotIndex + 1; i < pathTokens.back().size(); ++i)
				{
					info.type += pathTokens.back()[i];
				}

				pathTokens.pop_back(); // remove the file name and type to leve only the path
			}

			info.path = vfsAssemblePath(pathTokens);

		}

		return info;
	}

	bool vfsPathExists(std::string path)
	{
		struct stat info;

		int statRC = stat(vfsSanitizeFilePath(path).c_str(), &info);
		if (statRC != 0)
		{
			if (errno == ENOENT) { return 0; } // something along the path does not exist
			if (errno == ENOTDIR) { return 0; } // something in path prefix is not a dir
			return false;
		}

		return (info.st_mode & S_IFDIR) ? true : false;
	}

	bool vfsPhysicalFileExists(std::string filePath)
	{
		struct stat info;
		int statRC = stat(vfsSanitizeFilePath(filePath).c_str(), &info);

		if (statRC == 0)
			return true;

		return false;
	}

	time_t vfsLastModified(std::string path)
	{
		struct stat info;

		int statRC = stat(path.c_str(), &info);

		if (statRC == 0)
		{
			return info.st_mtime;
		}

		return -1;
	}

	bool vfsLoadFile(vFile& newFile, std::string path)
	{
		std::string sanitizedPath = vfsSanitizeFilePath(path);
		FilePathInfo pathInfo = vfsGetFilePathInfo(sanitizedPath);

		std::string filePath = pathInfo.path + pathInfo.name + "." + pathInfo.type;

		std::ifstream file_in(filePath, std::ios::in | std::ios::binary | std::ios::ate);

		if (file_in.is_open())
		{
			newFile.fileName = pathInfo.name;
			newFile.fileType = pathInfo.type;
			newFile.filePhysicalPath = pathInfo.path;
			newFile.byteSize = file_in.tellg();
			newFile.data = new unsigned char[newFile.byteSize];

			file_in.seekg(0);
			file_in.read((char*)newFile.data, newFile.byteSize);

			file_in.close();

			return true;
		}

		return false;
	}

	vDir::vDir(std::string Name, int DirID, int ParentID)
	{
		name = Name;
		id = DirID;
		parentID = ParentID;
	}

	bool VirtualFileSystem::LoadFile(vFile& newFile, std::string path, std::string name, std::string type, int fileID)
	{
		std::string filePath = path + name + "." + type;

		std::ifstream file_in(filePath, std::ios::in | std::ios::binary | std::ios::ate);

		if (file_in.is_open())
		{
			newFile.fileName = name;
			newFile.fileType = type;
			newFile.filePhysicalPath = path;
			newFile.byteSize = file_in.tellg();
			newFile.data = new unsigned char[newFile.byteSize];
			newFile.id = fileID;

			file_in.seekg(0);
			file_in.read((char*)newFile.data, newFile.byteSize);

			file_in.close();

			return true;
		}
		else
		{
			pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Error"), std::string("Cannot open File '" + name + "." + type + "' at '" + path + "'.")));
		}

		return false;
	}

	int VirtualFileSystem::AddDirToList(std::string dirName, int dirParent)
	{
		std::lock_guard<std::mutex> mut(fileIO);
		int lastID = virtualDirectoryList.size();
		virtualDirectoryList.push_back(vDir(dirName, lastID, dirParent));
		return lastID;
	}

	void VirtualFileSystem::PrintFileTreeRec(int currentDir, int depth)
	{
		if (currentDir >= 0 && currentDir < virtualDirectoryList.size())
		{
			vDir& directory = virtualDirectoryList[currentDir];

			std::cout << std::string (depth, ' ') << "|-" << directory.name << " [" << currentDir << "]" << "\n";

			for (int i = 0; i < directory.filesIDList.size(); ++i)
			{
				vFile& currentFile = virtualFileList[directory.filesIDList[i]];
				std::cout << std::string(depth + 1, ' ') << "|-" << currentFile.fileName << '.' << currentFile.fileType << " [" << currentFile.id << "]" << "\n";
			}

			for (int i = 0; i < directory.directoriesIDList.size(); ++i)
			{
				PrintFileTreeRec(directory.directoriesIDList[i], depth + 1);
			}
		}
	}

	/*-------------------------------------------------------------------------------------------------Constructors------*/

	VirtualFileSystem::VirtualFileSystem()
	{
		// Adding a "root" directory as the first one with id = 0 and parent id = -1
		virtualDirectoryList.push_back(vDir("root", 0, -1));
	}

	/*-------------------------------------------------------------------------------------------------Termination-------*/

	VirtualFileSystem::~VirtualFileSystem()
	{
		Terminate();
	}

	void VirtualFileSystem::Update()
	{
		ProcessFileRequests();
	}

	void VirtualFileSystem::Terminate()
	{
		std::lock_guard<std::mutex> mut(fileIO);
		int totalNonEmpty = 0;

		//for (vFile& file : fileTree.virtualFileList)
		//{
		//	if (file.byteSize > 0)
		//		totalNonEmpty++;
		//}
		//
		//pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Info"), std::string("Total number of files loaded : " + std::to_string(fileTree.virtualFileList.size()))));
		pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Info"), std::string("Total number of files loaded : " + std::to_string(totalNonEmpty) + " ( Size > 0 )")));

		//for (vFile& file : fileTree.virtualFileList)
		//{
		//	if (file.byteSize > 0)
		//	{
		//		pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Info"),
		//			std::string("File '" + file.fileName + "." + file.fileType + "' was deleted successfully. Freed " + std::to_string(file.byteSize) + " bytes")));
		//
		//		// Deletes the file data and resets the file size
		//		file.Terminate();
		//	}
		//}
		//
		//pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Info"), std::string("Termination successful.")));

	}

	/*-------------------------------------------------------------------------------------------------Routines----------*/

	std::vector<int> VirtualFileSystem::CheckForFileModifications()
	{
		std::lock_guard<std::mutex> mut(fileIO);
		std::vector<int> modifiedFileIDs;

		for (int i = 0; i < virtualFileList.size(); ++i)
		{
			time_t lastTime = vfsLastModified(virtualFileList[i].filePhysicalPath);

			// if the last modification time of the virtual file is not equal to the last time of the physical file, then add the file id to the modified file list
			if (lastTime > 0)
			{
				if (virtualFileList[i].lastModified != lastTime)
					modifiedFileIDs.push_back(i);
			}
		}

		return modifiedFileIDs;
	}

	void VirtualFileSystem::PrintFileTree()
	{
		PrintFileTreeRec(0, 0);
	}

	int VirtualFileSystem::vMakeDir(std::string virtualDirectory)
	{
		std::string sanitizedPath = vfsSanitizeFilePath(virtualDirectory);
		std::vector<std::string> pathTokens = vfsGetPathTokens(sanitizedPath);

		if (pathTokens.size() > 0)
		{
			// adding an offset to the loop variable so that we dont add root to root if the path begins with it
			bool firstRoot = false;
			if (pathTokens[0] == "root")
				firstRoot = true;

			int currentDirID = 0;

			for (int i = firstRoot; i < pathTokens.size(); ++i)
			{
				int searchedDirID = DirContainsDir(currentDirID, pathTokens[i]);

				if (searchedDirID != -1)
				{
					currentDirID = searchedDirID;
				}
				else
				{
					int newID = AddDirToList(pathTokens[i], currentDirID);
					virtualDirectoryList[currentDirID].directoriesIDList.push_back(newID);
					currentDirID = newID;
				}
			}

			return currentDirID;
		}

		pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Error"), std::string("Cannot create empty Virtual Dir.")));

		return 0;
	}

	int VirtualFileSystem::DirContainsDir(int DirID, std::string DirName)
	{
		std::lock_guard<std::mutex> mut(fileIO);

		if (DirID >= 0 && DirID < virtualDirectoryList.size())
		{
			vDir& directory = virtualDirectoryList[DirID];

			for (int i = 0; i < directory.directoriesIDList.size(); ++i)
			{
				if (virtualDirectoryList[directory.directoriesIDList[i]].name == DirName)
				{
					return directory.directoriesIDList[i];
				}
			}
		}

		return -1;
	}

	int VirtualFileSystem::DirContainsFile(int DirID, std::string FileName, std::string FileType)
	{
		std::lock_guard<std::mutex> mut(fileIO);

		if (DirID >= 0 && DirID < virtualDirectoryList.size())
		{
			vDir& directory = virtualDirectoryList[DirID];

			for (int i = 0; i < directory.filesIDList.size(); ++i)
			{
				if (virtualFileList[directory.filesIDList[i]].fileName == FileName && virtualFileList[directory.filesIDList[i]].fileType == FileType)
				{
					return directory.filesIDList[i];
				}
			}
		}

		return -1;
	}

	int VirtualFileSystem::vGetDirID(std::string virtualPath)
	{
		std::string sanitizedPath = vfsSanitizeFilePath(virtualPath);
		std::vector<std::string> pathTokens = vfsGetPathTokens(sanitizedPath);

		int currentDirID = 0;

		if (pathTokens.size() > 0)
		{
			// adding an offset to the loop variable so that we dont search for root inside root if the path begins with it
			bool firstRoot = false;
			if (pathTokens[0] == "root")
				firstRoot = true;

			for (int i = firstRoot; i < pathTokens.size(); ++i)
			{
				int searchedDirID = DirContainsDir(currentDirID, pathTokens[i]);

				if (searchedDirID != -1)
				{
					currentDirID = searchedDirID;
				}
				else
				{
					return -1;
				}
			}
		}

		return currentDirID;
	}

	void VirtualFileSystem::vLoadFile(std::string physicalPath, std::string virtualPath)
	{
		std::string sanitizedPath = vfsSanitizeFilePath(physicalPath);
		std::string sanitizedVirtualPath = vfsSanitizeFilePath(virtualPath);
		FilePathInfo pathInfo = vfsGetFilePathInfo(physicalPath);

		// Check if the file already exists in the target directory
		// Check if the target directory exists first...
		int parentDirID = vGetDirID(sanitizedVirtualPath);
		if (parentDirID >= 0)
		{
			// Dir exists...
			if (DirContainsFile(parentDirID, pathInfo.name, pathInfo.type) >= 0)
			{
				std::string fileNameType = pathInfo.name + "." + pathInfo.type;
				pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Error"), std::string("vFile '" + fileNameType + "' already exists at vDir '" + sanitizedVirtualPath + "'.")));
				return;
			}
		}

		int newFileID = AddFileToList(physicalPath);
		if (newFileID >= 0)
		{
			int parentDirID = vMakeDir(sanitizedVirtualPath);
			virtualFileList[newFileID].parentID = parentDirID;
			virtualDirectoryList[parentDirID].filesIDList.push_back(newFileID);

			pSendMessage(Message(MSG_FILE_LOADED, std::string(sanitizedPath), pathInfo));

			return;
		}

		pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Error"), std::string("Cannot load vFile from Path '" + sanitizedPath + "'.")));
	}

	vFile VirtualFileSystem::vGetFile(std::string fullVirtualPath)
	{
		std::string sanitizedPath = vfsSanitizeFilePath(fullVirtualPath);
		FilePathInfo pathInfo = vfsGetFilePathInfo(sanitizedPath);

		int directoryID = vGetDirID(pathInfo.path);

		if (directoryID >= 0)
		{
			int fileID = DirContainsFile(directoryID, pathInfo.name, pathInfo.type);
			if (fileID >= 0)
			{
				return virtualFileList[fileID];
			}
		}

		return noFile;
	}

	bool VirtualFileSystem::vFileExists(std::string fullVirtualPath)
	{
		std::string sanitizedPath = vfsSanitizeFilePath(fullVirtualPath);
		FilePathInfo pathInfo = vfsGetFilePathInfo(sanitizedPath);

		int directoryID = vGetDirID(pathInfo.path);

		if (directoryID >= 0)
		{
			int fileID = DirContainsFile(directoryID, pathInfo.name, pathInfo.type);
			if (fileID >= 0)
			{
				return true;
			}
		}

		return false;
	}

	void VirtualFileSystem::vLoadFileAsync(std::string physicalPath, std::string virtualPath)
	{
		std::lock_guard<std::mutex> mut(requestLock);
		FileRequest request {physicalPath, virtualPath};
		fileRequestList.push(request);
	}

	void VirtualFileSystem::ProcessFileRequests()
	{
		std::lock_guard<std::mutex> mut(requestLock);

		while (fileRequestList.size() > 0)
		{
			FileRequest& request = fileRequestList.back();
			vLoadFile(request.physicalPath, request.virtualPath);
			fileRequestList.pop();
		}
	}

	int VirtualFileSystem::AddFileToList(std::string physicalPath)
	{
		std::string sanitizedPath = vfsSanitizeFilePath(physicalPath);
		FilePathInfo pathInfo = vfsGetFilePathInfo(sanitizedPath);

		if (pathInfo.name.size() == 0 || pathInfo.type.size() == 0)
		{
			pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Error"), std::string("Loading vFile from '" + physicalPath + "' Resulted in empty file name or type.")));
			return -1;
		}

		int newFileID = virtualFileList.size();
		virtualFileList.push_back(vFile());
		if (LoadFile(virtualFileList.back(), pathInfo.path, pathInfo.name, pathInfo.type, newFileID))
		{
			return newFileID;
		}

		return -1;
	}

}