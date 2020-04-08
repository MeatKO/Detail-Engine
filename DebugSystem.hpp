#pragma once

#include "ECS.hpp"
#include "dMath.hpp"
#include <GLFW/glfw3.h>

namespace detailEngine
{
	class DebugSystem
	{
	public:
		DebugSystem(EntityController* entityCtrl) { entityController = entityCtrl; }

		void Update(Input* input)
		{
			if (entityController != nullptr)
			{
				std::vector<std::vector<Component>>& components = entityController->GetAllComponents();
				std::vector<Entity>& entities = entityController->GetAllEntities();
				
				//if (input->IsPressed(GLFW_KEY_T))
				//{
					system("CLS");

					std::cout << "----------COMPONENTS----------\n";

					for (int i = 0; i < components.size(); i++)
					{
						//std::cout << "Component Type " << i << "\n";
						std::cout << ComponentTypeName((ComponentType)i) << " Component \n";
						for (int k = 0; k < components[i].size(); k++)
						{
							std::cout << "|-- Component's Entity ID : " << components[i][k].GetEntityID() << "\n";
						}
					}

					std::cout << "----------ENTITIES-----------\n";

					for (int i = 0; i < entities.size(); i++)
					{
						std::cout << "----------------------------------\n";
						std::cout << "Entity Name : " << entities[i].name << "\n";
						std::cout << "Entity Id   : " << entities[i].id << "\n";
						std::cout << "Components\n";
						for (int k = 0; k < entities[i].components.size(); k++)
						{
							if (entities[i].components[k].GetType() != 0)
							{
								std::cout << "|--" << ComponentTypeName(entities[i].components[k].GetType()) << "\n";
							}

						}
					}
				//}
			}
		}

		std::string ComponentTypeName(ComponentType Type)
		{
			if (Type == CT_DEFAULT)
				return "Default";
			if (Type == CT_DISABLED)
				return "Disabled";
			if (Type == CT_POSITION)
				return "Position";
			if (Type == CT_SHADER)
				return "Shader";
			if (Type == CT_MODEL)
				return "Model";
			if (Type == CT_CAMERA)
				return "Camera";

			return "";
		}

		EntityController* entityController;
	};
}