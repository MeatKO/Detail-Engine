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

		std::mutex requestMutex;
		std::mutex deliverMutex;
		void SwapRequestBuffers();
		std::vector<Asset> deliveredAssets;
		std::vector<Asset> requestedAssets[2];
		bool requestBuffer = true;
	};
}