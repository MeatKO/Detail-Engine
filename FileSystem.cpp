﻿//#include "FileSystem.hpp"
//#include "AssetManager.hpp"
//#include "glModel.hpp"
//
//namespace detailEngine
//{
//	long int UnixTimestamp()
//	{
//		time_t t = std::time(0);
//		long int now = static_cast<long int> (t);
//		return now;
//	}
//
//	bool PathExists(std::string path) // requires a sanitized path
//	{
//		struct stat info;
//
//		int statRC = stat(path.c_str(), &info);
//		if (statRC != 0)
//		{
//			if (errno == ENOENT) { return 0; } // something along the path does not exist
//			if (errno == ENOTDIR) { return 0; } // something in path prefix is not a dir
//			return false;
//		}
//
//		return (info.st_mode & S_IFDIR) ? true : false;
//	}
//
//	std::string ReplaceSpaces(std::string input, char replaceWith)
//	{
//		for (int i = 0; i < input.size(); ++i)
//		{
//			if (input[i] == ' ')
//			{
//				input[i] = replaceWith;
//			}
//		}
//
//		return input;
//	}
//
//	FileSystem::FileSystem()
//	{
//		directories.push_back(Directory("root"));
//
//		AddDirectory("newDir", "");
//	}
//
//	int FileSystem::GetDirIndex(std::string DirName)
//	{
//		for (int i = 0; i < directories.size(); ++i)
//		{
//			if (directories[i].name == DirName)
//				return i;
//		}
//
//		return -1;
//	}
//
//	int FileSystem::FileTreeValidatePath(std::string path)
//	{
//		std::vector<std::string> directoryNames = SanitizePath(path);
//		std::string validPath = "";
//
//		int ret = FileTreeValidatePathRec(0, 0, directoryNames, validPath);
//
//		if (ret == -1)
//		{
//			pSendMessage(Message(MSG_LOG, std::string("FileSystem Error"), std::string("The Path 'root" + path + "' is invalid'")));
//		}
//
//		return ret;
//	}
//
//	int FileSystem::FileTreeValidatePathRec(int parentID, int depth, std::vector<std::string> directoryNames, std::string validPath)
//	{
//		std::string currentDirName = directories[parentID].name;
//		validPath += currentDirName + '/';
//
//		// if the whole list is looped through and everything is in check
//		if (directoryNames.size() == depth)
//		{
//			return parentID; // give the id of the deepest path
//		}
//
//		std::string searchedDirName = directoryNames[depth]; // depth starts from 0 so it can be used as an index
//
//		// check if the current parent dir's list containst a dir with the name value of the searchedDirName
//		for (int i = 0; i < directories[parentID].directoryIDs.size(); ++i)
//		{
//			// get the id from the parent dir's list, use the id as an index in the directories list, check its name
//			if (directories[directories[parentID].directoryIDs[i]].name == searchedDirName)
//			{
//				return FileTreeValidatePathRec(directories[parentID].directoryIDs[i], depth + 1, directoryNames, validPath);
//			}
//		}
//
//		// if nothing is found, print error message containing the latest search for directoryName and the validPath
//		pSendMessage(Message(MSG_LOG, std::string("FileSystem Error"), std::string("The Path '" + validPath + "' doesn't contain Directory '" + searchedDirName + "'")));
//
//		return -1;
//	}
//
//	int FileSystem::AddDirectory(std::string DirName, std::string path)
//	{
//		std::lock_guard<std::mutex> mut(fileioMutex);
//
//		int ret = FileTreeValidatePath(path);
//
//		if (ret != -1)
//		{
//			int newDirID = directories.size(); // it will get incremented by 1 and this number will be the new dir's ID
//			directories.push_back(Directory(DirName));
//			directories.back().parentDirID = ret; // set the new directory's parent
//			directories[ret].directoryIDs.push_back(newDirID); // add the new directory to the parent's dirID list
//
//			return newDirID;
//		}
//
//		return -1;
//	}
//
//	void FileSystem::AddFile(int FileID, std::string filePath)
//	{
//		std::string dirPath = GetPathNoFile(filePath);
//		std::string fileName = GetPathFileName(filePath);
//	}
//
//	void FileSystem::Update(EntityController* entityController, AssetManager* assetManager)
//	{
//		//Debug();
//		//ExecuteAllRequests();
//
//		// moved the loading stuff to the execute message function since its non blocking and i dont want to make 200 buffers for temporary strings and what not
//		// also this function kind of has no purpose anymore
//		// i like how the filesystem has its own thread
//	}
//
//	void FileSystem::ExecuteMessage(Message message)
//	{
//		if (message.GetTopic() == MSG_LOAD_DIR)
//		{
//			
//			std::string path = std::any_cast<std::string>(message.GetEvent());
//
//			std::string newPath = GetPathNoFile(path);
//
//			LoadDir(newPath); // if this fails idk lol
//		}
//	}
//
//	bool FileSystem::IsLoaded(std::string filename)
//	{
//		std::lock_guard<std::mutex> mut(fileioMutex);
//
//		for (File& file : files)
//		{
//			if (file.GetName() == filename)
//				return true;
//		}
//
//		return false;
//	}
//
//	void FileSystem::Debug()
//	{
//		//LoadDir("detail/models/snowgrass/");
//		
//		for (File& file : files)
//		{
//			file.PrintInfo();
//		}
//
//		//for (File& file : files)
//		//{
//		//	file.Erase();
//		//}
//	}
//
//	// The asset manager kept the file types without a .
//	File* FileSystem::GetFile(std::string fileName, std::string fileType)
//	{
//		std::lock_guard<std::mutex> mut(fileioMutex);
//
//		for (int i = 0; i < files.size(); ++i)
//		{
//			if ((files[i].GetName() == fileName) && (files[i].GetType() == fileType))
//			{
//				return &files[i];
//			}
//		}
//
//		return nullptr;
//	}
//
//	File* FileSystem::GetFile(std::string fileNameType)
//	{
//		std::string fileName, fileType;
//
//		SplitFileNameType(fileNameType, fileName, fileType);
//
//		std::lock_guard<std::mutex> mut(fileioMutex);
//
//		for (int i = 0; i < files.size(); ++i)
//		{
//			if ((files[i].GetName() == fileName) && (files[i].GetType() == fileType))
//			{
//				return &files[i];
//			}
//		}
//
//		return nullptr;
//	}
//
//	// Nothing special just loads all other files in the same directory
//	void FileSystem::LoadOBJModel(std::string path)
//	{
//		std::string newPath = GetPathNoFile(path);
//
//		LoadDir(newPath);
//	}
//
//	void FileSystem::LoadTextureFile(std::string path)
//	{
//		std::string newPath = GetSanitizedPath(path);
//		std::string fileName = GetPathFileName(path);
//		std::string fileType = GetPathFileType(path);
//
//		bool fileExists = false;
//
//		for (File& file : files)
//		{
//			if ((file.GetName() == fileName) && (file.GetType() == fileType))
//				fileExists = true;
//		}
//
//		// cant use the function because of deadlocks...
//		if (fileExists)
//		{
//			pSendMessage(Message(MSG_LOG, std::string("FileSystem Warning"), std::string("Trying to reload file '" + fileName + "." + fileType + "'.")));
//			return;
//		}
//
//		int width, height;
//		unsigned char* data = TextureImage(newPath, width, height);
//		std::string dataString(reinterpret_cast<char*>(data)); // should work ??
//
//		files.push_back(File(UnixTimestamp()));
//		files.back().Fill(dataString);
//		files.back().SetName(fileName);
//		files.back().SetType(fileType);
//		//files.back().SetMode(OPEN_BINARY);
//		//
//		//files.back().aux[0] = width;
//		//files.back().aux[1] = height;
//		//files.back().aux[2] = (intptr_t)data;
//
//		//DeleteTextureImage(data); // not needed anymore
//
//	}
//
//	bool FileSystem::FileExists(std::string fileName, std::string fileType)
//	{
//		std::lock_guard<std::mutex> mut(fileioMutex);
//
//		for (File& file : files)
//		{
//			if ((file.GetName() == fileName) && (file.GetType() == fileType))
//				return true;
//		}
//
//		return false;
//	}
//
//	void FileSystem::LoadFile(std::string path) // loads a single file into the file list
//	{
//		std::lock_guard<std::mutex> mut(fileioMutex);
//
//		std::string newPath = GetSanitizedPath(path);
//		std::string fullFileName = GetPathFullFilename(path);
//		std::string fileName = GetPathFileName(path);
//		std::string fileType = GetPathFileType(path);
//		//FileOpenMode mode = TypeToMode(fileType);
//
//		if (fileType == "png" || fileType == "tga" || fileType == "jpg" || fileType == "jpeg")
//		{
//			LoadTextureFile(path);
//			return;
//		}
//
//		bool fileExists = false;
//
//		for (File& file : files)
//		{
//			if ((file.GetName() == fileName) && (file.GetType() == fileType))
//				fileExists = true;
//		}
//
//		// cant use the function because of deadlocks...
//		// EDIT : wtf did I write, what is that comment ??
//		if (fileExists)
//		{
//			pSendMessage(Message(MSG_LOG, std::string("FileSystem Warning"), std::string("Trying to reload file '" + fileName + "." + fileType + "'.")));
//			return;
//		}
//
//		std::ifstream FILE;
//
//		//if (mode == OPEN_UNSUPPORTED)
//		//{
//		//	// error unsupported file type
//		//	pSendMessage(Message(MSG_LOG, std::string("FileSystem Error"), std::string("File Type '" + fileType + "' is not supported.")));
//		//
//		//	return;
//		//}
//
//		//if (mode == OPEN_BINARY)
//		//{
//		//	FILE = std::ifstream(newPath, std::ios::binary);
//		//}
//		//else if(mode == OPEN_TEXT)
//		//{
//		//	FILE = std::ifstream(newPath);
//		//
//	   
//		//if (!FILE.fail())
//		//{
//		//	files.push_back(File(UnixTimestamp()));
//		//	files.back().Fill(FILE);
//		//	files.back().SetName(fileName);
//		//	files.back().SetType(fileType);
//		//	files.back().SetMode(mode);
//	   
//		//	FILE.close();
//		//}
//		//else
//		//{
//		//	// error file doesnt exist
//		///	pSendMessage(Message(MSG_LOG, std::string("FileSystem Error"), std::string("File '" + fullFileName + "' cannot be accessed.")));
//		//}
//	}
//
//	void FileSystem::LoadDir(std::string path) // loads all the file from a specific directory in the file list
//	{
//		std::string newPath = GetSanitizedPath(path);
//
//		std::vector<std::string> fileNames;
//
//		fileNames = DirGetAllFileNames(newPath);
//
//		for (std::string& name : fileNames)
//		{
//			//::cout << name << std::endl;
//			LoadFile(name);
//		}
//	}
//
//	std::vector<std::string> FileSystem::DirGetAllFileNames(std::string path)
//	{
//		std::vector<std::string> fileNames;
//
//		std::string newPath = GetSanitizedPath(path);
//
//		if (PathExists(newPath))
//		{
//			for (auto& entry : fs::directory_iterator(newPath)) // crashes on invalid path or smth
//				fileNames.push_back(entry.path().string());
//		}
//
//		return fileNames;
//	}
//
//	std::string GetPathFileName(std::string path)
//	{
//		std::string fileName = GetPathFullFilename(path);
//		std::string out;
//		for (int i = 0; i < fileName.size(); ++i)
//		{
//			if (fileName[i] == '.')
//			{
//				if (i + 1 < fileName.size())
//				{
//					if (fileName[i + 1] != '.') // support for /../../
//						break;
//				}
//			}
//			out += fileName[i];
//		}
//		return out;
//	}
//
//	std::string GetPathFileType(std::string path)
//	{
//		std::string fileName = GetPathFullFilename(path);
//		std::string out;
//		bool begin = false;
//
//		for (int i = 0; i < fileName.size(); ++i)
//		{
//			if (fileName[i] == '.')
//			{
//				begin = true;
//				continue; // dont actually count the dot as part of the type
//			}
//
//			if (begin)
//			{
//				out += fileName[i];
//			}
//		}
//		return out;
//	}
//
//	std::string GetPathFullFilename(std::string path)
//	{
//		return SanitizePath(path).back();
//	}
//
//	std::string GetSanitizedPath(std::string path)
//	{
//		std::vector<std::string> words = SanitizePath(path);
//
//		std::string out = "";
//
//		for (int i = 0; i < words.size() - 1; ++i)
//		{
//			if (words[i] == "" || words[i] == " ")
//				continue;
//
//			out += words[i] + "/";
//		}
//
//		out += words.back();
//
//		return out;
//	}
//
//	// Works only with ASCII characters < 127
//	// No ? * | < > : "
//	std::vector<std::string> SanitizePath(std::string path)
//	{
//		std::vector<std::string> out;
//		std::string filter = "";
//		bool reset = false;
//
//		for (int i = 0; i < path.size(); ++i)
//		{
//			if (path[i] == '\\' || path[i] == '/')
//			{
//				reset = true;
//			}
//			else
//			{
//				if (reset)
//				{
//					// if the path isnt something like "           " 
//					// must be idiot proof since i will be working with it
//					if (!StringContainsOnly(filter, ' '))
//					{
//						filter = ReplaceSpaces(filter, '_');
//						out.push_back(filter);
//					}
//
//					filter = "";
//					reset = false;
//				}
//
//				// the first char in a dir cannot be an empty space
//				if (!(filter.size() == 0 && path[i] == ' '))
//				{
//					if (path[i] > 0 && path[i] < 127)
//					{
//						if (!(path[i] == '?' || path[i] == '*' || path[i] == '|' || path[i] == '<' || path[i] == '>' || path[i] == ':' || path[i] == '"'))
//						{
//							filter += path[i];
//						}
//					}
//				}
//			}
//		}
//
//		if (!StringContainsOnly(filter, ' '))
//		{
//			filter = ReplaceSpaces(filter, '_');
//			out.push_back(filter);
//		}
//
//		return out;
//
//	}
//
//	bool StringContainsOnly(std::string input, char character)
//	{
//		for (int i = 0; i < input.size(); ++i)
//		{
//			if (input[i] != character)
//				return false;
//		}
//
//		return true;
//	}
//
//	// expects a path that contains a file name and type !
//	std::string GetPathNoFile(std::string path)
//	{
//		std::vector<std::string> words = SanitizePath(path);
//
//		std::string out = "";
//
//		for (int i = 0; i < words.size() - 1; ++i)
//		{
//			if (words[i] == "" || words[i] == " ")
//				continue;
//
//			out += words[i] + "\\";
//		}
//
//		// if the last element doesnt contain a . then push it in the vector
//		// if it contains a . its most likely a file so dont push it
//		if (!words.back().find('.'))
//		{
//			out += words.back();
//		}
//
//		return out;
//	}
//
//	void SplitFileNameType(std::string fileNamePath, std::string& file, std::string& type)
//	{
//		bool dot = false;
//
//		for (int i = 0; i < fileNamePath.size(); ++i)
//		{
//			if (dot)
//			{
//				type += fileNamePath[i];
//			}
//			else
//			{
//				if (fileNamePath[i] == '.')
//				{
//					dot = true;
//				}
//				else
//				{
//					file += fileNamePath[i];
//				}
//			}
//		}
//	}
//
//	void File::SetName(std::string Name) { name = Name; }
//	std::string File::GetName() { return name; }
//	void File::SetType(std::string Type) { type = Type; }
//	std::string File::GetType() { return type; }
//	//void File::SetMode(FileOpenMode Mode) { mode = Mode; }
//	//FileOpenMode File::GetMode() { return mode; }
//	void File::Fill(std::ifstream& file) { data.clear(); data << file.rdbuf(); }
//	void File::Fill(std::string& string) { data.clear(); data << string; }
//	void File::Append(std::ifstream& file) { data << file.rdbuf(); }
//	void File::Erase() { data.clear(); name = "deleted"; type = "deleted"; }
//	std::stringstream& File::Data() { return data; }
//	long int File::GetCreationTime() { return creationTime; }
//	int File::GetSize() { return data.rdbuf()->str().size(); }
//	void File::Dump()
//	{
//		std::string word;
//		while (getline(data, word))
//		{
//			std::cout << word;
//		}
//	}
//	void File::PrintInfo()
//	{
//		std::cout << "Name : '" << GetName() << "' Type : '" << GetType() << "' Size : " << GetSize() << "\n";
//		std::cout << "Created at : " << GetCreationTime() << "\n";
//	}
//}
