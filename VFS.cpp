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

	std::string vfsNormalizeVirtualFilePath(std::string path)
	{
		std::string sanitizedPath = vfsSanitizeFilePath(path);
		std::vector<std::string> pathTokens = vfsGetPathTokens(sanitizedPath);
		std::vector<std::string> newTokens;
		bool homeDir = false; // to make sure we dont add the home dir at every ~

		if (path.size() > 0)
		{
			if (path[0] == '/')
			{
				newTokens.push_back("root");
			}
			else
			{
				if (pathTokens.size() > 0)
				{
					if ((pathTokens[0] != "root") && (pathTokens[0] != "~"))
					{
						newTokens.push_back("root");
						homeDir = true;
					}
				}
			}
		}

		for (int i = 0; i < pathTokens.size(); ++i)
		{
			if (pathTokens[i] == "~" && !homeDir)
			{
				newTokens.push_back("root/detail");
				homeDir = true;
			}
			else
			{
				newTokens.push_back(pathTokens[i]);
			}
		}

		return vfsAssemblePath(newTokens);
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

	void VirtualFileSystem::PrintFileTreeRec(int currentDir, int depth)
	{
		if (currentDir >= 0 && currentDir < virtualDirectoryList.size())
		{
			vDir& directory = virtualDirectoryList[currentDir];
		
			std::cout << std::string (depth, ' ') << "|-" << directory.name << " [" << currentDir << "]" << "\n";
		
			for (int i = 0; i < directory.subFileIDs.size(); ++i)
			{
				vFile& currentFile = virtualFileList[directory.subFileIDs[i]];
				std::cout << std::string(depth + 1, ' ') << "|-" << currentFile.fileName << '.' << currentFile.fileType << " [" << currentFile.id << "]" << "\n";
			}
		
			for (int i = 0; i < directory.subDirIDs.size(); ++i)
			{
				PrintFileTreeRec(directory.subDirIDs[i], depth + 1);
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
		//vProcessFileRequests();
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

	void VirtualFileSystem::PrintFileTree()
	{
		PrintFileTreeRec(0, 0);
	}

	/*-------------------------------------------------------------------------------------------------Routines----------*/

	std::vector<int> VirtualFileSystem::vCheckForFileModifications()
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

	void VirtualFileSystem::MakeDir(std::string virtualDirPath)
	{
		std::string sanitizedPath = vfsSanitizeFilePath(virtualDirPath);
		std::string normalizedPath = vfsNormalizeVirtualFilePath(sanitizedPath);
		std::vector<std::string> pathTokens = vfsGetPathTokens(normalizedPath);

		int lastDirID = 0;

		for (int i = 1; i < pathTokens.size(); ++i)
		{
			// check if the index is valid
			if (lastDirID >= 0 && lastDirID < virtualDirectoryList.size())
			{
				int subDirID = 0;
				if (DirContainsDir(lastDirID, pathTokens[i], subDirID))
				{
					lastDirID = subDirID;
				}
				else
				{
					// the lastDirID doenst contain a dir called pathTokens[i] so we will make a new directory and add it 
					int newDirID = 0;
					newDirID = AddDirToList(pathTokens[i], lastDirID);
					AddDirToDir(lastDirID, newDirID);
					lastDirID = newDirID;
				}
			}
		}
	}

	std::string VirtualFileSystem::TraverseDirectory(int dirID)
	{
		std::vector<int> pathTokens;
		std::string backDir = "";
		int currentID = dirID;

		if ((currentID >= 0) && (currentID < virtualDirectoryList.size()))
		{
			while (currentID >= 0)
			{
				if ((currentID >= 0) && (currentID < virtualDirectoryList.size()))
				{
					pathTokens.push_back(currentID);
					currentID = virtualDirectoryList[currentID].parentID;
				}
				else
				{
					break;
				}
			}

			for (int i = (pathTokens.size() - 1); i >= 0; --i)
			{
				backDir += virtualDirectoryList[pathTokens[i]].name + "/";
			}
		}
		else
		{
			pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Error"), std::string("Cannot traverse path for invalid vDir [" + std::to_string(dirID) +"].")));
		}
		

		return backDir;
	}

	bool VirtualFileSystem::DirContainsDir(int parentDirID, std::string subDirName)
	{
		if (parentDirID >= 0)
		{
			if (parentDirID < virtualDirectoryList.size())
			{
				for (int i = 0; i < virtualDirectoryList[parentDirID].subDirIDs.size(); ++i)
				{
					int currentSubDirID = virtualDirectoryList[parentDirID].subDirIDs[i];

					if (virtualDirectoryList[currentSubDirID].name == subDirName)
					{
						return true;
					}
				}
			}
		}
		return false;
	}

	bool VirtualFileSystem::DirContainsDir(int parentDirID, std::string subDirName, int& subDirID)
	{
		if (parentDirID >= 0)
		{
			if (parentDirID < virtualDirectoryList.size())
			{
				for (int i = 0; i < virtualDirectoryList[parentDirID].subDirIDs.size(); ++i)
				{
					int currentSubDirID = virtualDirectoryList[parentDirID].subDirIDs[i];

					if (virtualDirectoryList[currentSubDirID].name == subDirName)
					{
						subDirID = currentSubDirID;
						return true;
					}
				}
			}
		}
		subDirID = -1;
		return false;
	}

	bool VirtualFileSystem::DirContainsDir(int parentDirID, int subDirID)
	{
		if (parentDirID != subDirID)
		{
			if ((parentDirID >= 0) && (subDirID >= 0))
			{
				if ((parentDirID < virtualDirectoryList.size()) && (subDirID < virtualDirectoryList.size()))
				{
					for (int i = 0; i < virtualDirectoryList[parentDirID].subDirIDs.size(); ++i)
					{
						int currentSubDirID = virtualDirectoryList[parentDirID].subDirIDs[i];

						if (currentSubDirID == subDirID)
						{
							return true;
						}
					}
				}
			}
		}
		return false;
	}

	int VirtualFileSystem::AddDirToList(std::string dirName, int dirParent)
	{
		std::lock_guard<std::mutex> mut(fileIO);
		int newDirID = virtualDirectoryList.size();
		virtualDirectoryList.push_back(vDir(dirName, newDirID, dirParent));
		return newDirID;
	}

	void VirtualFileSystem::AddDirToDir(int parentDirID, int subDirID)
	{
		if (parentDirID != subDirID)
		{
			if ((parentDirID >= 0) && (subDirID >= 0))
			{
				if ((parentDirID < virtualDirectoryList.size()) && (subDirID < virtualDirectoryList.size()))
				{
					if (!DirContainsDir(parentDirID, subDirID))
					{
						virtualDirectoryList[parentDirID].subDirIDs.push_back(subDirID);
						std::string parentName = virtualDirectoryList[parentDirID].name;
						std::string childrenName = virtualDirectoryList[subDirID].name;
						pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Info"), std::string("Successfully added vDir '" + childrenName + "' [" + std::to_string(subDirID) + "] to vDir '" + parentName + "' [" + std::to_string(parentDirID) + "].")));
						return;
					}
				}
			}
			pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Error"), std::string("Cannot add vDir [" + std::to_string(subDirID) + "] to vDir [" + std::to_string(parentDirID) + "].")));
			return;
		}
		pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Error"), std::string("Tried to add vDir [" + std::to_string(parentDirID) + "] to itself.")));
	}

	int VirtualFileSystem::GetDirID(std::string virtualDirPath)
	{
		std::string sanitizedPath = vfsSanitizeFilePath(virtualDirPath);
		std::string normalizedPath = vfsNormalizeVirtualFilePath(sanitizedPath);
		std::vector<std::string> pathTokens = vfsGetPathTokens(normalizedPath);

		// example dir : 
		// root/detail/models

		int lastDirID = 0;

		for (int i = 1; i < pathTokens.size(); ++i)
		{
			// check if the index is valid
			if (lastDirID >= 0 && lastDirID < virtualDirectoryList.size())
			{
				int subDirID = 0;
				if (DirContainsDir(lastDirID, pathTokens[i], subDirID))
				{
					lastDirID = subDirID;
				}
				else
				{
					pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Error"), std::string("Cannot get ID of invalid Path target '" + normalizedPath + "'.")));
					return -1;
				}
			}
			else
			{
				pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Error"), std::string("Cannot get ID of invalid Path target '" + normalizedPath + "'.")));
				return -1;
			}
		}

		return lastDirID;
	}

	int VirtualFileSystem::AddFileToList(std::string physicalPath, int fileParent)
	{
		std::string sanitizedPath = vfsSanitizeFilePath(physicalPath);
		FilePathInfo pathInfo = vfsGetFilePathInfo(sanitizedPath);
		vFile newFile;

		LoadFile(newFile, pathInfo.path, pathInfo.name, pathInfo.type);
		newFile.parentID = fileParent;

		std::lock_guard<std::mutex> mut(fileIO);

		int newFileID = virtualFileList.size();
		newFile.id = newFileID;
		virtualFileList.push_back(newFile);

		return newFileID;
	}
}