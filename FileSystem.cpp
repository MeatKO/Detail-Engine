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
		LoadDir("detail/models/snowgrass/");

		for (File& file : files)
		{
			file.PrintInfo();
		}
	}

	void FileSystem::LoadFile(std::string path) // loads a single file into the file list
	{
		std::string newPath = GetSanitizedPath(path);
		std::string fullFileName = GetPathFullFilename(path);
		std::string fileName = GetPathFileName(path);
		std::string fileType = GetPathFileType(path);
		FileOpenMode mode = TypeToMode(fileType);

		std::ifstream FILE;

		if (mode == OPEN_UNSUPPORTED)
		{
			// error unsupported file type
			pSendMessage(Message(MSG_LOG, std::string("FileSystem Error"), std::string("File Type '" + fileType + "' is not supported.")));

			return;
		}

		if (mode == OPEN_BINARY)
		{
			FILE = std::ifstream(newPath, std::ios::binary);
		}
		else if(mode == OPEN_TEXT)
		{
			FILE = std::ifstream(newPath);
		}

		if (!FILE.fail())
		{
			files.push_back(File(UnixTimestamp()));
			files.back().Fill(FILE);
			files.back().SetName(fileName);
			files.back().SetType(fileType);
			files.back().SetMode(mode);

			FILE.close();
		}
		else
		{
			// error file doesnt exist
			pSendMessage(Message(MSG_LOG, std::string("FileSystem Error"), std::string("File '" + fullFileName + "' cannot be accessed.")));
		}
	}

	void FileSystem::LoadDir(std::string path) // loads all the file from a specific directory in the file list
	{
		std::string newPath = GetSanitizedPath(path);

		std::vector<std::string> fileNames;

		fileNames = DirGetAllFileNames(newPath);

		for (std::string& name : fileNames)
		{
			std::cout << name << std::endl;
			LoadFile(name);
		}
	}

	std::vector<std::string> FileSystem::DirGetAllFileNames(std::string path)
	{
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

	// Works only with ASCII characters < 127
	// No ? * | < > : "
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
					// if the path isnt something like "           " 
					// must be idiot proof since i will be working with it
					if (!StringContainsOnly(filter, ' '))
					{
						out.push_back(filter);
					}

					filter = "";
					reset = false;
				}

				// the first char in a dir cannot be an empty space
				if (!(filter.size() == 0 && path[i] == ' '))
				{
					if (path[i] > 0 && path[i] < 127)
					{
						if (!(path[i] == '?' || path[i] == '*' || path[i] == '|' || path[i] == '<' || path[i] == '>' || path[i] == ':' || path[i] == '"'))
						{
							filter += path[i];
						}
					}
				}
			}
		}

		if (!StringContainsOnly(filter, ' '))
		{
			out.push_back(filter);
		}

		return out;

	}

	bool FileSystem::StringContainsOnly(std::string input, char character)
	{
		for (int i = 0; i < input.size(); ++i)
		{
			if (input[i] != character)
				return false;
		}

		return true;
	}

	void File::SetName(std::string Name) { name = Name; }
	std::string File::GetName() { return name; }
	void File::SetType(std::string Type) { type = Type; }
	std::string File::GetType() { return type; }
	void File::SetMode(FileOpenMode Mode) { mode = Mode; }
	FileOpenMode File::GetMode() { return mode; }
	void File::Fill(std::ifstream& file) { contents.clear(); contents << file.rdbuf(); }
	void File::Append(std::ifstream& file) { contents << file.rdbuf(); }
	void File::Erase() { contents.clear(); }
	std::stringstream& File::Data() { return contents; }
	long int File::GetCreationTime() { return creationTime; }
	int File::GetSize() { return contents.rdbuf()->str().size(); }
	void File::Dump()
	{
		std::string word;
		while (getline(contents, word))
		{
			std::cout << word;
		}
	}
	void File::PrintInfo()
	{
		std::cout << "Name : '" << GetName() << "' Type : '" << GetType() << "' Size : " << GetSize() << " (Bytes), Binary : " << GetMode() << "\n";
		std::cout << "Created at : " << GetCreationTime() << "\n";
	}
}
