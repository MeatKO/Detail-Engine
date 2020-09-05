#pragma once

#include "ECS.hpp"
#include "glLights.hpp"
#include "glCubemap.hpp"
#include "glShader.hpp"
#include "glCamera.hpp"

namespace detailEngine
{
	class Scene
	{
	public:
		Scene(std::string Name)
		{
			name = Name;
		}

		std::string name = "unnamed";
		std::string skybox = "detail"; // asset name 

		std::vector<int> entityList;
		std::vector<int> shadersList;
		std::vector<int> lightsList;
		std::vector<int> dynamicCubemapList; // add positions later ?
		std::vector<int> staticCubemapList;
		Camera camera = Camera();
	};

	// dont forget to put it on the message bus
	class SceneManager : public Publisher, public Subscriber
	{
	public:
		SceneManager() {}

		void AddScene(std::string sceneName)
		{
			sceneList.push_back(Scene(sceneName));
		}

		void RemoveScene(std::string sceneName)
		{
			for (int i = 0; i < sceneList.size(); i++)
			{
				if (sceneList[i].name == sceneName)
				{
					sceneList.erase(sceneList.begin() + i);
				}
			}
		}

		void ExecuteMessage(Message message)
		{
			//if (message.GetTopic() == MSG_SCENE)
			//{
			//	
			//}
		}

		void Update()
		{

		}

		std::vector<Scene> sceneList;
	};

}