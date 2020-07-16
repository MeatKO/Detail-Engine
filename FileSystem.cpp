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
	ComponentAssetType StringToCAT(std::string type)
	{
		if (type == "obj")
			return CAT_MODEL;

		if (type == "aabb")
			return CAT_AABB;

		return CAT_DEFAULT;
	}
	
	void FileSystem::Update(EntityController* entityController, AssetManager* assetManager)
	{
		ExecuteAllRequests();
	}

	void FileSystem::RequestAsset(Asset asset)
	{
		std::lock_guard<std::mutex> mut(requestMutex);
		requestedAssets[!requestBuffer].push_back(asset);
	}

	std::vector<Asset> FileSystem::CollectAssets()
	{
		std::lock_guard<std::mutex> mut(deliverMutex);

		std::vector<Asset> vectorCopy = deliveredAssets;
		deliveredAssets.clear();

		return vectorCopy;
	}

	void FileSystem::LoadFile(std::string path) // loads a single file into the file list
	{
		// Lock the file list - try not to make deadlocks ok ? bye
		// push back a file to the vector
		// open the requested file and then access fileList.back() and add the opened file content to the <File> container
		std::lock_guard<std::mutex> mut(fileioMutex);

		files.push_back(File(UnixTimestamp()));
	}

	void FileSystem::LoadDir(std::string path) // loads all the file from a specific directory in the file list
	{
		std::lock_guard<std::mutex> mut(fileioMutex);

	}

	std::vector<std::string> FileSystem::DirGetAllFileNamesAbsolute(std::string path)
	{
		std::lock_guard<std::mutex> mut(fileioMutex);
		std::vector<std::string> fileNames;



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

		for (std::string& str : words)
		{
			out += str + "/";
		}

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

	void FileSystem::ExecuteAllRequests()
	{
		SwapRequestBuffers();
		for (Asset& asset : requestedAssets[requestBuffer])
		{
			ExecuteRequest(asset);
			DeliverAsset(asset);
		}
		requestedAssets[requestBuffer].clear();
	}

	void FileSystem::ExecuteRequest(Asset& asset)
	{
		ComponentAssetType Type = StringToCAT(asset.fileType);

		asset.assetType = Type;

		if (Type == CAT_MODEL)
		{
			Model newMdl(asset.name);
			asset.data = newMdl;
		}
	}

	void FileSystem::ExecuteMessage(Message message)
	{
	}

	void FileSystem::DeliverAsset(Asset asset)
	{
		std::lock_guard<std::mutex> mut(deliverMutex);
		deliveredAssets.push_back(asset);
	}

	void FileSystem::SwapRequestBuffers()
	{
		std::lock_guard<std::mutex> mut(requestMutex);
		requestBuffer = !requestBuffer;
	}

}
