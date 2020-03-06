#pragma once

#include "dMath.hpp"
#include "glTexture.hpp"
#include "glShader.hpp"
#include <fstream>
#include <sstream>
#include <string>

using namespace dMath;

namespace detailEngine
{
	struct Vertex
	{
		dMath::vec3 Position;
		dMath::vec3 Normal;
		dMath::vec2 UV;
		dMath::vec3 Tangent;
		dMath::vec3 Bitangent;
		float meshID;

		bool operator == (Vertex vert)
		{
			if (this->Position.x == vert.Position.x && this->Position.y == vert.Position.y && this->Position.z == vert.Position.z)
			{
				if (this->Normal.x == vert.Normal.x && this->Normal.y == vert.Normal.y && this->Normal.z == vert.Normal.z)
				{
					if (this->UV.x == vert.UV.x && this->UV.y == vert.UV.y)
					{
						return true;
					}
				}
			}
			return false;
		}
	};

	struct Texture
	{
		unsigned int id;
		std::string type;
		std::string name;
	};

	struct Material
	{
		std::string name;
		float Ns, Ni, d, illum;
		vec3 Ka, Kd, Ks, Ke;
		std::vector<Texture> textureList;
		void LoadMtlTextures(std::string modelName)
		{
			for (Texture& tex : textureList)
			{
				if (tex.type == "map_Kd")
				{
					std::string path = "detail/models/" + modelName + "/" + tex.name;
					tex.id = LoadTexture(path);
				}
			}
		}
	};

	struct Mesh
	{
		unsigned int VAO, VBO, EBO;
		bool soft = true;
		std::string modelName;
		std::string name;
		std::string usedMaterial = "DEFAULT";
		std::vector<uint3> faces;
		
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		void Draw(Shader* shader, std::vector<Material> materials)
		{
			Material usedMtl;
			for (int i = 0; i < materials.size(); i++)
			{
				if (usedMaterial == materials[i].name)
				{
					usedMtl = materials[i];
				}
			}

			for (int i = 0; i < usedMtl.textureList.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i);
				glUniform1i(glGetUniformLocation((shader->Program), usedMtl.textureList[i].name.c_str()), i);
				glBindTexture(GL_TEXTURE_2D, usedMtl.textureList[i].id);
			}

			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
			glBindVertexArray(0);

			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, 0);
		}
		void SetupMesh()
		{
			glGenVertexArrays(1, &this->VAO);
			glGenBuffers(1, &this->VBO);
			glBindVertexArray(this->VAO);
			glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
			glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);
			glGenBuffers(1, &EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

			// Vertex Positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));

			// Vertex Normals
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

			// Vertex Texture Coordinates
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, UV));

			// vertex tangent
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

			// vertex bitangent
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

			// mesh ID
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, meshID));

			glBindVertexArray(0);
		}
	};

	class Model
	{
	public:
		Model(std::string name)
		{
			modelName = name;
			LoadOBJ(name); // Loads materials as well
			ProcessMeshes();
			for (int i = 0; i < meshes.size(); i++)
			{
				meshes[i].SetupMesh();
			}
			LoadMaterialTextures();
		}

		void Draw(Shader* shader)
		{
			int meshesSize = meshes.size();
			for (int i = 0; i < meshesSize; i++)
			{
				meshes[i].Draw(shader, materials);
			}
		}

		void LoadOBJ(std::string ModelName)
		{
			std::string filePath = "detail/models/" + ModelName + "/" + ModelName + ".obj";
			std::ifstream file(filePath);

			if (!file)
			{
				return;
			}

			std::vector<std::string> materialNames;

			meshes.push_back(Mesh()); // adding a default mesh in case the file contains data before a new object or a group

			std::string line;
			long lineCount = 0;
			int newMeshCount = 1;
			while (std::getline(file, line))
			{
				std::stringstream lineStream(line);
				std::string word;
				float x = 1, y = 1, z = 1;

				lineStream >> word;
				if (word == "#")
				{
					continue;
				}
				else if (word == "mtllib")
				{
					lineStream >> word;
					materialNames.push_back(word);
				}
				else if (word == "usemtl")
				{
					lineStream >> word;
					if (meshes.back().usedMaterial == "DEFAULT")
					{
						meshes.back().usedMaterial = word;
					}
					else
					{
						meshes.push_back(Mesh());
						meshes.back().usedMaterial = word;
						meshes.back().modelName = modelName;
						meshes.back().name = "newmesh" + newMeshCount;
						newMeshCount++;
					}
				}
				else if (word == "s")
				{
					lineStream >> word; // smooth shading
					if (word == "off" || word == "0")
					{
						meshes.back().soft = false;
					}
				}
				else if (word == "o" || word == "g") // new mesh
				{
					meshes.push_back(Mesh());
					lineStream >> word; // obj name
					meshes.back().name = word;
					meshes.back().modelName = modelName;
				}
				else if (word == "v")
				{
					lineStream >> x >> y >> z;
					loadVertices.push_back(vec3(x, y, z));
				}
				else if (word == "vt")
				{
					lineStream >> x >> y;
					loadUVs.push_back(vec2(x, y));
				}
				else if (word == "vn")
				{
					lineStream >> x >> y >> z;
					loadNormals.push_back(vec3(x, y, z));
				}
				else if (word == "f")
				{
					while (lineStream >> word)
					{
						// Initializing this to be 1,1,1 in case a parameter is missing while loading
						// If one of the face components is missing then its index will be 0 and the face wont pass the check later in ProcessMeshes()
						uint3 face(1);
						// Making sure i wont read nonsense if the model doesnt include UVs or normals
						if (sscanf(word.c_str(), "%u/%u/%u ", &face.x, &face.y, &face.z) != 3) // Normal model containing vertex uv and normal
						{
							if (sscanf(word.c_str(), "%u//%u ", &face.x, &face.z) != 2) // only vertex and normal
							{
								if (sscanf(word.c_str(), "%u/%u ", &face.x, &face.z) != 2) // only vertex and normal 
								{
									if (sscanf(word.c_str(), "%u// ", &face.x) != 1) // only vertex
									{
										if (sscanf(word.c_str(), "%u ", &face.x) != 1) // only vertex
										{
											// The line begins with "f" and contains no values
											continue;
										}
									}
								}
							}
						}

						meshes.back().faces.push_back(face);
					}
				}
			}

			// This doesnt follow the .mtl specification fix it 
			for (std::string mtlName : materialNames)
			{
				//std::cout << "Material " << mtlName << std::endl;
				std::string path = "detail/models/" + ModelName + "/" + mtlName;
				LoadOBJMtl(path);
			}

			file.close();
		}

		// This function turns the raw loadVertices, loadUVs and loadNormals into Vertex and combines the data from all the meshes
		void ProcessMeshes()
		{
			// Read the face data from all the meshes and create the Vertex vector
			// Starting from 1 because the first element is the default mesh and normally it would have nothing in it
			for (int i = 1; i < meshes.size(); i++)
			{
				int vertSize = loadVertices.size();
				int uvSize = loadUVs.size();
				int normalSize = loadNormals.size();

				meshes[i].vertices.resize(loadVertices.size());
				// The definition of 'face' might be a bit confusing here, a face here is defined as a  V/T/N pair thats why it was necessary to keep the count
				// of vertices per face
				for (uint3 face : meshes[i].faces)
				{
					// Making sure we wont read elements with negative index... and not read empty vectors as well
					if (face.x > 0 && face.y > 0 && face.z > 0)
					{
						Vertex newVert;
						if (vertSize > 0)
						{
							newVert.Position = loadVertices[face.x - 1];
						}
						if (uvSize > 0)
						{
							// Inverting the UV coordinates
							vec2 uv = loadUVs[face.y - 1];
							newVert.UV = vec2(uv.x, 1.0f - uv.y);
						}
						if (normalSize > 0)
						{
							newVert.Normal = loadNormals[face.z - 1];
						}
							
						newVert.meshID = i;

						// Sorting the vertices and indices the same way they were in the .obj file
						// This helps us optimize-out the repeating vertices later
						meshes[i].vertices[face.x - 1] = newVert;
						meshes[i].indices.push_back(face.x - 1);
					}
				}
			}
		}

		bool LoadOBJMtl(std::string absolutePath)
		{
			std::fstream materialFile(absolutePath);

			if (!materialFile)
			{
				return false;
			}
			float x, y, z;

			// Pushing a default material in case the file contains data before a "newmtl"
			materials.push_back(Material());

			std::string Line;
			while (std::getline(materialFile, Line))
			{
				std::stringstream lineStream(Line);
				std::string word;
				lineStream >> word;

				if (word == "#")
				{
					continue;
				}
				else if (word == "newmtl")
				{
					materials.push_back(Material());
					lineStream >> word;
					materials.back().name = word;
				}
				else if (word == "illum")
				{
					lineStream >> x;
					materials.back().illum = x;
				}
				else if (word == "Ns")
				{
					lineStream >> x;
					materials.back().Ns = x;
				}
				else if (word == "Ni")
				{
					lineStream >> x;
					materials.back().Ni = x;
				}
				else if (word == "d")
				{
					lineStream >> x;
					materials.back().d = x;
				}
				else if (word == "Ka")
				{
					lineStream >> x >> y >> z;
					materials.back().Ka = dMath::vec3(x, y, z);
				}
				else if (word == "Kd")
				{
					lineStream >> x >> y >> z;
					materials.back().Kd = dMath::vec3(x, y, z);
				}
				else if (word == "Ks")
				{
					lineStream >> x >> y >> z;
					materials.back().Ks = dMath::vec3(x, y, z);
				}
				else if (word == "Ke")
				{
					lineStream >> x >> y >> z;
					materials.back().Ke = dMath::vec3(x, y, z);
				}
				else if (word == "map_Ka" || word == "map_Kd" || word == "map_Ks" || word == "map_Ns" || word == "map_d" || word == "map_bump")
				{
					materials.back().textureList.push_back(Texture());

					materials.back().textureList.back().type = word;
					lineStream >> word;
					materials.back().textureList.back().name = word;
				}
			}

			materialFile.close();
			return true;
		}

		void LoadMaterialTextures()
		{
			//std::vector<Texture> textures;

			for (Material& mat : materials)
			{
				//or (Texture tex : mat.textureList)
				//
				//	std::cout << "texture : " << tex.name << " Type : " << tex.type << std::endl;
				//	if (tex.type == "map_Kd")
				//	{
				//		Texture newTex;
				//		newTex.name = tex.name;
				//		newTex.type = tex.type;
				//		textures.push_back(newTex);
				//	}
				//
				mat.LoadMtlTextures(modelName);
			}
		}

		std::string modelName;
		std::vector<vec3> loadVertices, loadNormals;
		std::vector<vec2> loadUVs;
		std::vector<Mesh> meshes;
		std::vector<Material> materials;
	};
	
}