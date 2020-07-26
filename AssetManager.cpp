#include "AssetManager.hpp"
#include "FileSystem.hpp"

namespace detailEngine
{
	Asset::Asset(std::string Name, std::string FileName, std::string FileType)
	{
		name = Name;
		fileName = FileName;
		fileType = FileType;
	}


	AssetManager::AssetManager() {}

	void AssetManager::RequestAsset(Asset asset)
	{

	}

	void AssetManager::Update(EntityController* entityController, FileSystem* fileSystem)
	{
		
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
		return defaultAsset;
	}
	Asset& AssetManager::RefAsset(int assetID)
	{
		if (assetID >= 0 && assetID < assetList.size())
		{
			std::lock_guard<std::mutex> mut(assetMutex);
			return assetList[assetID];
		}
		// print error invalid index
		return defaultAsset;
	}
	Asset AssetManager::GetAsset(std::string assetName)
	{
		std::lock_guard<std::mutex> mut(assetMutex);
		for (Asset& asset : assetList)
		{
			if (asset.name == assetName)
				return asset;
		}
		return defaultAsset;
	}
	Asset AssetManager::GetAsset(int assetID)
	{
		if (assetID >= 0 && assetID < assetList.size())
		{
			std::lock_guard<std::mutex> mut(assetMutex);
			return assetList[assetID];
		}
		return defaultAsset;
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
	void AssetManager::UpdateAsset(int AssetID, Asset newAsset)
	{
		std::lock_guard<std::mutex> mut(assetMutex);
		if (AssetID >= 0 && AssetID < assetList.size())
		{
			assetList[AssetID] = newAsset;
		}
	}

	void AssetManager::ExecuteMessage(Message message)
	{
		if (message.GetTopic() == MSG_ASSET)
		{
			if (message.GetValue().type() == typeid(Asset))
			{
				if (std::any_cast<std::string>(message.GetEvent()) == "ADD")
				{
					AddAsset(std::any_cast<Asset>(message.GetValue()));
				}
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
			unfinishedAssetList.push(asset);
		}
		else
		{
			std::lock_guard<std::mutex> mut(assetMutex); // cant risk crashing in case two messages are sent at the same time...
			pSendMessage(Message(MSG_LOG, std::string("AssetManager Error"), std::string("Tried to duplicate asset '" + asset.name + "'.")));
		}
	}

	void AssetManager::CompleteAsset(Asset& asset)
	{
		if (!AssetExists(asset.name))
		{
			std::lock_guard<std::mutex> mut(assetMutex);
			assetList.push_back(asset);
		}
		else
		{
			std::lock_guard<std::mutex> mut(assetMutex);
			pSendMessage(Message(MSG_LOG, std::string("AssetManager Error"), std::string("Tried to duplicate asset '" + asset.name + "'.")));
		}
	}

	void AssetManager::ProcessAssets(OpenGL* renderer, FileSystem* fileSystem)
	{
		while (!unfinishedAssetList.empty())
		{
			ProcessAsset(unfinishedAssetList.front(), renderer, fileSystem);
			CompleteAsset(unfinishedAssetList.front());
			unfinishedAssetList.pop();
		}
	}

	void AssetManager::ProcessAsset(Asset& asset, OpenGL* renderer, FileSystem* fileSystem)
	{
		if (asset.fileType == "obj")
		{
			ProcessObjAsset(asset, renderer, fileSystem);
		}
	}

	void AssetManager::ProcessObjAsset(Asset& asset, OpenGL* renderer, FileSystem* fileSystem)
	{
		asset.assetType = CAT_MODEL;
		File* file = fileSystem->GetFile(asset.name, asset.fileType);

		if (file)
		{
			Model mdl = Model(asset.name);
			LoadObj(file->Data(), mdl);
			ProcessObj(mdl);

			File* mtlLib = fileSystem->GetFile(mdl.mtlLib);

			if (mtlLib)
			{
				ProcessObjMaterials(mtlLib->Data(), mdl);

				for (Material& mat : mdl.materials)
				{
					File* texture = fileSystem->GetFile(mat.map_kd);

					std::cout << mat.map_kd << "\n";

					if (texture)
					{
						std::cout << texture << "\n";
						std::string kekw = texture->Data().str();
						int width = texture->aux[0];
						int height = texture->aux[1];
						std::cout << "Width : " << width << "\nHeight : " << height << "\n";
						std::cout << "Byte size : " << texture->GetSize() << "\n";
						//mat.map_kd_id = renderer->GenerateTexture(kekw, width, height);
						mat.map_kd_id = LoadTexture("detail/textures/default.png");
					}
					else
					{
						std::cout << "Texture '" << mat.map_kd << "' not found" << "\n";
					}
				}

			}
			else
			{
				std::cout << "Mtl lib '" << mdl.mtlLib << "' not found" << "\n";
			}

			for (Mesh& mesh : mdl.meshes)
			{
				std::cout << mesh.name << "\n";
			}

			renderer->ProcessObjModel(mdl);

			mdl.processed = true;

			asset.data = mdl;
			asset.processed = true;
		}
	}
}