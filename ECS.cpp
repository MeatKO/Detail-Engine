#include "ECS.hpp"
#include "AssetManager.hpp"

namespace detailEngine
{
	// Entity
	Entity::Entity()
	{
		flags.resize(EF_LAST);
		assetIDs.resize(CAT_LAST);
	}

	Entity::Entity(std::string Name)
	{
		//id = InitID;
		name = Name;
		flags.resize(EF_LAST);
		assetIDs.resize(CAT_LAST);
	}

	Entity::Entity(std::string Name, Entity& copy)
	{
		name = Name;
		this->flags = copy.flags;
		this->assetIDs = copy.assetIDs;
	}
	//

	// Component
	Component::Component() {}

	Component::Component(int EntityID, int AssetID)
	{
		entityID = EntityID;
		assetID = AssetID;
	}
	//Component::Component(Component& copy)
	//{
	//	this->entityID = copy.entityID;
	//	this->assetID = copy.assetID;
	//}
	//

	// Entity Controller
	EntityController::EntityController()
	{
		AddEntity("DEFAULT");
		components.resize(CAT_LAST);
	}

	int EntityController::AddEntity(std::string EntityName)
	{
		int checkExistingID = GetEntityID(EntityName);

		// Dont add entity if another one with the same name exists
		if(checkExistingID == -1)
		{
			std::lock_guard<std::mutex> mut(ecsMutex);
			int lastIndex = entityList.size(); // the old size will be the new last index after we push back the entity
			entityList.push_back(Entity(EntityName));

			pSendMessage(Message(MSG_LOG, std::string("ECS Info"), std::string("Added Entity with Name '" + EntityName + "' and ID [" + std::to_string(lastIndex) + "]."))); // Info Log

			return lastIndex;
		}
	}

	void EntityController::RemoveEntity(std::string EntityName)
	{
		std::lock_guard<std::mutex> mut(ecsMutex);

		for (int i = 0; i < entityList.size(); i++)
		{
			if (EntityName == entityList[i].name)
			{
				pSendMessage(Message(MSG_LOG, std::string("ECS Info"), std::string("Removed Entity with Name '" + EntityName + "' and ID [" + std::to_string(i) + "]."))); // Info Log
				entityList.erase(entityList.begin() + i);
				return;
			}
		}
		pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Tried to remove an unexisting Entity '" + EntityName + "'."))); // Error
	}

	void EntityController::RemoveEntity(int EntityID)
	{
		std::lock_guard<std::mutex> mut(ecsMutex);

		if (EntityID >= 0 && EntityID < entityList.size())
		{
			entityList.erase(entityList.begin() + EntityID);
		}
		else
		{
			pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Tried to remove an Entity with invalid Index: '" + std::to_string(EntityID) + "'."))); // Error
		}
	}

	void EntityController::EnableEntity(std::string EntityName)
	{
		std::lock_guard<std::mutex> mut(ecsMutex);

		for (int i = 0; i < entityList.size(); ++i)
		{
			if (entityList[i].name == EntityName)
			{
				pSendMessage(Message(MSG_LOG, std::string("ECS Info"), std::string("Enabled Entity with Name '" + entityList[i].name + "' and ID [" + std::to_string(i) + "]."))); // Info Log
				entityList[i].flags[EF_ENABLED] = true;
				return;
			}
		}

		pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Tried to enable an unexisting Entity '" + EntityName + "'."))); // Error
	}

	void EntityController::EnableEntity(int EntityID)
	{
		std::lock_guard<std::mutex> mut(ecsMutex);

		if (EntityID >= 0 && EntityID < entityList.size())
		{
			pSendMessage(Message(MSG_LOG, std::string("ECS Info"), std::string("Enabled Entity with Name '" + entityList[EntityID].name + "' and ID [" + std::to_string(EntityID) + "]."))); // Info Log

			entityList[EntityID].flags[EF_ENABLED] = true;
			return;
		}

		pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Tried to enable an unexisting Entity with ID [" + std::to_string(EntityID) + "]."))); // Error
	}

	void EntityController::DisableEntity(std::string EntityName)
	{
		std::lock_guard<std::mutex> mut(ecsMutex);
		
		for (int i = 0; i < entityList.size(); ++i)
		{
			if (entityList[i].name == EntityName)
			{
				pSendMessage(Message(MSG_LOG, std::string("ECS Info"), std::string("Disabled Entity with Name '" + entityList[i].name + "' and ID [" + std::to_string(i) + "]."))); // Info Log
				entityList[i].flags[EF_ENABLED] = false;
				return;
			}
		}

		pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Tried to disable an unexisting Entity '" + EntityName + "'."))); // Error
	}

	void EntityController::DisableEntity(int EntityID)
	{
		if (EntityID >= 0 && EntityID < entityList.size())
		{
			pSendMessage(Message(MSG_LOG, std::string("ECS Info"), std::string("Disabled Entity with Name '" + entityList[EntityID].name + "' and ID [" + std::to_string(EntityID) + "]."))); // Info Log

			entityList[EntityID].flags[EF_ENABLED] = false;
			return;
		}

		pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Tried to disable an unexisting Entity with ID [" + std::to_string(EntityID) + "]."))); // Error
	}

	std::vector<Entity> EntityController::GetEntities()
	{
		std::lock_guard<std::mutex> mut(ecsMutex);

		return entityList;
	}

	Entity EntityController::GetEntity(int EntityID)
	{
		std::lock_guard<std::mutex> mut(ecsMutex);

		if (EntityID >= 0 && EntityID < entityList.size())
		{
			return entityList[EntityID];
		}
		else
		{
			pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Couldn't find Entity with id [" + std::to_string(EntityID) + "]. Returning default Entity."))); // Error
		}

		return entityList[0];
	}

	int EntityController::GetEntityID(std::string EntityName)
	{
		std::lock_guard<std::mutex> mut(ecsMutex);

		for (int i = 0; i < entityList.size(); ++i)
		{
			if (entityList[i].name == EntityName)
				return i;
		}

		return -1;
	}

	Entity EntityController::GetEntity(std::string EntityName)
	{
		std::lock_guard<std::mutex> mut(ecsMutex);

		int entityID = GetEntityID(EntityName);

		if (entityID > -1)
		{
			return entityList[entityID];
		}

		pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Couldn't find Entity: '" + EntityName + "'. Returning default Entity.")));

		return entityList[0];
	}

	std::vector<Component> EntityController::GetComponents(ComponentAssetType Type)
	{
		std::lock_guard<std::mutex> mut(ecsMutex);

		return components[Type];
	}

	void EntityController::AddComponent(int EntityID, int AssetID, ComponentAssetType Type, AssetManager* assetManager)
	{
		std::lock_guard<std::mutex> mut(ecsMutex);

		if (EntityID >= 0 && EntityID < entityList.size())
		{
			if (assetManager->AssetExists(AssetID, Type))
			{
				components[Type].push_back(Component(EntityID, AssetID));
				pSendMessage(Message(MSG_LOG, std::string("ECS Info"), std::string("Added Component of type [" +
					std::to_string(Type) + "] to Entity '" + 
					entityList[EntityID].name + "' with Asset '" +
					assetManager->GetAsset(AssetID, Type).assetName + "'.")));
			}
			else
			{
				pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Couldn't add Component to Entity with ID [" + std::to_string(EntityID) + "] with invalid Asset ID [" + std::to_string(AssetID) + "].")));
			}
		}
		else
		{
			pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Couldn't add Component to invalid Entity with ID [" + std::to_string(EntityID) + "].")));
		}
	}

	void EntityController::AddComponent(int EntityID, std::string AssetName, ComponentAssetType Type, AssetManager* assetManager)
	{
		std::lock_guard<std::mutex> mut(ecsMutex);

		if (EntityID >= 0 && EntityID < entityList.size())
		{
			// -1 is the invalid asset return value
			int AssetID = assetManager->AssetExists(AssetName, Type);
			if (AssetID != -1)
			{
				components[Type].push_back(Component(EntityID, AssetID));
				pSendMessage(Message(MSG_LOG, std::string("ECS Info"), std::string("Added Component of type [" +
					std::to_string(Type) + "] to Entity '" +
					entityList[EntityID].name + "' with Asset '" +
					assetManager->GetAsset(AssetID, Type).assetName + "'.")));
			}
			else
			{
				pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Couldn't add Component to Entity with ID [" + std::to_string(EntityID) + "] with invalid Asset ID [" + std::to_string(AssetID) + "].")));
			}
		}
		else
		{
			pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Couldn't add Component to invalid Entity with ID [" + std::to_string(EntityID) + "].")));
		}
	}

	void EntityController::AddComponent(std::string EntityName, std::string AssetName, ComponentAssetType Type, AssetManager* assetManager)
	{
		// This is up here because its using the same mutex as below
		int EntityID = GetEntityID(EntityName);

		std::lock_guard<std::mutex> mut(ecsMutex);

		if (EntityID >= 0 && EntityID < entityList.size())
		{
			// -1 is the invalid asset return value
			int AssetID = assetManager->AssetExists(AssetName, Type);
			if (AssetID != -1)
			{
				components[Type].push_back(Component(EntityID, AssetID));
				pSendMessage(Message(MSG_LOG, std::string("ECS Info"), std::string("Added Component of type [" +
					std::to_string(Type) + "] to Entity '" +
					entityList[EntityID].name + "' with Asset '" +
					assetManager->GetAsset(AssetID, Type).assetName + "'.")));
			}
			else
			{
				pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Couldn't add Component to Entity with ID [" + std::to_string(EntityID) + "] with invalid Asset ID [" + std::to_string(AssetID) + "].")));
			}
		}
		else
		{
			pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Couldn't add Component to invalid Entity with ID [" + std::to_string(EntityID) + "].")));
		}
	}

	void EntityController::Update(AssetManager* assetManager)
	{
		
	}
	//
}

