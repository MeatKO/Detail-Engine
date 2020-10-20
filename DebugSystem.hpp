#pragma once

#include "ECS.hpp"
#include "AssetManager.hpp"

#include <GLFW/glfw3.h>

namespace detailEngine
{
	class DebugSystem : public Publisher, public Subscriber
	{
	public:
		DebugSystem() {}

		void TestMessageBus(int messageCount);

		void Update(Input* input, EntityController* entityController, AssetManager* assetManager)
		{
			if (entityController != nullptr)
			{
				//std::vector<std::vector<Component>>& components = entityController->GetAllComponents();
				//std::vector<Entity> entities = entityController->GetAllEntities();
				//std::vector<Asset> assets = assetManager->GetAllAssets();
				//
				//system("CLS");
				//for (Asset asset : assets)
				//{
				//	std::cout << "Asset  :  " << asset.name << std::endl;
				//}
				//for (Entity entity : entities)
				//{
				//	std::cout << "Entity :  " << entity.name << std::endl;
				//}

				//if (input->IsPressed(GLFW_KEY_T))
				//{
					//system("CLS");
					//
					//std::cout << "----------COMPONENTS----------\n";
					//
					//for (int i = 0; i < components.size(); i++)
					//{
					//	//std::cout << "Component Type " << i << "\n";
					//	std::cout << ComponentTypeName((ComponentAssetType)i) << " Component \n";
					//	for (int k = 0; k < components[i].size(); k++)
					//	{
					//		std::cout << "|-- Component's Entity ID : " << components[i][k].GetEntityID() << "\n";
					//	}
					//}
					//
					//std::cout << "----------ENTITIES-----------\n";
					//
					//for (int i = 0; i < entities.size(); i++)
					//{
					//	std::cout << "----------------------------------\n";
					//	std::cout << "Entity Name : " << entities[i].name << "\n";
					//	std::cout << "Entity Id   : " << entities[i].id << "\n";
					//	std::cout << "Components\n";
					//	for (int k = 0; k < entities[i].components.size(); k++)
					//	{
					//		if (entities[i].components[k].GetType() != 0)
					//		{
					//			std::cout << "|--" << ComponentTypeName(entities[i].components[k].GetType()) << "\n";
					//		}
					//
					//	}
					//}
				//}
			}
		}

		std::string ComponentTypeName(ComponentAssetType Type)
		{
			if (Type == CAT_DEFAULT)
				return "Default";
			if (Type == CAT_SHADER)
				return "Shader";
			if (Type == CAT_MODEL)
				return "Model";
			if (Type == CAT_CAMERA)
				return "Camera";

			return "";
		}
	};
}