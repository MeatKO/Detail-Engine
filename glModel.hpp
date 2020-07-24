#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/quaternion.hpp"

#include <iostream>
#include <string>
#include <sstream>

#include "ECS.hpp"
#include "FileSystem.hpp"
#include "AssetManager.hpp"
#include "OpenGL.hpp"

using namespace glm;

namespace detailEngine
{
	std::vector<std::string> SplitString(std::string input, char divider);

	struct VTN
	{
		int data[3];
	};

	void StringToVTN(std::string faceString, VTN& vtn);
	
	struct Vertex
	{
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;

		int index[3];
	};

	struct Face
	{
		VTN vtn[3];
	};

	struct Material
	{
		std::string name;
		std::string map_kd;
		int map_kd_id;
	};

	struct MtlLib
	{
		std::string name;
		std::vector<Material> materials;
	};

	struct Mesh
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Face> faces;

		std::string name = "";
		std::string usedMaterial = "";

		unsigned int VAO, VBO, EBO;
	};

	class Model
	{
	public:

		Model(std::string Name) : name(Name) {}

		bool processed = false;

		std::string name = "";
		std::string mtlLib = "";

		std::vector<Mesh> meshes;
		std::vector<Material> materials;

		std::vector<vec3> vertices;
		std::vector<vec2> uvs;
		std::vector<vec3> normals;
	};

	void LoadObj(std::stringstream& file, Model& model);
	void ProcessObj(Model& model);
	void ProcessObjMaterials(std::stringstream& mtlFile, Model& model);
}