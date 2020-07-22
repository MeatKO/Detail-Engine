#pragma once

// This needs a complete rewrite and its 1:30 AM 

#include "ECS.hpp"
#include "glModel.hpp"

namespace detailEngine
{
	class FileSystem;
	
	class Asset
	{
	public:
		Asset(std::string Name, std::string FileName, std::string FileType);

		std::string name;
		std::string fileName;
		std::string fileType;
		ComponentAssetType assetType;
		bool deleted = false;
		bool processed = false;
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
		void ExecuteMessage(Message message);
		std::mutex assetMutex;
		void AddAsset(Asset asset);
		std::vector<Asset> assetList;

		void ProcessAsset(Asset& asset, FileSystem* fileSystem);
		void ProcessObjAsset(Asset& asset, FileSystem* fileSystem);
		void ProcessObjTextures(Asset& asset, FileSystem* fileSystem);

		Asset defaultAsset = Asset("DEFAULT", "DEFAULT", "DEFAULT");
	};
}