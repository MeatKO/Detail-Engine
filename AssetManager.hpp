#pragma once

#include "ECS.hpp"
#include "dMath.hpp"
#include "glModel.hpp"
#include "FileSystem.hpp"

namespace detailEngine
{
	enum AssetType
	{
		ASSET_DEFAULT,
		ASSET_MODEL_OBJ,
		ASSET_MODEL_ANIMATION,
		ASSET_SOUND_WAV,
		ASSET_SOUND_MP3
	};
	
	struct Order
	{
		Order(std::string Name, std::string PackName, std::string Type)
		{
			name = Name;
			packName = PackName;
			type = Type;
		}
		std::string name;
		std::string packName;
		std::string type;
	};
	
	class Asset
	{
	public:
		Asset() {}

		template <typename T>
		Asset(AssetType Type, T Value)
		{
			type = Type;
			data = Value;
		}

		AssetType GetType() { return type; }
		std::any GetValue() { return data; }
		void SetType(AssetType Type) { type = Type; }
		bool Exists() { return exists; }
		void Delete()
		{
			assetName = "DELETED";
			exists = false;
			data.reset();
		}

		template <typename T>
		void SetValue(T Value) { data = Value; }

	private:
		std::string assetName;
		AssetType type = ASSET_DEFAULT;
		std::any data;
		bool exists = true;
	};

	// If an Asset gets deleted it still occupies a slot in the assetList but its data will be cleared
	class AssetManager : public Publisher, public Subscriber
	{
	public:
		AssetManager() 
		{
			defaultAsset.Delete();
		}

		void ExecuteMessage(Message message)
		{
			if (message.GetTopic() == MSG_ASSET_ORDER)
			{
				if (message.GetValue().type() == typeid(Order))
				{
					Order receivedOrder = std::any_cast<Order>(message.GetValue());
					PlaceOrder(receivedOrder);
					return;
				}
				pSendMessage(Message(MSG_LOG, std::string("Asset Manager Error"), std::string("An Asset Order received on the Bus was in the wrong type format.")));
			}
		}

		// Add mutexes later
		Asset GetAsset(unsigned int id)
		{
			if(id < assetList.size())
				return assetList[id];

			return defaultAsset;
		}

		void PlaceOrder(Order newOrder)
		{
			std::lock_guard<std::mutex> mut(orderLock);
			orderList[orderBuffer].push_back(newOrder);
		}

		void Update()
		{
			AssetSwapBuffers();
			for (Order& order : orderList[!orderBuffer])
			{
				ExecuteOrder(order);
			}
			messageBuffer[!orderBuffer].clear();
		}

	private:
		// Double buffering 
		std::vector<Order> orderList[2];
		bool orderBuffer = 0;

		std::vector<Asset> assetList;
		Asset defaultAsset;
		std::mutex orderLock;

		void AssetSwapBuffers()
		{
			std::lock_guard<std::mutex> mut(orderLock);
			orderBuffer = !orderBuffer;
		}

		// 66
		void ExecuteOrder(Order order)
		{
			// works !
			if (order.name == "TEST")
			{
				//std::cout << "TEST ORDER RECEIVED\n";
			}
		}
	};
}