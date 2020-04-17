#include "AssetManager.hpp"
#include "FileSystem.hpp"

namespace detailEngine
{
	Asset::Asset(std::string Name, std::string Location, std::string FileType)
	{
		name = Name;
		location = Location;
		fileType = FileType;
	}


	AssetManager::AssetManager() {}
	void AssetManager::RequestAsset(Asset asset)
	{
		if (!AssetExists(asset.name))
		{
			std::lock_guard<std::mutex> mut(requestMutex);
			requestedAssets[!requestBuffer].push_back(asset);
		}
		else
		{
			// err tried to request duplicate asset
		}
	}
	void AssetManager::Update(EntityController* entityController, FileSystem* fileSystem)
	{
		ExecuteRequests(fileSystem);

		std::vector<Asset> receivedAssets = fileSystem->CollectAssets();
		for (Asset& asset : receivedAssets)
		{
			if (!AssetExists(asset.name))
			{
				AddAsset(asset);
			}
		}
	}
	bool AssetManager::AssetExists(std::string assetName)
	{
		std::lock_guard<std::mutex> mut(assetMutex);
		for (Asset& asset : assetList)
		{
			if (asset.name == assetName)
				return true;
		}
		return false;
	}
	Asset& AssetManager::RefAsset(std::string assetName)
	{
		std::lock_guard<std::mutex> mut(assetMutex);
		for (Asset& asset : assetList)
		{
			if (asset.name == assetName)
				return asset;
		}
		// print error not found
	}
	Asset& AssetManager::RefAsset(int assetID)
	{
		if (assetID >= 0 && assetID < assetList.size())
		{
			std::lock_guard<std::mutex> mut(assetMutex);
			return assetList[assetID];
		}
		// print error invalid index
	}
	std::vector<Asset> AssetManager::GetAllAssets()
	{
		return assetList;
	}
	int AssetManager::GetAssetID(std::string assetName)
	{
		std::lock_guard<std::mutex> mut(assetMutex);
		for (int i = 0; i < assetList.size(); i++)
		{
			if (assetName == assetList[i].name)
			{
				return i;
			}
		}
		return -1;
	}
	void AssetManager::ExecuteRequests(FileSystem* fileSystem)
	{
		SwapRequestBuffers();
		for (Asset& asset : requestedAssets[requestBuffer])
		{
			fileSystem->RequestAsset(asset);
			// offload to filesystem
		}
		requestedAssets[requestBuffer].clear();
		
	}
	void AssetManager::ExecuteMessage(Message message)
	{
		if (message.GetTopic() == MSG_ASSET)
		{
			if (message.GetValue().type() == typeid(Asset))
			{
				RequestAsset(std::any_cast<Asset>(message.GetValue()));
			}
			else
			{
				// error
			}
		}
	}
	void AssetManager::AddAsset(Asset asset)
	{
		if (!AssetExists(asset.name))
		{
			std::lock_guard<std::mutex> mut(assetMutex);
			assetList.push_back(asset);
		}
		else
		{
			// err tried to add duplicate asset
		}
	}
	void AssetManager::SwapRequestBuffers()
	{
		requestBuffer = !requestBuffer;
	}
}