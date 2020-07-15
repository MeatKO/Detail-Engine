#pragma once

#include "ECS.hpp"
#include "glLights.hpp"
#include "glCubemap.hpp"
#include "glShader.hpp"
#include "glCamera.hpp"

/*
The scene is meant to define a list of active game compoments.
You can switch between scenes and modify them by adding and removing entities, lights, cubemaps etc.
Should be a nice way of organizing the assets.

The entities and the shaders are referenced by name.
The lights and cubemaps are held by the Scene class.

this WILL crash if you init a scene before init-ing an opengl context
must run on the same thread as the renderer because of it...
*/

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

		std::vector<std::string> entityList;
		std::vector<std::string> shadersList;
		std::vector<Light> lightsList;
		std::vector<CubemapEnvDynamic> dynamicCubemapList; // add positions later ?
		std::vector<CubemapEnvStatic> staticCubemapList;
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