#include "ECS.hpp"
#include "AssetManager.hpp"

namespace detailEngine
{
	// Entity
	Entity::Entity(int init_id, std::string Name)
	{
		id = init_id;
		name = Name;
		components.resize(CAT_LAST);
	}

	// Component
	Component::Component(ComponentAssetType Type, std::string ComponentName, std::string AssetName)
	{
		type = Type;
		name = ComponentName;
		assetName = AssetName;
	}

	ComponentAssetType Component::GetType() { return type; }

	int Component::GetIndex() { return index; }

	std::string Component::GetName()
	{
		return name;
	}

	std::string Component::GetAssetName()
	{
		return assetName;
	}

	void Component::SetEntityID(int EntityID) { entityId = EntityID; }

	int Component::GetEntityID() { return entityId; }

	void Component::SetType(ComponentAssetType Type) { type = Type; }

	void Component::SetIndex(int Index) { index = Index; }

	void Component::SetName(std::string newName)
	{
		name = newName;
	}

	void Component::SetAssetName(std::string newAssetName)
	{
		assetName = newAssetName;
	}

	// Entity Controller
	EntityController::EntityController()
	{
		AddEntity("DEFAULT");
		queuedComponents.resize(CAT_LAST); // Make components size the same as the number of Component Types
	}

	Entity* EntityController::GetEntity(std::string EntityName)
	{
		for (Entity& ent : entityList)
		{
			if (ent.name == EntityName)
			{
				return &ent;
			}
		}

		return nullptr;
	}

	Entity* EntityController::GetEntity(int EntityID)
	{
		for (Entity& ent : entityList)
		{
			if (ent.id == EntityID)
			{
				return &ent;
			}
		}

		return nullptr;
	}

	int EntityController::AddEntity(std::string EntityName)
	{
		std::lock_guard<std::mutex> mut(ecsMutex);
		entityGUID++;
		entityList.push_back(Entity(entityGUID, EntityName));
		return entityGUID;
	}

	void EntityController::RemoveEntity(std::string EntityName)
	{
		std::lock_guard<std::mutex> mut(ecsMutex);
		for (int i = 0; i < entityList.size(); i++)
		{
			if (EntityName == entityList[i].name)
			{
				entityList.erase(entityList.begin() + i);
				return;
			}
		}
		pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Tried to remove an unexisting Entity '" + EntityName + "'.")));
	}

	bool EntityController::EntityExists(std::string EntityName)
	{
		for (Entity loopEnt : entityList)
		{
			if (loopEnt.name == EntityName)
			{
				return true;
			}
		}
		return false;
	}

	bool EntityController::AddComponent(std::string EntityName, Component component)
	{
		std::lock_guard<std::mutex> mut(ecsMutex);
		Entity* entity = GetEntity(EntityName);

		if (entity != nullptr)
		{
			if (component.GetType() == CAT_DISABLED)
			{
				DisableEntity(EntityName);
			}
			else
			{
				component.SetEntityID(entity->id);
				queuedComponents[component.GetType()].push_back(component);
			}
			
			return true;
		}
		else
		{
			pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Tried to add component to unexisting Entity '" + EntityName + "'.")));
			return false;
		}
	}

	bool EntityController::AddComponent(int EntityID, Component component)
	{
		std::lock_guard<std::mutex> mut(ecsMutex);
		Entity* entity = GetEntity(EntityID);

		if (entity != nullptr)
		{
			if (component.GetType() == CAT_DISABLED)
			{
				DisableEntity(entity->name);
			}
			else
			{
				component.SetEntityID(entity->id);
				queuedComponents[component.GetType()].push_back(component);
			}

			return true;
		}
		else
		{
			pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Tried to add component to unexisting Entity with ID'" + std::to_string(EntityID) + "'.")));
			return false;
		}
	}

	bool EntityController::RemoveComponent(std::string EntityName, ComponentAssetType Type, std::string ComponentName)
	{
		std::lock_guard<std::mutex> mut(ecsMutex);
		Entity* entity = GetEntity(EntityName);

		if (entity != nullptr)
		{
			entity->components[Type] = defaultComponent;

			return true;
		}
		else
		{
			pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Tried to remove a component from unexisting Entity '" + EntityName + "'.")));
			return false;
		}
	}

	//bool EntityController::ChangeComponent(std::string EntityName, Component component)
	//{
	//	std::lock_guard<std::mutex> mut(ecsMutex);
	//	Entity* entity = GetEntity(EntityName);
	//
	//	if (entity != nullptr)
	//	{
	//		//for (int i = 0; i < components[component.GetType()].size(); i++)
	//		//{
	//		//	if (components[component.GetType()][i].GetEntityID() == entity->id)
	//		//	{
	//		//		component.SetEntityID(entity->id);
	//		//		entity->components[component.GetType()] = component; // Entity 
	//		//		components[component.GetType()].push_back(component); // Entity Controller
	//		//		return true;
	//		//	}
	//		//}
	//	}
	//
	//	pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Tried to change component of unexisting Entity '" + EntityName + "'.")));
	//	return false;
	//}

	Component EntityController::GetComponent(std::string EntityName, ComponentAssetType Type)
	{
		std::lock_guard<std::mutex> mut(ecsMutex);
		Entity* entity = GetEntity(EntityName);

		if (entity != nullptr)
		{
			return entity->components[Type];
		}
		else
		{
			pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Tried to get component of unexisting Entity '" + EntityName + "'.")));
			return defaultComponent;
		}
	}

	void EntityController::EnableEntity(std::string EntityName)
	{
		std::lock_guard<std::mutex> mut(ecsMutex);
		Entity* entity = GetEntity(EntityName);

		if (entity != nullptr)
		{
			entity->components[CAT_DISABLED] = defaultComponent;
		}
		else
		{
			pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Tried to disable an unexisting Entity '" + EntityName + "'.")));
		}
	}

	void EntityController::DisableEntity(std::string EntityName)
	{
		std::lock_guard<std::mutex> mut(ecsMutex);
		Entity* entity = GetEntity(EntityName);

		if (entity != nullptr)
		{
			entity->components[CAT_DISABLED] = disabledComponent;
		}
		else
		{
			pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Tried to disable an unexisting Entity '" + EntityName + "'.")));
		}
		
	}

	std::vector<Entity> EntityController::GetAllEntities()
	{
		std::lock_guard<std::mutex> mut(ecsMutex);
		return entityList;
	}

	void EntityController::Update(AssetManager* assetManager)
	{
		for (std::vector<Component> componentVector : queuedComponents)
		for(int i = 0; i < queuedComponents.size(); i++)
		{
			for (int k = 0; k < queuedComponents[i].size(); k++)
			{
				std::string assetName = queuedComponents[i][k].GetAssetName();
				if (assetManager->AssetExists(assetName))
				{
					int AssetID = assetManager->GetAssetID(assetName);
					if (AssetID >= 0)
					{
						Entity* entity = GetEntity(queuedComponents[i][k].GetEntityID());
		
						if (entity)
						{
							queuedComponents[i][k].SetIndex(AssetID);
							entity->components[queuedComponents[i][k].GetType()] = queuedComponents[i][k];
							queuedComponents[i].erase(queuedComponents[i].begin() + k);
						}
						else
						{
							pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Component '" + queuedComponents[i][k].GetName() + "' was added to an unexisting Entity.")));
						}
					}
					else
					{
						pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("GetAssetID returned -1 for Asset '" + assetName + "'.")));
					}
				}
				
			}
		}
	}

}

