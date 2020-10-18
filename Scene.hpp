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
		Scene(std::string Name)
		{
			name = Name;
			flags.resize(SF_LAST);
		}

		std::string name = "unnamed";
		std::string skybox = "detail"; // asset name 

		int width = 0, height = 0;
		std::vector<int> entityList;
		std::vector<std::vector<Component>> componentList;

		//std::vector<int> shadersList;
		//std::vector<int> lightsList;
		//std::vector<int> dynamicCubemapList; // add positions later ?
		//std::vector<int> staticCubemapList;
		Camera camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f));
		std::vector<bool> flags; // resize to SF_LAST
	};

	// dont forget to put it on the message bus
	class SceneManager : public Publisher, public Subscriber
	{
	public:
		SceneManager() {}

		void AddScene(std::string sceneName)
		{
			if (!SceneExists(sceneName))
			{
				std::lock_guard<std::mutex> mut(sceneMutex);

				sceneList.push_back(Scene(sceneName));
				pSendMessage(Message(MSG_LOG, std::string("Scene Manager Info"), std::string("Scene '" + sceneName + "' was created successfully.")));
			}
			else
			{
				pSendMessage(Message(MSG_LOG, std::string("Scene Manager Error"), std::string("Scene called '" + sceneName + "' already exists.")));
			}
		}

		void RemoveScene(std::string sceneName)
		{
			std::lock_guard<std::mutex> mut(sceneMutex);

			for (int i = 0; i < sceneList.size(); i++)
			{
				if (sceneList[i].name == sceneName)
				{
					sceneList.erase(sceneList.begin() + i);
				}
			}
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

		Scene& GetSceneRef(int sceneID)
		{
			if (sceneID >= 0 && sceneID < sceneList.size())
			{
				return sceneList[sceneID];
			}

			return invalidScene;
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
					//if (mouseInit)
					//{
					//	mouseLastX = xPos;
					//	mouseLastY = yPos;
					//
					//	mouseInit = false;
					//}
					//
					//float xOffset = xPos - mouseLastX;
					//float yOffset = mouseLastY - yPos;
					//
					//mouseLastX = xPos;
					//mouseLastY = yPos;

					sceneList[i].camera.ProcessMouseMovement(newX, newY);
				}
			}
		}

		void ExecuteMessage(Message message)
		{
			if (message.GetTopic() == MSG_MOUSEDELTA)
			{
				//Sleep(1);
				int moveX = std::any_cast<float>(message.GetEvent());
				int moveY = std::any_cast<float>(message.GetValue());

				UpdatePlayerCamera(moveX, moveY);
			}
		}

		void Update()
		{
			// uhh... code ?
			//pSendMessage(Message(MSG_LOG, std::string("Scene Manager Error"), std::string("Scene called '' already exists.")));
		}

		std::vector<Scene> sceneList;
		std::mutex sceneMutex;
		Scene invalidScene = Scene("invalid");
	};

}