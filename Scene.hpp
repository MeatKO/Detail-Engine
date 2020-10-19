#pragma once

#include "ECS.hpp"
#include "glLights.hpp"
#include "glCubemap.hpp"
#include "glShader.hpp"
#include "glCamera.hpp"

namespace detailEngine
{
	enum SceneFlag
	{
		SF_FOCUSED,
		SF_ENABLED,
		SF_LAST
	};

	class Scene
	{
	public:
		Scene() {}
		Scene(std::string Name)
		{
			name = Name;
			flags.resize(SF_LAST);
		}

		bool AddEntity(Entity& entity)
		{
			for (int i = 0; i < entityList.size(); ++i)
			{
				if (entityList[i] == entity.id)
				{
					return false;
				}
			}
			
			entityList.push_back(entity.id);
			
			// We make components out of the asset ID list, Component include their owner ID and the asset ID
			// the owner ID of all the components in this case is the same 
			Component tempComponent(entity.id, -1);
			
			for (int i = 0; i < CAT_LAST; ++i)
			{
				for (int k = 0; k < entity.assetIDs[i].size(); ++k)
				{
					// Entity asset list
					tempComponent.assetID = entity.assetIDs[i][k];
					// Scene component list
					componentList[i].push_back(tempComponent);
				}
			}

			return true;
		}
		
		bool RemoveEntity(int entityID)
		{
			bool found = false;
			
			for (int i = 0; i < entityList.size(); ++i)
			{
				if (entityList[i] == entityID)
				{
					entityList.erase(entityList.begin() + i);
					found = true;
				}	
			}
			
			// Remove all the components as well
			if (found)
			{
				for (int i = 0; i < CAT_LAST; ++i)
				{
					for (int k = 0; k < componentList[i].size(); ++k)
					{
						if (componentList[i][k].entityID == entityID)
						{
							componentList[i].erase(componentList[i].begin() + k);
						}
					}
				}
			
				return true;
			}
		
			return false;
		}

		std::string name = "unnamed";
		std::string skybox = "detail"; // asset name

		int width = 0, height = 0;
		std::vector<int> entityList;
		std::vector<std::vector<Component>> componentList;

		Camera camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f));
		std::vector<bool> flags; // resize to SF_LAST
	};

	// dont forget to put it on the message bus
	class SceneManager : public Publisher, public Subscriber
	{
	public:
		SceneManager() {}

		bool AddEntity(EntityController* entityController, std::string sceneName, int entityID)
		{
			Scene* scene = GetScenePointer(sceneName);
			Entity entity = entityController->GetEntity(entityID);
		
			if (entity.id <= 0)
			{
				pSendMessage(Message(MSG_LOG, std::string("Scene Manager Error"),
					std::string("Entity '" + entity.name + "' [" + std::to_string(entity.id) + "] has an invalid ID. and can't be Added to Scene '" + sceneName + "'.")));
				return false;
			}
		
			if (scene)
			{
				std::lock_guard<std::mutex> mut(sceneMutex);

				if (scene->AddEntity(entity))
				{
					pSendMessage(Message(MSG_LOG, std::string("Scene Manager Error"),
						std::string("Entity '" + entity.name + "' [" + std::to_string(entity.id) + "] was Added to Scene '" + sceneName + "'.")));
					return true;
				}
		
				pSendMessage(Message(MSG_LOG, std::string("Scene Manager Error"),
					std::string("Entity '" + entity.name + "' [" + std::to_string(entity.id) + "] already exists in Scene '" + sceneName + "' and can't be Added.")));
				return false;
			}
		
			pSendMessage(Message(MSG_LOG, std::string("Scene Manager Error"),
				std::string("Can't Add Entity '" + entity.name + "' [" + std::to_string(entity.id) + "] to an unexisting Scene '" + sceneName + "'.")));
			return false;
		}

		bool RemoveEntity(EntityController* entityController, std::string sceneName, int entityID)
		{
			Scene* scene = GetScenePointer(sceneName);
			Entity entity = entityController->GetEntity(entityID);

			if (entity.id <= 0)
			{
				pSendMessage(Message(MSG_LOG, std::string("Scene Manager Error"),
					std::string("Entity '" + entity.name + "' [" + std::to_string(entity.id) + "] has an invalid ID and can't be Removed from Scene '" + sceneName + "'.")));
				return false;
			}

			if (scene)
			{
				std::lock_guard<std::mutex> mut(sceneMutex);

				if (scene->RemoveEntity(entity.id))
				{
					pSendMessage(Message(MSG_LOG, std::string("Scene Manager Error"),
						std::string("Entity '" + entity.name + "' [" + std::to_string(entity.id) + "] was Removed from Scene '" + sceneName + "'.")));
					return true;
				}

				pSendMessage(Message(MSG_LOG, std::string("Scene Manager Error"),
					std::string("Entity '" + entity.name + "' [" + std::to_string(entity.id) + "] doesn't exist in Scene '" + sceneName + "' and can't be Removed.")));
				return false;
			}

			pSendMessage(Message(MSG_LOG, std::string("Scene Manager Error"),
				std::string("Can't Remove Entity '" + entity.name + "' [" + std::to_string(entity.id) + "] to an unexisting Scene '" + sceneName + "'.")));
			return false;
		}

		bool AddScene(std::string sceneName)
		{
			if (!SceneExists(sceneName))
			{
				std::lock_guard<std::mutex> mut(sceneMutex);

				sceneList.push_back(Scene(sceneName));
				pSendMessage(Message(MSG_LOG, std::string("Scene Manager Info"), std::string("Scene '" + sceneName + "' was created successfully.")));
				return true;
			}
			else
			{
				pSendMessage(Message(MSG_LOG, std::string("Scene Manager Error"), std::string("Scene called '" + sceneName + "' already exists.")));
				return false;
			}
		}

		bool RemoveScene(std::string sceneName)
		{
			std::lock_guard<std::mutex> mut(sceneMutex);

			for (int i = 0; i < sceneList.size(); i++)
			{
				if (sceneList[i].name == sceneName)
				{
					sceneList.erase(sceneList.begin() + i);
					return true;
				}
			}

			return false;
		}

		Scene& GetSceneRef(std::string sceneName)
		{
			for (int i = 0; i < sceneList.size(); i++)
			{
				if (sceneList[i].name == sceneName)
					return sceneList[i];
			}

			return invalidScene;
		}

		Scene* GetScenePointer(std::string sceneName)
		{
			for (int i = 0; i < sceneList.size(); i++)
			{
				if (sceneList[i].name == sceneName)
					return &sceneList[i];
			}

			return nullptr;
		}

		Scene& GetSceneRef(int sceneID)
		{
			if (sceneID >= 0 && sceneID < sceneList.size())
			{
				return sceneList[sceneID];
			}

			return invalidScene;
		}

		Scene* GetScenePointer(int sceneID)
		{
			if (sceneID >= 0 && sceneID < sceneList.size())
			{
				return &sceneList[sceneID];
			}

			return nullptr;
		}

		bool SceneExists(std::string sceneName)
		{
			std::lock_guard<std::mutex> mut(sceneMutex);

			for (int i = 0; i < sceneList.size(); i++)
			{
				if (sceneList[i].name == sceneName)
					return true;
			}

			return false;
		}

		void UpdatePlayerCamera(float newX, float newY)
		{
			for (int i = 0; i < sceneList.size(); ++i)
			{
				if (sceneList[i].flags[SF_FOCUSED])
				{
					sceneList[i].camera.ProcessMouseMovement(newX, newY);
				}
			}
		}

		void ExecuteMessage(Message message)
		{
			if (message.GetTopic() == MSG_MOUSEDELTA)
			{
				int moveX = std::any_cast<float>(message.GetEvent());
				int moveY = std::any_cast<float>(message.GetValue());

				UpdatePlayerCamera(moveX, moveY);
			}
		}

		void Update(Input* input, double deltaTime)
		{
			for (int i = 0; i < sceneList.size(); ++i)
			{
				if (sceneList[i].flags[SF_FOCUSED])
				{
					sceneList[i].camera.ProcessKeyboardInput(input, (float)deltaTime);
				}
			}
		}

		std::vector<Scene> sceneList;
		std::mutex sceneMutex;
		Scene invalidScene = Scene("invalid");
	};

}