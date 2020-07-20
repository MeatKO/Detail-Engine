#include "FileSystem.hpp"
#include "AssetManager.hpp"
#include "glModel.hpp"

namespace detailEngine
{
	long int UnixTimestamp()
	{
		time_t t = std::time(0);
		long int now = static_cast<long int> (t);
		return now;
	}

	bool PathExists(std::string path) // requires a sanitized path
	{
		struct stat info;

		int statRC = stat(path.c_str(), &info);
		if (statRC != 0)
		{
			if (errno == ENOENT) { return 0; } // something along the path does not exist
			if (errno == ENOTDIR) { return 0; } // something in path prefix is not a dir
			return false;
		}

		return (info.st_mode & S_IFDIR) ? true : false;
	}

	FileOpenMode TypeToMode(std::string fileType)
	{
		if (fileType == ".obj" || fileType == ".mtl")
			return OPEN_TEXT;

		if (fileType == ".png" || fileType == ".tga")
			return OPEN_BINARY;

		return OPEN_UNSUPPORTED;
	}

	FileSystem::FileSystem()
	{
		Debug();
	}

	void FileSystem::Update(EntityController* entityController, AssetManager* assetManager)
	{
		//ExecuteAllRequests();
	}

	bool FileSystem::IsLoaded(std::string filename)
	{
		std::lock_guard<std::mutex> mut(fileioMutex);

		for (File& file : files)
		{
			if (file.GetName() == filename)
				return true;
		}

		return false;
	}

	void FileSystem::Debug()
	{
		//LoadFile("detail/models/snowgrass/snowgrass.obj");
		//if (files.size() > 0)
		//{
		//	files.back().Dump();
		//}

		//DirGetAllFileNames("detail/models/de_inferno");
		LoadDir("detail/models/snowgrass/");

		for (File& file : files)
		{
			std::cout << "Name : " << file.GetName() << " Type : " << file.GetType() << " Size : " << file.GetSize() << " Bytes \n";
			//file.Dump();
		}
	}

	void FileSystem::LoadFile(std::string path) // loads a single file into the file list
	{
		// Lock the file list - try not to make deadlocks ok ? bye
		// push back a file to the vector
		// open the requested file and then access fileList.back() and add the opened file content to the <File> container
		//std::lock_guard<std::mutex> mut(fileioMutex);

		std::string newPath = GetSanitizedPath(path);
		std::string fullFileName = GetPathFullFilename(path);
		std::string fileName = GetPathFileName(path);
		std::string fileType = GetPathFileType(path);

		std::ifstream FILE;

		if (TypeToMode(fileType) == OPEN_BINARY)
		{
			FILE = std::ifstream(newPath, std::ios::binary);
		}
		else
		{
			FILE = std::ifstream(newPath);
		}

		if (!FILE.fail())
		{
			files.push_back(File(UnixTimestamp()));
			//files.back().Data() << FILE.rdbuf();
			files.back().Fill(FILE);
			files.back().SetName(fileName);
			files.back().SetType(fileType);

			FILE.close();
		}
		else
		{
			pSendMessage(Message(MSG_LOG, std::string("FileSystem Error"), std::string("File '" + fullFileName + "' cannot be accessed."))); // error file doesnt exist
		}
	}

	void FileSystem::LoadDir(std::string path) // loads all the file from a specific directory in the file list
	{
		//std::lock_guard<std::mutex> mut(fileioMutex);

		std::string newPath = GetSanitizedPath(path);

		std::vector<std::string> fileNames;

		fileNames = DirGetAllFileNames(newPath);

		for (std::string& name : fileNames)
		{
			std::cout << name << std::endl;
			LoadFile(name);
		}

		for (File& file : files)
		{
			std::cout << file.GetName() << file.GetType() << std::endl;
		}
	}

	std::vector<std::string> FileSystem::DirGetAllFileNames(std::string path)
	{
		//std::lock_guard<std::mutex> mut(fileioMutex);

		std::vector<std::string> fileNames;

		std::string newPath = GetSanitizedPath(path);

		if (PathExists(newPath))
		{
			for (auto& entry : fs::directory_iterator(newPath)) // crashes on invalid path or smth
				fileNames.push_back(entry.path().string());
		}

		return fileNames;
	}

	std::string FileSystem::GetPathFileName(std::string path)
	{
		std::string fileName = GetPathFullFilename(path);
		std::string out;
		for (int i = 0; i < fileName.size(); ++i)
		{
			if (fileName[i] == '.')
			{
				if (i + 1 < fileName.size())
				{
					if (fileName[i + 1] != '.') // support for /../../
						break;
				}
			}
			out += fileName[i];
		}
		return out;
	}

	std::string FileSystem::GetPathFileType(std::string path)
	{
		std::string fileName = GetPathFullFilename(path);
		std::string out;
		bool begin = false;

		for (int i = 0; i < fileName.size(); ++i)
		{
			if (fileName[i] == '.')
				begin = true;

			if (begin)
			{
				out += fileName[i];
			}
		}
		return out;
	}

	std::string FileSystem::GetPathFullFilename(std::string path)
	{
		return SanitizePath(path).back();
	}

	std::string FileSystem::GetSanitizedPath(std::string path)
	{
		std::vector<std::string> words = SanitizePath(path);

		std::string out = "";

		for (int i = 0; i < words.size() - 1; ++i)
		{
			if (words[i] == "" || words[i] == " ")
				continue;

			out += words[i] + "\\";
		}

		out += words.back();

		return out;
	}

	std::vector<std::string> FileSystem::SanitizePath(std::string path)
	{
		std::vector<std::string> out;
		std::string filter = "";
		bool reset = false;

		for (int i = 0; i < path.size(); ++i)
		{
			if (path[i] == '\\' || path[i] == '/')
			{
				reset = true;
			}
			else
			{
				if (reset)
				{
					out.push_back(filter);
					filter = "";
					reset = false;
				}
				filter += path[i];
			}
		}
		out.push_back(filter);

		return out;

	}
}
