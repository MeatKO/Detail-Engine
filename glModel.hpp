#pragma once

#include "dMath.hpp"
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include "SOIL2/SOIL2.h"
#include "glShader.hpp"

using namespace dMath;

namespace detailEngine
{
	unsigned int LoadTexturePath(std::string directory);

	enum ModelType
	{
		MDL_OBJ
	};

	struct Vertex
	{
		dMath::vec3 Position;
		dMath::vec2 UV;
		dMath::vec3 Normal;
		dMath::vec3 Tangent;
		dMath::vec3 Bitangent;
	};

	struct Texture
	{
		unsigned int id;
		std::string type;
		std::string name;
	};

	struct Mesh
	{
		unsigned int VAO, VBO, EBO;

		std::string name;
		std::string usedMaterial;
		std::vector<Vertex> vertexList;
		bool soft = true;
		int vertPerFace = 0; 
		int facesCount = 0;

		std::vector<int> vertexIndices;
		std::vector<int> uvIndices;
		std::vector<int> normalIndices;
		std::vector<dMath::vec3> loadVertices;
		std::vector<dMath::vec2> loadUVs;
		std::vector<dMath::vec3> loadNormals;

		void SetupMesh()
		{
			glGenVertexArrays(1, &this->VAO);
			glGenBuffers(1, &this->VBO);
			glBindVertexArray(this->VAO);
			glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
			glBufferData(GL_ARRAY_BUFFER, this->vertexList.size() * sizeof(Vertex), &this->vertexList[0], GL_STATIC_DRAW);

			// Vertex Positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

			// Vertex Normals
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
			
			// Vertex Texture Coordinates
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, UV));
			
			// vertex tangent
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
			
			// vertex bitangent
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

			glBindVertexArray(0);
		}

		void BuildVertices()
		{
			for (int i = 0; i < vertexIndices.size(); i++)
			{
				Vertex newVert;
				newVert.Position = loadVertices[vertexIndices[i] - 1];
				dMath::vec2 uv = loadUVs[uvIndices[i] - 1];
				dMath::vec3 normal;
				newVert.UV = dMath::vec2(uv.x, 1 - uv.y);
				int vertexIndex = vertexIndices[i];
				// If smooth shading is enabled add all the normals of the faces this point is part of and normalize them
				//if (soft)
				//{
				//	for (int k = 0; k < vertexIndices.size(); k++)
				//	{
				//		if (vertexIndex == vertexIndices[k])
				//		{
				//			normal = normal + loadNormals[normalIndices[k] - 1];
				//		}
				//	}
				//
				//	newVert.Normal = normalize(normal);
				//}
				//else
				//{
				//	newVert.Normal = loadNormals[normalIndices[i] - 1];
				//}

				//newVert.Normal = loadNormals[normalIndices[i] - 1];
				newVert.Normal = vec3(1.0f);

				vertexList.push_back(newVert);
			}
		}

		void TriangulateFaces()
		{
			std::vector<int> newVertexIndices;
			std::vector<int> newUVIndices;
			std::vector<int> newNormalIndices;

			for (int i = 0; i < (vertexIndices.size() / 4); i++)
			{
				int index1 = vertexIndices[i * 4 + 0];
				int index2 = vertexIndices[i * 4 + 1];
				int index3 = vertexIndices[i * 4 + 2];
				int index4 = vertexIndices[i * 4 + 3];

				newVertexIndices.push_back(index1);
				newVertexIndices.push_back(index2);
				newVertexIndices.push_back(index3);

				newVertexIndices.push_back(index2);
				newVertexIndices.push_back(index3);
				newVertexIndices.push_back(index4);

				index1 = uvIndices[i * 4 + 0];
				index2 = uvIndices[i * 4 + 1];
				index3 = uvIndices[i * 4 + 2];
				index4 = uvIndices[i * 4 + 3];

				newUVIndices.push_back(index1);
				newUVIndices.push_back(index2);
				newUVIndices.push_back(index3);
				newUVIndices.push_back(index2);
				newUVIndices.push_back(index3);
				newUVIndices.push_back(index4);

				index1 = normalIndices[i * 4 + 0];
				index2 = normalIndices[i * 4 + 1];
				index3 = normalIndices[i * 4 + 2];
				index4 = normalIndices[i * 4 + 3];

				newNormalIndices.push_back(index1);
				newNormalIndices.push_back(index2);
				newNormalIndices.push_back(index3);
				newNormalIndices.push_back(index2);
				newNormalIndices.push_back(index3);
				newNormalIndices.push_back(index4);
			}

			vertexIndices = newVertexIndices;
			uvIndices = newUVIndices;
			normalIndices = newNormalIndices;
		}
	};

	struct Material
	{
		std::string name;
		float Ns, Ni, d, illum;
		dMath::vec3 Ka, Kd, Ks, Ke;
		std::vector<Texture> textureList;
	};

	class Model
	{
	public:
		Model(std::string path, ModelType type)
		{
			if (type == MDL_OBJ)
			{
				LoadOBJ(path);
			}
		}

		void Draw(Shader* shader)
		{
			Material usedMat;
			for (Mesh mesh : meshes)
			{
				for (Material mat : materials)
				{
					if (mat.name == mesh.usedMaterial)
					{
						usedMat = mat;
					}
				}

				for (int i = 0; i < usedMat.textureList.size(); i++)
				{
					glActiveTexture(GL_TEXTURE0 + i);
				
					glUniform1i(glGetUniformLocation((shader->Program), usedMat.textureList[i].name.c_str()), i);
					glBindTexture(GL_TEXTURE_2D, usedMat.textureList[i].id);
				}

				glBindVertexArray(mesh.VAO);
				glDrawArrays(GL_TRIANGLES, 0, mesh.vertexList.size());
				glBindVertexArray(0);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}

		bool LoadOBJ(std::string path)
		{
			std::string filePath = "detail/models/" + path + "/" + path + ".obj";
			std::ifstream file(filePath);

			if (!file)
			{
				std::cout << "File doesn't exist" << std::endl;
				return false;
			}

			std::vector<Mesh> tempMeshes;
			std::vector<std::string> materialNames;
			std::string Line;

			while (std::getline(file, Line))
			{
				std::stringstream lineStream(Line);
				std::string word;
				lineStream >> word;
				
				float x, y, z;

				if (word == "#")
				{
					continue;
				}
				else if (word == "mtllib")
				{
					lineStream >> word; // mtllib name
					materialNames.push_back(word);
				}
				else if (word == "usemtl")
				{
					lineStream >> word; // usemtl name
					tempMeshes.back().usedMaterial = word;
				}
				else if (word == "s")
				{
					lineStream >> word; // smooth shading
					if (word == "off" || word == "0")
					{
						tempMeshes.back().soft = false;
					}
				}
				else if (word == "o") // new object
				{
					tempMeshes.push_back(Mesh());
					lineStream >> word; // obj name
					tempMeshes.back().name = word;
				}
				else if (word == "v")
				{
					lineStream >> x >> y >> z;
					tempMeshes.back().loadVertices.push_back(vec3(x, y, z));
				}
				else if (word == "vt")
				{
					lineStream >> x >> y;
					tempMeshes.back().loadUVs.push_back(vec2(x, y));
				}
				else if (word == "vn")
				{
					lineStream >> x >> y >> z;
					tempMeshes.back().loadNormals.push_back(vec3(x, y, z));
				}
				else if (word == "f")
				{
					tempMeshes.back().facesCount++;
					while (lineStream >> word)
					{
						// Making sure i wont read nonsense if the model doesnt include UVs or normals
						if (sscanf(word.c_str(), "%f/%f/%f ", &x, &y, &z) != 3)
						{
							if (sscanf(word.c_str(), "%f//%f ", &x, &z) != 2)
							{
								if (sscanf(word.c_str(), "%f/%f ", &x, &z) != 2)
								{
									if (sscanf(word.c_str(), "%f// ", &x) != 1)
									{
										if (sscanf(word.c_str(), "%f ", &x) != 1)
										{
											std::cout << "Invalid obj model. Tried to read face value for '" << word << "'" << std::endl;
											return false;
										}
									}
								}
							}
						}
						tempMeshes.back().vertexIndices.push_back(x);
						tempMeshes.back().uvIndices.push_back(y);
						tempMeshes.back().normalIndices.push_back(z);
					}
				}
			}

			for (Mesh& mesh : tempMeshes)
			{
				std::cout << mesh.name << std::endl;
				if ((mesh.vertexIndices.size() / mesh.facesCount) == 4)
				{
					mesh.TriangulateFaces();
				}
				mesh.BuildVertices();
				mesh.SetupMesh();
			}

			for (std::string materialName : materialNames)
			{
				std::cout << materialName << std::endl;
				LoadOBJMtl("detail/models/" + path + "/" + materialName);
			}

			for (Material& material : materials)
			{
				for (Texture& tex : material.textureList)
				{
					tex.id = LoadTexturePath("detail/models/" + path + "/" + tex.name);
				}
			}

			meshes = tempMeshes;

			file.close();
			return true;
		}

		bool LoadOBJMtl(std::string absolutePath)
		{
			std::fstream materialFile(absolutePath);
		
			if (!materialFile)
			{
				std::cout << "Material '" + absolutePath + "' doesn't exist" << std::endl;
				return false;
			}
		
			Material material;
			float x, y, z;
		
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
					lineStream >> word;
					material.name = word;
				}
				else if (word == "illum")
				{
					lineStream >> x;
					material.illum = x;
				}
				else if (word == "Ns" )
				{
					lineStream >> x;
					material.Ns = x;
				}
				else if (word == "Ni")
				{
					lineStream >> x;
					material.Ni = x;
				}
				else if (word == "d")
				{
					lineStream >> x;
					material.d = x;
				}
				else if (word == "Ka")
				{
					lineStream >> x >> y >> z;
					material.Ka = dMath::vec3(x, y, z);
				}
				else if (word == "Kd") 
				{
					lineStream >> x >> y >> z;
					material.Kd = dMath::vec3(x, y, z);
				}
				else if (word == "Ks")
				{
					lineStream >> x >> y >> z;
					material.Ks = dMath::vec3(x, y, z);
				}
				else if (word == "Ke") 
				{
					lineStream >> x >> y >> z;
					material.Ke = dMath::vec3(x, y, z);
				}
				else if (word == "map_Ka" || word == "map_Kd" || word == "map_Ks" || word == "map_Ns" || word == "map_d" || word == "map_bump") 
				{
					material.textureList.push_back(Texture());

					material.textureList.back().type = word;
					lineStream >> word;
					material.textureList.back().name = word;
				}
			}
		
			materialFile.close();
			materials.push_back(material);
			return true;
		}

     private:
        std::vector<Mesh> meshes;
        std::vector<Material> materials;
		std::vector<Texture> textures;
	};

	unsigned int LoadTexturePath(std::string directory)
	{
		GLuint textureID;
		glGenTextures(1, &textureID);
	
		GLint width, height, nrComponents;
		unsigned char* data = SOIL_load_image(directory.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format = 0;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;
	
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
	
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << directory << std::endl;
		}

		SOIL_free_image_data(data);
	
		return textureID;
	}
}