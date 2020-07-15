#pragma once

#include "ECS.hpp"
#include "OpenGL.hpp"

using namespace glm;

namespace detailEngine
{
	enum ModelType
	{
		MDL_OBJ
	};

	// The material holds only the texture name, the texture can be found in the asset manager
	struct Material
	{
		Material(std::string TextureName) : textureName(TextureName) {}
		std::string textureName = "default";
	};

	struct Mesh
	{
		Material mat;
		unsigned int VAO, VBO, EBO;
	};

	// upon initialization, the Model will only hold the name and the type of the file it needs to load
	// the Asset Manager takes care of the rest - when the needed model file is loaded, it is passed to the loadObj function as a stringstream and worked upon
	// after this, the Asset Manager fills in the Mesh vector inside the Model object and continues to work on the Materials the Meshes have
 
	class Model
	{
	public:
		Model() {}
		Model(std::string ModelName, ModelType ModelType)
		{
			modelName = ModelName;
			type = ModelType;
		}

		std::string modelName = "";
		ModelType type;

		std::vector<Mesh> meshesList;

	private:
	};

	// streams have no copy constructor...
	std::vector<Mesh> loadObj(std::stringstream& file)
	{

	}
}