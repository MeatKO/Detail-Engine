#pragma once

#include "ECS.hpp"
#include "glLights.hpp"
#include "glCubemap.hpp"
#include "glShader.hpp"

/*
The scene is meant to define a list of active game compoments.
You can switch between scenes and modify them by adding and removing entities, lights, cubemaps etc.
Should be a nice way of organizing the assets.

The entities and the shaders are referenced by name.
The lights and cubemaps are held by the Scene class.
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

		std::vector<std::string> entityList;
		std::vector<std::string> shadersList;
		std::vector<Light> lightsList;
		std::vector<CubemapEnvDynamic> dynamicCubemapList; // add positions later ?
		std::vector<CubemapEnvStatic> staticCubemapList;
	};
}