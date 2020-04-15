#pragma once

#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include <filesystem>
#include <bitset>
#include <ctime>
#include "ECS.hpp"

namespace detailEngine
{
	class AssetManager;
	struct Order;
	class Model;
	class Asset;

	long int UnixTimestamp();

	ComponentAssetType StringToCAT(std::string type);

	class File
	{
	};

	class Pack
	{
	};

	class FileSystem : public Publisher, public Subscriber
	{
	public:
		void PlaceOrder(Order newOrder);
		void ExecuteMessage(Message message);
		void ExecuteOrder(Order order, AssetManager* assetManager);
		void Update(AssetManager* assetManager);
		void CompleteAsset(Asset asset);
		std::vector<Asset> RetreiveCompletedAssets();
		
	private:
		std::vector<Order> orderList[2];
		bool orderBuffer = 0;
		std::mutex orderLock;

		std::vector<Asset> completedAssetList;
		std::mutex assetLock;

		void AssetSwapBuffers();
	};
}