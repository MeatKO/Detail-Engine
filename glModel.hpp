#pragma once

#include "dMath.hpp"
#include <string>
#include <vector>
#include <fstream>

using namespace dMath;

namespace engine
{
	class Texture
	{

	};

	class Mesh
	{

	};

	class Model
	{
	public:
		Model(std::string path)
		{

		}

		bool LoadModel(std::string path)
		{

		}

	private:
		std::vector<Mesh> meshesList;
		std::vector<Texture> textureList;
	};
}