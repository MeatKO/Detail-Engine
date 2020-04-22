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
