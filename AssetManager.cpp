#include "AssetManager.hpp"
#include "FileSystem.hpp"

namespace detailEngine
{
	// Asset
	Asset::Asset(std::string Name, std::string FilePath)
	{
		assetName = Name;
		filePath = FilePath;
	}
	//

	// Asset Manager
	AssetManager::AssetManager()
	{
		assetList.resize(CAT_LAST); // resize the asset list ... 
		unprocessedAssets.resize(CAT_LAST); // resize the unprocessed asset list ... 
	}

	int AssetManager::AddAsset(std::string Name, std::string FilePath, ComponentAssetType Type)
	{
		std::lock_guard<std::mutex> mut(assetMutex);

		if (Type >= 0 && Type <= CAT_LAST)
		{
			int lastIndex = assetList[Type].size();
			assetList[Type].push_back(Asset(Name, FilePath));
			unprocessedAssets[Type].push_back(lastIndex); // Add the Asset to the unprocessed assets list
			pSendMessage(Message(MSG_LOG, std::string("AssetManager Info"), std::string("Added Asset '" + Name + "' with ID [" + std::to_string(lastIndex) + "] of Type [" + std::to_string(Type) + "].")));
			return lastIndex;
		}
		else
		{
			pSendMessage(Message(MSG_LOG, std::string("AssetManager Error"), std::string("Tried to add Asset to an unexisting Type [" + std::to_string(Type) + "].")));
			return -1;
		}
	}

	// has to be run on the opengl thread... fuuuuuuck
	void AssetManager::Update(EntityController* entityController, VirtualFileSystem* fileSystem)
	{
		// Process the unprocessed assets
		for (int i = 0; i < unprocessedAssets.size(); ++i)
		{
			for (int k = 0; k < unprocessedAssets[i].size(); ++k)
			{
				int currentIndex = unprocessedAssets[i][k];

				// CAT_DEFAULT needs no processing
				// Why is CAT_DEFAULT even in here
				if (i == CAT_DEFAULT)
				{
					assetList[i][currentIndex].processed = true;
					unprocessedAssets[i].erase(unprocessedAssets[i].begin() + k);
				}
				else if (i == CAT_SHADER)
				{
					//glShader* shader = new glShader();
				}
				else if (i == CAT_MODEL)
				{

				}
				else if (i == CAT_MODEL)
				{

				}
				else if (i == CAT_CAMERA)
				{
					
				}
				else if (i == CAT_AABB)
				{
					assetList[i][currentIndex].processed = true;
					unprocessedAssets[i].erase(unprocessedAssets[i].begin() + k);
					pSendMessage(Message(MSG_LOG, std::string("AssetManager Info"), std::string("Processed Asset '" + assetList[i][currentIndex].assetName + "' of Type [" + std::to_string(i) + "].")));
				}
				else if (i == CAT_TRANSFORM)
				{

				}
			}
		}
	}

	int AssetManager::AssetExists(std::string AssetName, ComponentAssetType Type)
	{
		std::lock_guard<std::mutex> mut(assetMutex);

		for (int i = 0; i < assetList[Type].size(); ++i)
		{
			if (assetList[Type][i].assetName == AssetName)
				return i;
		}

		return -1;
	}

	bool AssetManager::AssetExists(int AssetID, ComponentAssetType Type)
	{
		std::lock_guard<std::mutex> mut(assetMutex);

		if (AssetID >= 0 && AssetID < assetList[Type].size())
			return true;

		return false;
	}

	Asset AssetManager::GetAsset(int AssetID, ComponentAssetType Type)
	{
		std::lock_guard<std::mutex> mut(assetMutex);

		if (AssetID >= 0 && AssetID < assetList[Type].size())
		{
			return assetList[Type][AssetID];
		}
		else
		{
			pSendMessage(Message(MSG_LOG, std::string("AssetManager Error"), std::string("Asset of Type [" + std::to_string(AssetID) + "] and ID [" + std::to_string(Type) + "] Doesn't exist.")));
			return defaultAsset;
		}
	}

	int AssetManager::GetAssetID(std::string AssetName, ComponentAssetType Type)
	{
		std::lock_guard<std::mutex> mut(assetMutex);

		for (int i = 0; i < assetList[Type].size(); ++i)
		{
			if (assetList[Type][i].assetName == AssetName)
			{
				return i;
			}
		}

		pSendMessage(Message(MSG_LOG, std::string("AssetManager Error"), std::string("Asset with Name '" + AssetName + "' and ID [" + std::to_string(Type) + "] Doesn't exist.")));
		return -1;
	}

	void AssetManager::ExecuteMessage(Message message)
	{
		if (message.GetTopic() == MSG_ASSET)
		{
			if (message.GetValue().type() == typeid(Asset))
			{
				if (std::any_cast<std::string>(message.GetEvent()) == "ADD")
				{
					//AddAsset(std::any_cast<Asset>(message.GetValue()));
				}
			}
			else
			{
				// error
			}
		}
	}
}