#pragma once

/*

OBJ : 
V - vertex, 3 component vector representing only a position
VN - normal, 3 component vector representing a normal attached to a vertex
VT - UV, 2 component vector used for texture mapping
F - Face, 4 integers (hopefully 4 and potentially all positive, but there are all type of idiotic implementationsi cant even WHY DO PEOPLE USE QUADS ITS NOT 1995)
usemtl - Property that shows the name of the used material for the current Mesh
mtllib - Name of a material library

support for faces with more than 3 vertices can be added later

The OpenGL VAO VBO EBO initialization functions are located in OpenGL.hpp

*/

// #include "OpenGL.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/quaternion.hpp"

#include <iostream>
#include <string>
#include <sstream>

#include "ECS.hpp"

using namespace glm;

namespace detailEngine
{
	enum ModelType
	{
		MDL_OBJ
	};

	struct Vertex
	{
		vec3 Position;
		vec3 Normal;
		vec2 UV;
	};

	struct Texture
	{
		int id = -1;
		std::string data = "";
		bool init = false;
	};

	// The material holds only the texture name, the texture can be found in the asset manager
	struct Material
	{
		Material() {}
		Material(std::string TextureName) : textureName(TextureName) {}
		std::string textureName = "default";
	};

	struct VTN
	{
		int data[3];
	};

	struct Face
	{
		VTN vtn[3];
	};

	std::vector<std::string> SplitString(std::string input, char divider);

	// used to turn pairs of Vertex / UV / Normal into numbers
	void StringToVTN(std::string faceString, VTN& vtn);
	

	struct Mesh
	{
		Mesh() {}
		Material mat;
		std::string name;
		unsigned int VAO, VBO, EBO;
		std::vector<Face> faces; // only triangles ! 
		std::vector<Vertex> vertices;
		std::vector<int> indices; // start from 1 for API compatibility
	};

	class Model
	{
	public:
		Model() {}
		Model(std::string ModelName, ModelType ModelType)
			: modelName(ModelName), type(ModelType) {}

		bool init = false;

		std::string modelName = "";
		ModelType type;

		std::vector<Mesh> meshes;

		std::vector<vec3> loadedVertices;
		std::vector<vec3> loadedUVs;
		std::vector<vec3> loadedNormals;

	};

	void loadObj(std::stringstream& file, Model& model);
	void ProcessObj(Model& model);
}