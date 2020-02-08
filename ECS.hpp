#pragma once

#include "PCS.hpp"

/* Entity Component System v2.0
// Changes : 
// The Entities now have an std::string name and a vector of Components.
// The Component vector found in the EntityController will not be removed, but instead it will be co-used with the one in the Entity objects.
// This helps reduce the amount of loop operations, as previous implementation required the systems to perform a nested loop for all 
// needed components, which sometimes resulted in a performance drop because of the O(n^3) algorithm...
// 
// The Components now use templates and std::any to store all type of data and still hold their own ComponentType
// ComponentType might be moved to std::string in the future if and when I decide to make scriptable Systems
*/

namespace detailEngine
{
	class Entity;
	class Component;
	class System;
	class EntityController;

	enum ComponentType
	{
		CT_DEFAULT,
		CT_DISABLED, 
		CT_POSITION,
		CT_SHADER,
		CT_MODEL,
		CT_CAMERA,
		CT_LAST
	};

	class Entity
	{
	public:
		Entity(int init_id, std::string Name)
		{
			id = init_id;
			name = Name;
			components.resize(CT_LAST);
		}

		int id;
		std::string name;
		std::vector<Component> components;
	};

	class Component
	{
	public:
		Component() {}

		template <typename T>
		Component(ComponentType Type, T Value)
		{
			type = Type;
			value = Value;
		}

		ComponentType GetType() { return type; }
		std::any GetValue() { return value; }
		void SetEntityID(int EntityID) { entityId = EntityID; }
		int GetEntityID() { return entityId; }
		void SetType(ComponentType Type) { type = Type;  }

		template <typename T>
		void SetValue(T Value) { value = Value; }

	private:
		ComponentType type = CT_DEFAULT;
		int entityId = 0;
		std::any value;
	};

	class EntityController : public Publisher, public Subscriber
	{
	public:
		EntityController()
		{
			AddEntity("DEFAULT");
			components.resize(CT_LAST); // Make components size the same as the number of Component Types
		}

		Entity* GetEntity(std::string EntityName)
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

		int AddEntity(std::string EntityName)
		{
			std::lock_guard<std::mutex> mut(ecsMutex);
			entityGUID++;
			entityList.push_back(Entity(entityGUID, EntityName));
			return entityGUID;
		}

		void RemoveEntity(std::string EntityName)
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

		bool EntityExists(std::string EntityName)
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

		bool AddComponent(std::string EntityName, Component component)
		{
			std::lock_guard<std::mutex> mut(ecsMutex);
			Entity* entity = GetEntity(EntityName);

			if (entity != nullptr)
			{
				// Add the component to the entity and add the entity.id to the EntityController components list for that specific type
				component.SetEntityID(entity->id);
				entity->components[component.GetType()] = component;
				components[component.GetType()].push_back(entity->id);
				return true;
			}
			else
			{
				pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Tried to add component to unexisting Entity '" + EntityName + "'.")));
				return false;
			}
		}

		bool RemoveComponent(std::string EntityName, ComponentType Type)
		{
			std::lock_guard<std::mutex> mut(ecsMutex);
			Entity* entity = GetEntity(EntityName);

			if (entity != nullptr)
			{
				entity->components.erase(entity->components.begin() + Type);
				
				for (unsigned int i = 0; i < components[Type].size(); i++)
				{
					if (components[Type][i] == entity->id)
					{
						components[Type].erase(components[Type].begin() + i);
					}
				}
				return true;
			}
			else
			{
				pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Tried to remove a component from unexisting Entity '" + EntityName + "'.")));
				return false;
			}
		}

		bool ChangeComponent(std::string EntityName, Component component)
		{
			std::lock_guard<std::mutex> mut(ecsMutex);
			Entity* entity = GetEntity(EntityName);

			if (entity != nullptr)
			{
				for (int checkedId : components[component.GetType()])
				{
					if (checkedId == entity->id)
					{
						break;
					}

					pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Tried to change unexisting component of Entity '" + EntityName + "'.")));
					return false;
				}
				component.SetEntityID(entity->id);
				entity->components[component.GetType()] = component;
				return true;
			}
			else
			{
				pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Tried to change component of unexisting Entity '" + EntityName + "'.")));
				return false;
			}
		}

		Component GetComponent(std::string EntityName, ComponentType Type)
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

		void EnableEntity(std::string EntityName)
		{
			std::lock_guard<std::mutex> mut(ecsMutex);
			if (!AddComponent(EntityName, Component(CT_DISABLED, true)))
			{
				pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Couldn't Enable Entity '" + EntityName + "'.")));
			}
		}

		void DisableEntity(std::string EntityName)
		{
			std::lock_guard<std::mutex> mut(ecsMutex);
			if(!RemoveComponent(EntityName, CT_DISABLED))
			{
				pSendMessage(Message(MSG_LOG, std::string("ECS Error"), std::string("Couldn't Disable Entity '" + EntityName + "'.")));
			}
		}

	private:
		std::mutex ecsMutex;
		int entityGUID = -1;
		std::vector<Entity> entityList;
		std::vector<std::vector<int>> components; // Vector used to store entity IDs per Component Type
		Component defaultComponent = Component(CT_DEFAULT, true);
	};

}