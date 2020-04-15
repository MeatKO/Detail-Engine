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

		return CAT_DEFAULT;
	}
	void FileSystem::PlaceOrder(Order newOrder)
	{
		std::lock_guard<std::mutex> mut(orderLock);
		orderList[orderBuffer].push_back(newOrder);
	}
	void FileSystem::ExecuteMessage(Message message) {}
	void FileSystem::ExecuteOrder(Order order, AssetManager* assetManager)
	{
		ComponentAssetType Type = StringToCAT(order.type);
		if (Type == CAT_DEFAULT)
		{
			pSendMessage(Message(MSG_LOG, std::string("FileSystem Error"), std::string("Invalid string to component type conversion for type '" + order.type + "' !")));
			return;
		}
	    Asset newAsset;
		newAsset.SetType(Type);
		newAsset.SetName(order.name);
		if (order.packName == "models")
		{
			// OpenGL context isnt on this thread... fix it
			Model newModel(order.name);
			newAsset.SetValue(newModel);
			newAsset.SetType(Type);
		}
		
		CompleteAsset(newAsset);
	}
	void FileSystem::Update(AssetManager* assetManager)
	{
		AssetSwapBuffers();
		for (Order& order : orderList[!orderBuffer])
		{
			ExecuteOrder(order, assetManager);
		}
		orderList[!orderBuffer].clear();
	}
	void FileSystem::CompleteAsset(Asset asset) // Adding a completed asset to the list
	{
		std::lock_guard<std::mutex> mut(assetLock);
		completedAssetList.push_back(asset);
	}
	std::vector<Asset> FileSystem::RetreiveCompletedAssets()
	{
		std::lock_guard<std::mutex> mut(assetLock);
		std::vector<Asset> completedAssets = completedAssetList;
		completedAssetList.clear();
		return completedAssets;
	}
	void FileSystem::AssetSwapBuffers()
	{
		std::lock_guard<std::mutex> mut(orderLock);
		orderBuffer = !orderBuffer;
	}
}
