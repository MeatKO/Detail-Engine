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
		File() {}

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
		std::stringstream contents; // makes a lot of sense to be private i know...
	};

	class FileSystem : public Publisher, public Subscriber
	{
	public:
		FileSystem() {}

		void Update(EntityController* entityController, AssetManager* assetManager);
		void RequestAsset(Asset asset);
		std::vector<Asset> CollectAssets();

	private:
		void ExecuteAllRequests();
		void ExecuteRequest(Asset& asset);
		void ExecuteMessage(Message message);
		void DeliverAsset(Asset asset);

		std::vector<File> files;

		std::mutex requestMutex;
		std::mutex deliverMutex;
		void SwapRequestBuffers();
		std::vector<Asset> deliveredAssets;
		std::vector<Asset> requestedAssets[2];
		bool requestBuffer = true;
	};
}