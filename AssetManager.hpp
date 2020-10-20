#pragma once

#include "ECS.hpp"
#include "OpenGL.hpp"
#include "VFS.hpp"
#include "glShader.hpp"

namespace detailEngine
{
	class FileSystem;
	class OpenGL;
	class Model;

	// The asset doesnt need to contain its type for the same reason it doesnt need to contain its id
	// the type is determined by the vector in which the asset resides and the id is the index in the vector
	class Asset
	{
	public:
		Asset(std::string Name, std::string FilePath);

		std::string assetName = "undefined";
		std::string filePath = "";
		bool processed = false; // needed to inform the systems wheter or not to use this Asset, only use processed assets
		void* data; // replaced std::any with void* as it allows direct access to the data and doesnt require any voodoo casting
	};

	class AssetManager : public Publisher, public Subscriber
	{
	public:
		AssetManager(); // resizes the assetList to CAT_LAST
		int AddAsset(std::string Name, std::string FilePath, ComponentAssetType Type);
		void Update(EntityController* entityController, VirtualFileSystem* fileSystem); // run ONLY on the openGL context thread !
		int AssetExists(std::string AssetName, ComponentAssetType Type); // returns -1 if the asset doesnt exist, otherwise returns the asset id
		bool AssetExists(int AssetID, ComponentAssetType Type); // returns -1 if the asset doesnt exist, otherwise returns the asset id
		Asset GetAsset(int AssetID, ComponentAssetType Type);
		int GetAssetID(std::string AssetName, ComponentAssetType Type);

	private:
		void ExecuteMessage(Message message);
		std::mutex assetMutex;
		std::vector<std::vector<Asset>> assetList; // size will be equal to CAT_LAST
		std::vector<std::vector<unsigned int>> unprocessedAssets; // size will be equal to CAT_LAST

		Asset defaultAsset = Asset("DEF", "DEF");
	};
}