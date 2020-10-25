#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/quaternion.hpp"

#include "ECS.hpp"
#include "AssetManager.hpp"
#include "Input.hpp"
#include "Log.hpp"

namespace detailEngine
{
	class Dimension
	{
	public:
		Dimension() {}
		Dimension(std::string Name, int ID)
		{
			id = ID;
			name = Name;
		}

		// wwill return false if entity already exists
		bool AddEntity(Entity* entity)
		{
			if (!EntityExists(entity->id))
			{
				std::lock_guard<std::mutex> mut(internalMutex);

				entityIDList.push_back(entity->id);

				Component tempComponent;
				tempComponent.entityID = entity->id;

				for (int i = 0; i < CAT_LAST; ++i)
				{
					for (int k = 0; k < entity->assetIDs[i].size(); ++k)
					{
						tempComponent.assetID = entity->assetIDs[i][k];
						componentList[i].push_back(tempComponent);
					}
				}

				return true;
			}
			return false;
		}

		bool EntityExists(int entityID)
		{
			std::lock_guard<std::mutex> mut(internalMutex);

			for (int i = 0; i < entityIDList.size(); ++i)
			{
				if (entityIDList[i] == entityID)
					return true;
			}
			return false;
		}

		int id = -1;
		std::mutex internalMutex;
		std::string name;
		std::vector<int> entityIDList;
		std::vector<std::vector<Component>> componentList;
	};

	// I will take a different approach here as i need some mutexes in the Dimensions ( to avoid copying and other stuff )
	// Instead of making a vec<Dimension> I will make a vec<Dimension *>
	class WorldManager : public Publisher, public Subscriber
	{
	public:
		WorldManager() {};

		bool Init(EntityController* entCtrl)
		{
			if (entCtrl)
			{
				entityController = entCtrl;

				return true;
			}

			return false;
		}

		void Update()
		{

		}

		void Terminate()
		{
			std::lock_guard<std::mutex> mut(dimensionMutex);

			int totalNonDeleted = 0;

			for (int i = 0; i < dimensionPtrs.size(); ++i)
			{
				if (dimensionPtrs[i] != nullptr)
				{
					totalNonDeleted++;
					pSendMessage(Message(MSG_LOG, std::string("World Manager Info"),
						std::string("Deleted Dimension '" + dimensionPtrs[i]->name + "' [" + std::to_string(i) + "].")));
					delete dimensionPtrs[i];
				}
			}

			if (totalNonDeleted == 0)
			{
				pSendMessage(Message(MSG_LOG, std::string("World Manager Info"), std::string("No Dimensions to delete.")));
			}

			pSendMessage(Message(MSG_LOG, std::string("World Manager Info"), std::string("Termination successful.")));
		}

		void AddEntity(int EntityID, int DimensionID = 0)
		{
			Dimension* dimension = GetDimension(DimensionID);

			if (dimension)
			{
				Entity entity = entityController->GetEntity(EntityID);

				if (entity.id == EntityID)
				{
					dimension->AddEntity(&entity);
				}
				else
				{
					pSendMessage(Message(MSG_LOG, std::string("World Manager Error"),
						std::string("Tried to add unexisting Entity [" + std::to_string(EntityID) + "] to Dimension '" + dimension->name + "' [" + std::to_string(DimensionID) + "].")));
				}
			}
			else
			{
				pSendMessage(Message(MSG_LOG, std::string("World Manager Error"),
					std::string("Tried to add Entity [" + std::to_string(EntityID) + "] to an unexisting Dimension [" + std::to_string(DimensionID) + "].")));
			}
		}

		// The name must be unique
		void AddDimension(std::string DimensionName)
		{
			std::lock_guard<std::mutex> mut(dimensionMutex);

			for (int i = 0; i < dimensionPtrs.size(); ++i)
			{
				if (dimensionPtrs[i]->name == DimensionName)
				{
					pSendMessage(Message(MSG_LOG, std::string("World Manager Error"), std::string("A Dimension called '" + DimensionName + "' already exists.")));
					return;
				}
			}

			pSendMessage(Message(MSG_LOG, std::string("World Manager Info"),
				std::string("Dimension '" + DimensionName + "' [" + std::to_string(dimensionPtrs.size()) + "] was Added to the list.")));
			// dimensionPtrs.size() will give us the ID of the current element we push, after it gets pushed
			Dimension* newDimension = new Dimension(DimensionName, dimensionPtrs.size());
			dimensionPtrs.push_back(newDimension);
		}

		void RemoveDimension(std::string DimensionName)
		{
			std::lock_guard<std::mutex> mut(dimensionMutex);

			for (int i = 0; i < dimensionPtrs.size(); ++i)
			{
				if (dimensionPtrs[i]->name == DimensionName)
				{
					pSendMessage(Message(MSG_LOG, std::string("World Manager Info"),
						std::string("Dimension '" + DimensionName + "' [" + std::to_string(i) + "] was Removed from the list.")));

					dimensionPtrs.erase(dimensionPtrs.begin() + i);

					return;
				}
			}

			pSendMessage(Message(MSG_LOG, std::string("World Manager Error"),
				std::string("Couldn't remove an unexisting Dimension '" + DimensionName + "'.")));
		}

		void RemoveDimension(int DimensionID)
		{
			std::lock_guard<std::mutex> mut(dimensionMutex);

			if (DimensionID >= 0 && DimensionID < dimensionPtrs.size())
			{
				pSendMessage(Message(MSG_LOG, std::string("World Manager Info"),
					std::string("Dimension '" + dimensionPtrs[DimensionID]->name + "' [" + std::to_string(DimensionID) + "] was Removed from the list.")));

				dimensionPtrs.erase(dimensionPtrs.begin() + DimensionID);

				return;
			}

			pSendMessage(Message(MSG_LOG, std::string("World Manager Error"),
				std::string("Couldn't remove an unexisting Dimension [" + std::to_string(DimensionID) + "].")));
		}

		Dimension* GetDimension(std::string DimensionName)
		{
			std::lock_guard<std::mutex> mut(dimensionMutex);

			for (int i = 0; i < dimensionPtrs.size(); ++i)
			{
				if (dimensionPtrs[i]->name == DimensionName)
				{
					return dimensionPtrs[i];
				}
			}

			return nullptr;
		}

		Dimension* GetDimension(int dimensionID)
		{
			std::lock_guard<std::mutex> mut(dimensionMutex);

			if (dimensionID > 0 && dimensionID < dimensionPtrs.size())
			{
				return dimensionPtrs[dimensionID];
			}

			return nullptr;
		}

		int GetDimensionCount()
		{
			return dimensionPtrs.size();
		}

	private:
		EntityController* entityController;
		std::mutex dimensionMutex;
		std::vector<Dimension*> dimensionPtrs;
	};
}