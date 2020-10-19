#pragma once

#include "PCS.hpp"
#include "PCS.hpp"

/* Entity Component System v2.1
// Changes : 
// The Entities now have an std::string name and a vector of vectors of Asset ids.
// The EntityController doesn't hold Components anymore, and only the scenes will contain Component data for fast searching
// 
// The Components now only hold an index of an Asset and their ownder's entityID
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
		CAT_DEFAULT, // wtf
		CAT_SHADER,
		CAT_MODEL,
		CAT_CAMERA,
		CAT_AABB,
		CAT_TRANSFORM,
		CAT_LAST
	};

	enum EntityFlag
	{
		EF_ENABLED,
		EF_VISIBLE,
		EF_PHYSICAL,
		EF_PARTICLE,
		EF_LAST
	};

	class Entity
	{
	public:
		Entity();
		Entity(std::string Name);
		Entity(std::string Name, Entity& copy);
		
		int id = -1; // not used for looping, only to know the ID of the copied Entity in other Systems (useful for error logging)
		std::string name = "unnamed";
		std::vector<bool> flags; // resize to EF_LAST
		std::vector<std::vector<int>> assetIDs; // resize it to CAT_LAST
	};

	class Component
	{
	public:
		Component();
		Component(int EntityID, int AssetID);

		int entityID = -1;
		int assetID = -1;
	};

	class EntityController : public Publisher, public Subscriber
	{
	public:
		EntityController(); // Adds an initial Entity !

		int AddEntity(std::string EntityName);
		void RemoveEntity(std::string EntityName);
		void RemoveEntity(int EntityID);

		void EnableEntity(std::string EntityName); // Dont use if you can
		void EnableEntity(int EntityID);

		void DisableEntity(std::string EntityName); // Dont use if you can
		void DisableEntity(int EntityID);

		std::vector<Entity> GetEntities();
		Entity GetEntity(int EntityID);
		int GetEntityID(std::string EntityName);
		Entity GetEntity(std::string EntityName);
		std::vector<Component> GetComponents(ComponentAssetType Type);

		void AddComponent(int EntityID, int AssetID, ComponentAssetType Type, AssetManager* assetManager);
		void AddComponent(int EntityID, std::string AssetName, ComponentAssetType Type, AssetManager* assetManager);
		void AddComponent(std::string EntityName, std::string AssetName, ComponentAssetType Type, AssetManager* assetManager);

		void Update(AssetManager* assetManager);

	private:
		std::mutex ecsMutex;
		std::vector<Entity> entityList;
		std::vector<std::vector<Component>> components;
		Component defaultComponent = Component(-1, -1); // -1 is invalid index so this is a good default component i guess
	};

}