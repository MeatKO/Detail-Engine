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

				if (path[i] == '\\' && path[i] == '/')
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

	bool VirtualFileSystem::vLoadFile(std::string fullPath, std::string virtualPath)
	{
		std::string sanitizedPath = vfsSanitizeFilePath(fullPath);
		FilePathInfo pathInfo = vfsGetFilePathInfo(sanitizedPath);
		
		if (pathInfo.name.size() > 0 && pathInfo.type.size() > 0)
		{
			vFile newFile;
		
			std::lock_guard<std::mutex> mut(fileIO);
		
			if (LoadFile(newFile, pathInfo.path, pathInfo.name, pathInfo.type))
			{
				newFile.fileVirtualPath = virtualPath;

				if (newFile.byteSize > 0)
				{
					newFile.id = fileTree.virtualFileList.size();

					fileTree.virtualFileList.push_back(newFile);

					int parentID = vEnsurePath(virtualPath);

					vAddFile(parentID, newFile.id);

					return true;
				}
		
				pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Info"),
					std::string("File '" + pathInfo.name + "." + pathInfo.type + "' had a size of 0 and will not be added.")));
		
				return false;
			}
		}
		else
		{
			pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Error"),
				std::string("PathInfo error for Path '" + sanitizedPath + "' - Name '" + pathInfo.name + "' - Type '" + pathInfo.type + "'.")));
		}

		return false;
	}

	bool VirtualFileSystem::vFreeFile(std::string virtualPath)
	{
		//return fileTree.vftFreeFile(virtualPath);
	}

	bool VirtualFileSystem::vReloadFile(std::string virtualPath)
	{

		//return fileTree.vftReloadFile(virtualPath);
	}

	vFile VirtualFileSystem::vGetVirtualFile(std::string fullVirtualPath)
	{
		std::string sanitizedPath = vfsSanitizeFilePath(fullVirtualPath);
		FilePathInfo pathInfo = vfsGetFilePathInfo(sanitizedPath);

		if (pathInfo.name.size() > 0 && pathInfo.type.size() > 0)
		{
			int targetDir = vGetPathTargetID(pathInfo.path);

			if (targetDir == -1)
			{
				pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Error"),
					std::string("Cannot Add vFile '" + pathInfo.name + "." + pathInfo.type + "' to Directory 'root'.")));

				return defaultFile;
			}

			// check if the dir contains the file

			for (int i = 0; i < fileTree.virtualDirectoryList[targetDir].fileIDs.size(); ++i)
			{
				int currentFileID = fileTree.virtualDirectoryList[targetDir].fileIDs[i];

				// check if the current file ID is valid
				if (currentFileID >= 0 && currentFileID < fileTree.virtualFileList.size())
				{
					if (fileTree.virtualFileList[currentFileID].fileName == pathInfo.name && fileTree.virtualFileList[currentFileID].fileType == pathInfo.type)
					{
						return fileTree.virtualFileList[currentFileID];
					}
				}
			}
		}

		pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Error"),
			std::string("Tried to get vFile but only path '" + sanitizedPath + "' was provided.")));

		return defaultFile;
	}

	int VirtualFileSystem::vAddDir(int parentID, std::string newDirName)
	{
		if (parentID >= 0 && parentID < fileTree.virtualDirectoryList.size())
		{
			std::string parentDir = vTraversePath(parentID);

			if (newDirName.size() > 0)
			{
				if (vDirContainsDir(parentID, newDirName) == -1)
				{
					int newDirID = fileTree.virtualDirectoryList.size();
					fileTree.virtualDirectoryList.push_back(vDir(newDirName, newDirID, parentID));
					fileTree.virtualDirectoryList[parentID].subDirs.push_back(newDirID);

					pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Info"),
						std::string("Directory '" + newDirName + "' was successfully added to '" + parentDir + "'.")));
					return newDirID;
				}
				pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Error"),
					std::string("Directory '" + parentDir + "' already contains Sub-Dir called '" + newDirName + "'.")));
				return -1;
			}
			pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Error"),
				std::string("Cannot Add Directory '" + newDirName + "' to '" + parentDir + "'.")));
			return -1;
		}
		pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Error"),
			std::string("Couldn't Add Directory '" + newDirName + "' to invalid Parent [" + std::to_string(parentID) + "].")));
		return -1;
	}

	void VirtualFileSystem::vAddFile(int parentID, int fileID)
	{
		if (parentID >= 0 && parentID < fileTree.virtualDirectoryList.size())
		{
			std::string parentPath = vTraversePath(parentID);

			if (fileID >= 0 && fileID < fileTree.virtualFileList.size())
			{
				std::string fileInfo = fileTree.virtualFileList[fileID].fileName + "." + fileTree.virtualFileList[fileID].fileType;
				
				for (int i = 0; i < fileTree.virtualDirectoryList[parentID].fileIDs.size(); ++i)
				{
					if (fileTree.virtualDirectoryList[parentID].fileIDs[i] == fileID)
					{
						pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Error"),
							std::string("Directory '" + parentPath + "' already contains File '" + fileInfo + "'.")));
						return;
					}
				}

				pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Info"),
					std::string("File " + fileInfo + " was successfully Added to '" + parentPath + "'.")));
				fileTree.virtualDirectoryList[parentID].fileIDs.push_back(fileID);
				return;
			}

			pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Error"),
				std::string("Couldn't Add unexisting File [" + std::to_string(fileID) + "] to Directory '" + parentPath + "'.")));
			return;
		}
		pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Error"),
			std::string("Couldn't Add File [" + std::to_string(fileID) + "] to an unexisting Directory [" + std::to_string(parentID) + "].")));
	}

	int VirtualFileSystem::vIsDirValid(std::string virtualPath)
	{
		std::string sanitizedPath = vfsSanitizeFilePath(virtualPath);
		std::vector<std::string> tokens = vfsGetPathTokens(sanitizedPath);

		int currentIndex = 0;

		if (tokens.size() > 0)
		{
			for (int i = (tokens[0] == "root"); i < tokens.size(); ++i)
			{
				int containedDirIndex = vDirContainsDir(currentIndex, tokens[i]);

				if (containedDirIndex >= 0)
					currentIndex = containedDirIndex;
				else
					return -1;
			}

			return currentIndex;
		}

		pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Error"),
			std::string("Cannot validate empty path '" + virtualPath + "'.")));

		return -1;
	}

	int VirtualFileSystem::vDirContainsDir(int dirID, std::string subDirName)
	{
		if (dirID >= 0 && dirID < fileTree.virtualDirectoryList.size())
		{
			for (int& currentID : fileTree.virtualDirectoryList[dirID].subDirs)
			{
				if (currentID >= 0 && currentID < fileTree.virtualDirectoryList.size())
				{
					if (fileTree.virtualDirectoryList[currentID].name == subDirName)
						return currentID;
				}
			}
		}

		return -1;
	}

	int VirtualFileSystem::vEnsurePath(std::string virtualPath)
	{
		std::string sanitizedPath = vfsSanitizeFilePath(virtualPath);
		FilePathInfo pathInfo = vfsGetFilePathInfo(sanitizedPath);
		std::vector<std::string> tokens = vfsGetPathTokens(pathInfo.path);

		int currentIndex = 0;

		if (tokens.size() > 0)
		{
			for (int i = (tokens[0] == "root"); i < tokens.size(); ++i)
			{
				int containedDirIndex = vDirContainsDir(currentIndex, tokens[i]);

				if (containedDirIndex == -1)
					containedDirIndex = vAddDir(currentIndex, tokens[i]);

				currentIndex = containedDirIndex;
			}

			return currentIndex;
		}

		pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Error"),
			std::string("Cannot validate empty path '" + virtualPath + "'.")));

		return -1;
	}

	int VirtualFileSystem::vGetPathTargetID(std::string virtualPath)
	{
		std::string sanitizedPath = vfsSanitizeFilePath(virtualPath);
		FilePathInfo pathInfo = vfsGetFilePathInfo(sanitizedPath);
		std::vector<std::string> tokens = vfsGetPathTokens(pathInfo.path);

		int currentIndex = 0;

		if (tokens.size() > 0)
		{
			for (int i = (tokens[0] == "root"); i < tokens.size(); ++i)
			{
				int containedDirIndex = vDirContainsDir(currentIndex, tokens[i]);

				if (containedDirIndex == -1)
					return -1;

				currentIndex = containedDirIndex;
			}

			return currentIndex;
		}

		return -1;
	}

	std::string VirtualFileSystem::vTraversePath(int dirID)
	{
		std::vector<std::string> directoryNames;
		std::string outString;
		int currentID = dirID;

		while (currentID != -1)
		{
			if (currentID >= 0 && currentID < fileTree.virtualDirectoryList.size())
			{
				directoryNames.push_back(fileTree.virtualDirectoryList[currentID].name);
			}

			currentID = fileTree.virtualDirectoryList[currentID].parentID;
		}

		for (int i = directoryNames.size() - 1; i >= 0; --i)
		{
			outString += directoryNames[i] + "/";
		}

		return outString;
	}

	bool VirtualFileSystem::vCheckParents(int dirID, int searchedID)
	{
		return false;
	}

	void VirtualFileSystem::Terminate()
	{
		std::lock_guard<std::mutex> mut(fileIO);
		int totalNonEmpty = 0;

		for (vFile& file : fileTree.virtualFileList)
		{
			if (file.byteSize > 0)
				totalNonEmpty++;
		}

		pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Info"), std::string("Total number of files loaded : " + std::to_string(fileTree.virtualFileList.size()))));
		pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Info"), std::string("Total number of files loaded : " + std::to_string(totalNonEmpty) + " ( Size > 0 )")));

		for (vFile& file : fileTree.virtualFileList)
		{
			if (file.byteSize > 0)
			{
				pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Info"),
					std::string("File '" + file.fileName + "." + file.fileType + "' was deleted successfully. Freed " + std::to_string(file.byteSize) + " bytes")));
		
				// Deletes the file data and resets the file size
				file.Terminate();
			}
		}

		pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Info"), std::string("Termination successful.")));

	}

	void VirtualFileSystem::CheckFileModifications()
	{

	}

	bool VirtualFileSystem::LoadFile(vFile& newFile, std::string path, std::string name, std::string type)
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

	void VirtualFileSystem::RecPrintTree(int index, int depth)
	{
		std::string spacing(depth, ' ');

		// check if the index is valid
		if (index >= 0 && index < fileTree.virtualDirectoryList.size())
		{
			// loop through the files
			for (int i = 0; i < fileTree.virtualDirectoryList[index].fileIDs.size(); ++i)
			{
				int currentFileID = fileTree.virtualDirectoryList[index].fileIDs[i];

				// check if the index is valid
				if (currentFileID >= 0 && currentFileID < fileTree.virtualFileList.size())
				{
					std::cout << spacing << ">" << fileTree.virtualFileList[currentFileID].fileName << "." << fileTree.virtualFileList[currentFileID].fileType << "\n";
				}
			}

			// loop through the sub directories
			for (int i = 0; i < fileTree.virtualDirectoryList[index].subDirs.size(); ++i)
			{
				int currentSubDir = fileTree.virtualDirectoryList[index].subDirs[i];

				// check if the index is valid
				if (currentSubDir >= 0 && currentSubDir < fileTree.virtualDirectoryList.size())
				{
					std::cout << spacing << "-" << fileTree.virtualDirectoryList[currentSubDir].name << "\n";
					RecPrintTree(currentSubDir, depth + 1);
				}
			}
		}
	}

	VirtualFileSystem::VirtualFileSystem()
	{
		defaultFilePathInfo.name = "DEFAULT";
		defaultFilePathInfo.type = "DEFAULT";
		defaultFilePathInfo.path = "DEFAULT";
	}

	VirtualFileSystem::~VirtualFileSystem()
	{
		Terminate();
	}

	void VirtualFileSystem::vPrintTree()
	{
		std::cout << "-root\n";
		RecPrintTree(0, 1);
	}

	vDir::vDir(std::string Name)
	{
		name = Name;
	}

	vDir::vDir(std::string Name, int DirID, int ParentID)
	{
		name = Name;
		dirID = DirID;
		parentID = ParentID;
	}
}