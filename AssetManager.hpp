#pragma once

#include "ECS.hpp"
#include "glModel.hpp"

namespace detailEngine
{
	class FileSystem;
	
	class Asset
	{
	public:
		Asset(std::string Name, std::string Location, std::string FileType);

		std::string name;
		std::string location;
		std::string fileType;
		ComponentAssetType assetType;
		bool deleted = false;
		std::any data;
	};

	class AssetManager : public Publisher, public Subscriber
	{
	public:
		AssetManager();
		void RequestAsset(Asset asset);
		void Update(EntityController* entityController, FileSystem* fileSystem);
		bool AssetExists(std::string assetName);
		Asset& RefAsset(std::string assetName);
		Asset& RefAsset(int assetID);
		Asset GetAsset(std::string assetName);
		Asset GetAsset(int assetID);
		std::vector<Asset> GetAllAssets();
		int GetAssetID(std::string assetName);
		void UpdateAsset(int AssetID, Asset newAsset);

	private:
		void ExecuteRequests(FileSystem* fileSystem);
		void ExecuteMessage(Message message);
		std::mutex assetMutex;
		std::mutex requestMutex;
		void AddAsset(Asset asset);
		void SwapRequestBuffers();
		std::vector<Asset> assetList;
		std::vector<Asset> requestedAssets[2];
		bool requestBuffer = true;

		Asset defaultAsset = Asset("DEFAULT", "DEFAULT", "DEFAULT");
	};
}