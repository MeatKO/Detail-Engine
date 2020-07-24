#include "glModel.hpp"

namespace detailEngine
{
	std::vector<std::string> SplitString(std::string input, char divider)
	{
		std::vector<std::string> strings;
		std::string currentString = "";

		for (int i = 0; i < input.size(); ++i)
		{
			if (input[i] == divider)
			{
				strings.push_back(currentString);
				currentString = "";
			}
			else
			{
				currentString += input[i];
			}
		}

		if (currentString.size() > 0)
			strings.push_back(currentString);

		return strings;
	}

	void StringToVTN(std::string faceString, VTN& vtn)
	{
		std::vector<std::string> facePairs = SplitString(faceString, '/');
		vtn.data[0] = std::stoi(facePairs[0]);
		vtn.data[1] = std::stoi(facePairs[1]);
		vtn.data[2] = std::stoi(facePairs[2]);
	}

	void LoadObj(std::stringstream& file, Model& model)
	{
		std::string line;
		std::string word;

		vec3 tempLoad(0.0f);

		Mesh tempMesh;

		while (std::getline(file, line))
		{
			std::stringstream lineStream(line);

			lineStream >> word;

			if (word == "#")
			{
				continue;
			}
			else if (word == "v")
			{
				lineStream >> tempLoad[0] >> tempLoad[1] >> tempLoad[2];
				model.vertices.push_back(tempLoad);
			}
			else if (word == "vt")
			{
				// Note : UVs have only 2 variables but i dont want to make 200 temp arrays and whatever
				lineStream >> tempLoad[0] >> tempLoad[1] >> tempLoad[2];
				model.uvs.push_back(glm::vec2(tempLoad[0], tempLoad[1]));
			}
			else if (word == "vn")
			{
				lineStream >> tempLoad[0] >> tempLoad[1] >> tempLoad[2];
				model.normals.push_back(tempLoad);
			}
			else if (word == "f")
			{
				Face face;

				for (int i = 0; i < 3; ++i)
				{
					lineStream >> word;
					VTN vtn;
					StringToVTN(word, vtn);
					face.vtn[i] = vtn;

				}

				model.meshes.back().faces.push_back(face);
			}
			else if (word == "o" || word == "g")
			{
				lineStream >> tempMesh.name; // mesh name

				model.meshes.push_back(tempMesh);
			}
			else if (word == "usedmtl")
			{
				model.meshes.back().usedMaterial = word;
			}
			else if (word == "mtllib")
			{
				lineStream >> word;
				model.mtlLib = word;
			}
		}
	}
	void ProcessObj(Model& model)
	{
		for (Mesh& mesh : model.meshes)
		{
			for (Face& face : mesh.faces)
			{
				for (int i = 0; i < 3; ++i)
				{
					Vertex vertex;
					// -1 because wavefront obj starts from 1 and not 0
					vertex.position = model.vertices[face.vtn[i].data[0] - 1];
					vertex.uv = model.uvs[face.vtn[i].data[1] - 1];
					vertex.normal = model.normals[face.vtn[i].data[2] - 1];

					mesh.vertices.push_back(vertex);
					mesh.indices.push_back(face.vtn[i].data[0]); // incorrect way of indexing...
				}
			}
		}

		// the loaded vertices, UVs and normals are no more needed

		model.vertices.clear();
		model.uvs.clear();
		model.normals.clear();
	}

	void ProcessObjMaterials(std::stringstream& mtlFile, Model& model)
	{
		std::string line;
		std::string word;
		
		while (std::getline(mtlFile, line))
		{
			std::stringstream lineStream(line);
		
			lineStream >> word;
			
			if (word == "#")
			{
				continue;
			}
			else if (word == "newmtl")
			{
				model.materials.push_back(Material());
				lineStream >> word;
				model.materials.back().name = word;
			}
			else if (word == "map_Kd")
			{
				lineStream >> word;
				model.materials.back().map_kd = word;
			}
		}
	}
}
