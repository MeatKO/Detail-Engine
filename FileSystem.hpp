#pragma once

#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include <filesystem>
#include <bitset>
#include <ctime>
#include "ECS.hpp"

namespace detailEngine
{
	long int UnixTimestamp();

	ComponentAssetType StringToCAT(std::string type);

	class Asset;
	class AssetManager;

	// only stores a stringstream containing the file, will be operated upon by the FileSystem class
	class File 
	{
	public:
		File(long int currentTime) : creationTime(currentTime) {}

		void SetName(std::string Name)
		{
			name = Name;
		}

		void Fill(std::ifstream& file)
		{
			contents.clear();
			contents << file.rdbuf();
		}

		void Append(std::ifstream& file)
		{
			contents << file.rdbuf();
		}

		void Erase()
		{
			contents.clear();
		}

		std::stringstream& Data()
		{
			return contents;
		}

	private:
		std::string name = "unnamed";
		std::stringstream contents; // makes a lot of sense to be private i know...
		long int creationTime = 0;
	};

	class FileSystem : public Publisher, public Subscriber
	{
	public:
		FileSystem() {}

		void Update(EntityController* entityController, AssetManager* assetManager);
		void RequestAsset(Asset asset);
		std::vector<Asset> CollectAssets();

	private:
		void LoadFile(std::string path);
		void LoadDir(std::string path); // Loads all the files from a directory
		std::vector<std::string> DirGetAllFileNamesAbsolute(std::string path);
		std::string GetPathFileName(std::string path); // returns the file name ONLY
		std::string GetPathFileType(std::string path); // returns the file type ONLY
		std::string GetPathFullFilename(std::string path); // returns the file name and its type
		std::string GetSanitizedPath(std::string path); // Gets SanitizePath() output and assembles it with / in between the words;
		std::vector<std::string> SanitizePath(std::string path); // splits the path into words

		// All of the code below is garbage
		void ExecuteAllRequests();
		void ExecuteRequest(Asset& asset);
		void ExecuteMessage(Message message);
		void DeliverAsset(Asset asset);

		std::vector<File> files;


		// All of the code below belongs in a cemetery 
		std::mutex requestMutex;
		std::mutex deliverMutex;
		std::mutex fileioMutex;
		void SwapRequestBuffers();
		std::vector<Asset> deliveredAssets;
		std::vector<Asset> requestedAssets[2];
		bool requestBuffer = true;
	};
}