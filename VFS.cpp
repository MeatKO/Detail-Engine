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
			if (vfsStringContains(pathTokens.back(), '.'))
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
		//std::string sanitizedPath = vfsSanitizeFilePath(fullPath);
		//FilePathInfo pathInfo = vfsGetFilePathInfo(sanitizedPath);
		//
		//if (pathInfo.name.size() > 0 && pathInfo.type.size() > 0)
		//{
		//	vFile newFile;
		//
		//	std::lock_guard<std::mutex> mut(fileIO);
		//
		//	if (LoadFile(newFile, pathInfo.path, pathInfo.name, pathInfo.type))
		//	{
		//		if (newFile.byteSize > 0)
		//		{
		//			pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Info"),
		//				std::string("File '" + pathInfo.name + "." + pathInfo.type + "' was added successfully.")));
		//			virtualFileList.push_back(newFile);
		//
		//			return true;
		//		}
		//
		//		pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Info"),
		//			std::string("File '" + pathInfo.name + "." + pathInfo.type + "' had a size of 0 and will not be added.")));
		//
		//		return false;
		//	}
		//}
		//else
		//{
		//	pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Error"),
		//		std::string("PathInfo error for Path '" + sanitizedPath + "' - Name '" + pathInfo.name + "' - Type '" + pathInfo.type + "'.")));
		//	pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Info"),
		//		std::string("Sanitizing Path '" + fullPath + "' => '" + sanitizedPath + "'.")));
		//}

		return false;
	}

	//bool VirtualFileSystem::vRemoveFile(std::string fileName, std::string fileType)
	//{
	//	return vRemoveFile(GetFileIndex(fileName, fileType)); // literally the function below;
	//}

	//bool VirtualFileSystem::vRemoveFile(int fileID)
	//{
	//	std::lock_guard<std::mutex> mut(fileIO);
	//
	//	if (fileID >= 0 && fileID < virtualFileList.size())
	//	{
	//		pSendMessage(Message(MSG_LOG, std::string("Virtual FileSystem Info"),
	//			std::string("File '" + virtualFileList[fileID].fileName + "." + virtualFileList[fileID].fileType + "' was deleted successfully. Freed " + std::to_string(virtualFileList[fileID].byteSize) + " bytes")));
	//		
	//		// Deletes the file data and resets the file size
	//		virtualFileList[fileID].Terminate();
	//		virtualFileList[fileID].deleted = true;
	//	}
	//
	//	return false;
	//}
	//
	//void VirtualFileSystem::vEnsureVirtualDir(std::string path)
	//{
	//	fileTree.vftEnsurePath(path);
	//}

	//FilePathInfo VirtualFileSystem::GetFilePathInfo(std::string fileName, std::string fileType)
	//{
	//	return GetFilePathInfo(GetFileIndex(fileName, fileType));
	//}

	FilePathInfo VirtualFileSystem::GetFilePathInfo(int fileID)
	{
		if (fileID >= 0 && fileID < fileTree.virtualFileList.size())
		{
			FilePathInfo newInfo;
			newInfo.name = fileTree.virtualFileList[fileID].fileName;
			newInfo.type = fileTree.virtualFileList[fileID].fileType;
			newInfo.path = fileTree.virtualFileList[fileID].filePhysicalPath;

			return newInfo;
		}

		return defaultFilePathInfo;
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

	void VirtualFileSystem::PrintTree()
	{
		//fileTree.rootDir.RecPrintDirs(0);
	}

	//int VirtualFileSystem::GetFileIndex(std::string fileName, std::string fileType)
	//{
	//	std::lock_guard<std::mutex> mut(fileIO);
	//
	//	for (int i = 0; i < virtualFileList.size(); ++i)
	//	{
	//		if (virtualFileList[i].fileName == fileName && virtualFileList[i].fileType == fileType && !virtualFileList[i].deleted)
	//			return i;
	//	}
	//
	//	return -1;
	//}

	//int VirtualFileTree::vftCheckPath(std::string path)
	//{
	//	std::string sanitizedPath = vfsSanitizeFilePath(path);
	//
	//	// using FilePathInfo to filter the path in case there are file name & type (who knows)
	//	FilePathInfo pathInfo = vfsGetFilePathInfo(sanitizedPath);
	//	std::vector<std::string> pathTokens = vfsGetPathTokens(pathInfo.path);
	//
	//	if (pathTokens.size() > 0)
	//	{
	//		if (pathTokens[0] != "root")
	//		{
	//			// invalid path (missing root)
	//			return -1;
	//		}
	//		
	//		return rootDir.RecPathSearch(1, pathTokens);
	//	}
	//
	//	return 0;
	//}
	//
	//void VirtualFileTree::vftEnsurePath(std::string path)
	//{
	//	std::string sanitizedPath = vfsSanitizeFilePath(path);
	//
	//	// using FilePathInfo to filter the path in case there are file name & type (who knows)
	//	FilePathInfo pathInfo = vfsGetFilePathInfo(sanitizedPath);
	//	std::vector<std::string> pathTokens = vfsGetPathTokens(pathInfo.path);
	//
	//	rootDir.RecEnsurePath(1, pathTokens);
	//}
	//
	//bool VirtualFileTree::vftAddFile(std::string path, int fileID)
	//{
	//	return false;
	//}

	vDir::vDir(std::string Name)
	{
		name = Name;
	}

	bool VirtualFileTree::vftLoadFile(std::string fullPath, std::string virtualPath)
	{
		return false;
	}

	//bool VirtualDir::RecPathSearch(int index, std::vector<std::string>& tokens)
	//{
	//	if (index == tokens.size() - 1)
	//	{
	//		if (GetSubDirID(tokens.back()) >= 0)
	//		{
	//			return  true;
	//		}
	//		return false;
	//	}
	//
	//	int subDirID = GetSubDirID(tokens[index]);
	//	if (subDirID >= 0)
	//	{
	//		subDirs[subDirID].RecPathSearch(index + 1, tokens);
	//	}
	//	else
	//	{
	//		return false;
	//	}
	//}
	//
	//void VirtualDir::RecEnsurePath(int index, std::vector<std::string>& tokens)
	//{
	//	if (index == tokens.size())
	//		return;
	//
	//	int subDirID = GetSubDirID(tokens[index]);
	//	if (subDirID >= 0)
	//	{
	//		subDirs[subDirID].RecPathSearch(index + 1, tokens);
	//	}
	//	else
	//	{
	//		subDirs.push_back(VirtualDir(tokens[index]));
	//		subDirs.back().RecPathSearch(index + 1, tokens);
	//	}
	//}
	//
	//void VirtualDir::RecPrintDirs(int spacing)
	//{
	//	//std::string space = std::string(spacing, ' ');
	//	//
	//	//for (int i = 0; i < subDirs.size(); ++i)
	//	//{
	//	//	std::cout << space << "-" << subDirs[i].name << "\n";
	//	//	subDirs[i].RecPrintDirs(spacing + 1);
	//	//}
	//}
}