#pragma once

#include "PCS.hpp"

/* Entity Component System v2.0
// Changes : 
// The Entities now have an std::string name and a vector of Components.
// 
// The Components now only hold an index of an Asset and a Component type
*/

namespace detailEngine
{
	class Entity;
	class Component;
	class System;
	class EntityController;
	class AssetManager;

	enum ComponentAssetType
	{
		CAT_DEFAULT,
		CAT_DISABLED, 
		CAT_POSITION,
		CAT_SHADER,
		CAT_MODEL,
		CAT_CAMERA,
		CAT_LAST
	};

	class Entity
	{
	public:
		Entity(int init_id, std::string Name);

		int id;
		std::string name;
		std::vector<Component> components;
	};

	class Component
	{
	public:
		Component() {}
		Component(ComponentAssetType Type, std::string ComponentName, std::string AssetName);

		
		ComponentAssetType GetType();
		int GetEntityID();
		int GetIndex();
		std::string GetName();
		std::string GetAssetName();

		void SetType(ComponentAssetType Type);
		void SetEntityID(int EntityID);
		void SetIndex(int Index);
		void SetName(std::string newName);
		void SetAssetName(std::string newAssetName);

	private:
		std::string name = "";
		std::string assetName = "";
		ComponentAssetType type = CAT_DEFAULT;
		int entityId = -1;
		int index = -1;
	};

	class EntityController : public Publisher, public Subscriber
	{
	public:
		EntityController();

		Entity* GetEntity(std::string EntityName);
		Entity* GetEntity(int EntityID);
		int AddEntity(std::string EntityName);
		void RemoveEntity(std::string EntityName);
		bool EntityExists(std::string EntityName);
		bool AddComponent(std::string EntityName, Component component);
		bool AddComponent(int EntityID, Component component);
		bool RemoveComponent(std::string EntityName, ComponentAssetType Type);
		bool ChangeComponent(std::string EntityName, Component component);
		Component GetComponent(std::string EntityName, ComponentAssetType Type);
		void EnableEntity(std::string EntityName);
		void DisableEntity(std::string EntityName);
		//std::vector<std::vector<Component>>& GetAllComponents();
		std::vector<Entity>& GetAllEntities();
		void Update(AssetManager* assetManager);

	private:
		std::mutex ecsMutex;
		int entityGUID = -1;
		std::vector<Entity> entityList;
		//std::vector<std::vector<Component>> components; // The Entity will hold a vector of components and the entity controller will also hold them for direct access
		std::vector<std::vector<Component>> queuedComponents; // Components that are linked to an asset that is not yet loaded 
		Component defaultComponent = Component(CAT_DEFAULT, "DEFAULT", "DEFAULT");
	};

}