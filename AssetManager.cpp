#include "AssetManager.hpp"
#include "FileSystem.hpp"

namespace detailEngine
{
	Order::Order(std::string Name, std::string PackName, std::string Type)
	{
		name = Name;
		packName = PackName;
		type = Type;
	}

	template<typename T>
	Asset::Asset(ComponentAssetType Type, std::string Name, T Value)
	{
		type = Type;
		data = Value;
		assetName = Name;
	}
	std::string Asset::GetName() { return assetName; }
	ComponentAssetType Asset::GetType() { return type; }
	std::any Asset::GetValue() { return data; }
	void Asset::SetType(ComponentAssetType Type) { type = Type; }
	void Asset::SetName(std::string Name){ assetName = Name; }
	bool Asset::Exists() { return exists; }
	void Asset::Delete()
	{
		assetName = "DELETED";
		exists = false;
		data.reset();
	}

	// Asset Manager
	AssetManager::AssetManager()
	{
		defaultAsset.Delete();
	}
	void AssetManager::ExecuteMessage(Message message)
	{
		if (message.GetTopic() == MSG_ASSET_ORDER)
		{
			
			if (message.GetValue().type() == typeid(Order))
			{
				//std::lock_guard<std::mutex> mut(orderLock);
				Order receivedOrder = std::any_cast<Order>(message.GetValue());
				PlaceOrder(receivedOrder);
				return;
			}
			pSendMessage(Message(MSG_LOG, std::string("Asset Manager Error"), std::string("An Asset Order received on the Bus was in the wrong type format.")));
		}
	}
	bool AssetManager::AssetExists(std::string assetName)
	{
		std::vector<Asset> assets = GetAllAssets();
		for (Asset asset : assets)
		{
			if (asset.GetName() == assetName)
				return true;
		}
		return false;
	}
	int AssetManager::GetAssetID(std::string assetName)
	{
		for (int i = 0; i < assetList.size(); i++)
		{
			if (assetName == assetList[i].GetName())
				return i;
		}
		return -1;
	}
	Asset AssetManager::GetAsset(unsigned int id)
	{
		std::lock_guard<std::mutex> mut(assetLock);
		if (id < assetList.size())
			return assetList[id];

		return defaultAsset;
	}
	Asset& AssetManager::RefAsset(unsigned int id)
	{
		std::lock_guard<std::mutex> mut(assetLock);
		if (id < assetList.size())
			return assetList[id];

		return defaultAsset;
	}
	std::vector<Asset> AssetManager::GetAllAssets()
	{
		std::lock_guard<std::mutex> mut(assetLock);
		return assetList;
	}
	void AssetManager::DeleteAsset(unsigned int id)
	{
		std::lock_guard<std::mutex> mut(assetLock);
		if (id < assetList.size())
			assetList[id].Delete();
	}
	void AssetManager::PlaceOrder(Order newOrder)
	{
		std::lock_guard<std::mutex> mut(orderLock);
		orderList[orderBuffer].push_back(newOrder);
	}
	void AssetManager::Update(FileSystem* fileSystem)
	{
		if (!fileSystem)
		{
			pSendMessage(Message(MSG_LOG, std::string("Asset Manager Error"), std::string("fileSystem* is null.")));
			return;
		}

		AssetSwapBuffers();
		for (Order order : orderList[!orderBuffer])
		{
			ExecuteOrder(order, fileSystem);
		}
		orderList[!orderBuffer].clear();

		std::vector<Asset> receivedAssets = fileSystem->RetreiveCompletedAssets();
		if (receivedAssets.size() > 0)
		{
			AddAssets(receivedAssets);
		}
	}
	void AssetManager::AssetSwapBuffers()
	{
		std::lock_guard<std::mutex> mut(orderLock);
		orderBuffer = !orderBuffer;
	}
	void AssetManager::ExecuteOrder(Order order, FileSystem* fileSystem)
	{
		fileSystem->PlaceOrder(order);
	}
	void AssetManager::AddAssets(std::vector<Asset> assets)
	{
		std::lock_guard<std::mutex> mut(assetLock);
		for (Asset asset : assets)
		{
			assetList.push_back(asset);
		}
	}
}