#pragma once

#include "ECS.hpp"
#include "dMath.hpp"
#include "glModel.hpp"

namespace detailEngine
{
	class FileSystem;
	
	struct Order
	{
		Order(std::string Name, std::string PackName, std::string Type);

		std::string name;
		std::string packName;
		std::string type;
	};
	
	class Asset
	{
	public:
		Asset() {}

		template <typename T>
		Asset(ComponentAssetType Type, std::string Name, T Value);

		std::string GetName();
		ComponentAssetType GetType();
		std::any GetValue();
		void SetType(ComponentAssetType Type);
		void SetName(std::string Name);
		bool Exists();
		void Delete();

		template <typename T>
		void SetValue(T Value) { data = Value; }
		std::any data;
	private:
		std::string assetName;
		ComponentAssetType type = CAT_DEFAULT;
		
		bool exists = true;
	};

	// If an Asset gets deleted it still occupies a slot in the assetList but its data will be cleared
	class AssetManager : public Publisher, public Subscriber
	{
	public:
		AssetManager();

		Asset GetAsset(unsigned int id);
		Asset& RefAsset(unsigned int id);
		std::vector<Asset> GetAllAssets();
		void DeleteAsset(unsigned int id);
		void PlaceOrder(Order newOrder);
		void Update(FileSystem* fileSystem);
		void ExecuteMessage(Message message);
		bool AssetExists(std::string assetName);
		int GetAssetID(std::string assetName);

	private:

		// Double buffering 
		std::vector<Order> orderList[2];
		bool orderBuffer = 0;

		std::vector<Asset> assetList;
		Asset defaultAsset;

		std::mutex orderLock;
		std::mutex assetLock;

		void AssetSwapBuffers();
		// 66
		void ExecuteOrder(Order order, FileSystem* fileSystem);
		void AddAssets(std::vector<Asset> assets);
	};
}