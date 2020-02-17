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
	struct Face;
	struct Vertex;
	struct Texture;
	struct Material;
	struct Mesh;
	class Model;

	enum ModelType;
	enum FaceType;

	void LoadObj(std::string modelName, std::vector<Mesh>& meshes, std::vector<Material>& materials);
	void TriangulateFaces(std::vector<unsigned int>& vertexIndices, std::vector<unsigned int>& uvIndices, std::vector<unsigned int>& normalIndices, FaceType Type);
	bool LoadOBJMtl(std::string absolutePath, Material& material);

	enum ModelType
	{
		MDL_OBJ
	};

	// Faces can have Vertices (V), UVs (T) and Normals (N)
	// Sometimes the model files don't contain UVs or Normals so knowing the type of loaded information is necessary while processing the mesh geometry
	enum FaceType
	{
		VTN,
		VT,
		VN,
		V
	};

	struct Vertex
	{
		dMath::vec3 Position;
		dMath::vec3 Normal;
		dMath::vec2 UV;
		dMath::vec3 Tangent;
		dMath::vec3 Bitangent;

		bool operator == (Vertex vert)
		{
			if (!(this->Position.x == vert.Position.x && this->Position.y != vert.Position.y && this->Position.z != vert.Position.z))
			{
				return false;
			}
			if (!(this->Normal.x == vert.Normal.x && this->Normal.y != vert.Normal.y && this->Normal.z != vert.Normal.z))
			{
				return false;
			}
			if (!(this->UV.x == vert.UV.x && this->UV.y != vert.UV.y))
			{
				return false;
			}
			return true;
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
	};

	struct Mesh
	{
		unsigned int VAO, VBO, EBO;

		bool soft = true;
		int vertPerFace = 0;
		int facesCount = 0;
		FaceType type;

		std::string name;
		std::string usedMaterial;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<unsigned int> vertexIndices;
		std::vector<unsigned int> uvIndices;
		std::vector<unsigned int> normalIndices;
		std::vector<dMath::vec3> loadVertices;
		std::vector<dMath::vec2> loadUVs;
		std::vector<dMath::vec3> loadNormals;

		void BuildVertices()
		{
			for (vec3 outVec : loadNormals)
			{
				//std::cout << outVec << "\n";
			}
			for (int i = 0; i < vertexIndices.size(); i++)
			{
				Vertex newVert;
				newVert.Position = loadVertices[vertexIndices[i] - 1];
				dMath::vec2 uv = loadUVs[uvIndices[i] - 1];
				dMath::vec3 normal;
				newVert.UV = dMath::vec2(uv.x, 1 - uv.y);
				int vertexIndex = vertexIndices[i];
				newVert.Normal = loadNormals[normalIndices[i] - 1];

				vertices.push_back(newVert);
			}
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
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(unsigned int), &vertexIndices[0], GL_STATIC_DRAW);

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

			glBindVertexArray(0);
		}
	};

	class Model
	{
	public:
		Model(std::string modelName, ModelType Type) : type(Type)
		{
			if (Type == MDL_OBJ)
			{
				LoadObj(modelName, meshes, materials);
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
				
				//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
				//glDrawElements(GL_TRIANGLES, mesh.vertexIndices.size(),GL_UNSIGNED_INT, (void*)0);

				glBindVertexArray(mesh.VAO);
				glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size());
				glBindVertexArray(0);

				//std::cout << mesh.vertexIndices.size() << std::endl;

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}

	private:
		ModelType type;
		std::vector<Mesh> meshes;
		std::vector<Material> materials;
	};

	// The vertex indices vector contains values above 0, so we have to substract 1 to access the vertex vector
	// The output index vector contains values directly for OpenGL ( starting from 0 ) 
	void IndexVertices(std::vector<Vertex>& vertices, std::vector<unsigned int>& vertexIndices)
	{
		std::vector<unsigned int> outIndices;
		std::vector<Vertex> outVertices;

		// Make a copy of the vertices vector so we can keep the original data while removing duplicate vertices
		outVertices = vertices;

		// Removing duplicates from the vertex vector
		for (int i = 0; i < outVertices.size(); i++)
		{
			for (int k = 0; k < outVertices.size(); k++)
			{
				// Make sure we're not deleting the Vertex if its matching its own element id...
				if (outVertices[i] == outVertices[k] && i != k)
				{
					outVertices.erase(outVertices.begin() + k);
				}
			}
		}


	}

	void LoadObj(std::string modelName, std::vector<Mesh>& meshes, std::vector<Material>& materials)
	{
		std::string filePath = "detail/models/" + modelName + "/" + modelName + ".obj";
		std::ifstream file(filePath);

		if (!file)
		{
			return;
		}

		//std::vector<Mesh> tempMeshes;
		std::vector<std::string> materialNames;
		std::string Line;

		while (std::getline(file, Line))
		{
			std::stringstream lineStream(Line);
			std::string word;
			lineStream >> word;

			// These will be used for indexing as well so if one of them is 0 then we will read elements below 0
			float x = 1, y = 1, z = 1; 

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
				meshes.back().usedMaterial = word;
			}
			else if (word == "s")
			{
				lineStream >> word; // smooth shading
				if (word == "off" || word == "0")
				{
					meshes.back().soft = false;
				}
			}
			else if (word == "o") // new mesh
			{
				meshes.push_back(Mesh());
				lineStream >> word; // obj name
				meshes.back().name = word;
			}
			else if (word == "v")
			{
				lineStream >> x >> y >> z;
				meshes.back().loadVertices.push_back(vec3(x, y, z));
			}
			else if (word == "vt")
			{
				lineStream >> x >> y;
				meshes.back().loadUVs.push_back(vec2(x, y));
			}
			else if (word == "vn")
			{
				lineStream >> x >> y >> z;
				meshes.back().loadNormals.push_back(vec3(x, y, z));
			}
			else if (word == "f")
			{
				meshes.back().facesCount++;
				while (lineStream >> word)
				{
					// Making sure i wont read nonsense if the model doesnt include UVs or normals
					if (sscanf(word.c_str(), "%f/%f/%f ", &x, &y, &z) != 3) // Normal model containing vertex uv and normal
					{
						if (sscanf(word.c_str(), "%f//%f ", &x, &z) != 2) // only vertex and normal
						{
							if (sscanf(word.c_str(), "%f/%f ", &x, &z) != 2) // only vertex and normal 
							{
								if (sscanf(word.c_str(), "%f// ", &x) != 1) // only vertex
								{
									if (sscanf(word.c_str(), "%f ", &x) != 1) // only vertex
									{
										// The line begins with "f" and contains no values
										return;
									}
									else
										meshes.back().type = V;
								}
								else
									meshes.back().type = V;
							}
							else
								meshes.back().type = VN;
						}
						else
							meshes.back().type = VN;
					}
					else
						meshes.back().type = VTN;

					meshes.back().vertexIndices.push_back(x);
					meshes.back().uvIndices.push_back(y);
					meshes.back().normalIndices.push_back(z);
				}
			}
		}

		for (Mesh& mesh : meshes)
		{
			std::cout << mesh.name << std::endl;
			mesh.vertPerFace = (mesh.vertexIndices.size() / mesh.facesCount);
			if (mesh.vertPerFace == 4)
			{
				TriangulateFaces(mesh.vertexIndices, mesh.uvIndices, mesh.normalIndices, mesh.type);
			}
			mesh.BuildVertices();
			mesh.SetupMesh();

			//for (vec3 norm : mesh.loadNormals)
			//{
			//	std::cout << "N : "<< norm << "\n";
			//}
		}
		
		for (std::string materialName : materialNames)
		{
			Material newMat;
			if (LoadOBJMtl("detail/models/" + modelName + "/" + materialName, newMat))
			{
				materials.push_back(newMat);
			}
		}
		
		for (Material& material : materials)
		{
			for (Texture& tex : material.textureList)
			{
				tex.id = LoadTexturePath("detail/models/" + modelName + "/" + tex.name);
			}
		}

		file.close();
	}

	bool LoadOBJMtl(std::string absolutePath, Material& material)
	{
		std::fstream materialFile(absolutePath);

		if (!materialFile)
		{
			return false;
		}
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
			else if (word == "Ns")
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
		return true;
	}

	// This function only works if the faces have 4 vertices its not a general purpose triangulation algorithm of any kind 
	// All its doing is just dumb index rearraging 
	void TriangulateFaces(std::vector<unsigned int>& vertexIndices, std::vector<unsigned int>& uvIndices, std::vector<unsigned int>& normalIndices, FaceType Type)
	{
		std::vector<unsigned int> newVertexIndices;
		std::vector<unsigned int> newUVIndices;
		std::vector<unsigned int> newNormalIndices;

		// There are 4 loops because i didn't want to assume that every vertex index is accompanied by uv and normal indices as well
		for (int i = 0; i < (vertexIndices.size() / 4); i++)
		{
			int index = i * 4 + 0;
			int index1 = vertexIndices[index];     int index2 = vertexIndices[index + 1];
			int index3 = vertexIndices[index + 2]; int index4 = vertexIndices[index + 3];
			newVertexIndices.push_back(index1); newVertexIndices.push_back(index2); newVertexIndices.push_back(index3);
			newVertexIndices.push_back(index4); newVertexIndices.push_back(index1); newVertexIndices.push_back(index3);
		}

		if (Type == VT || Type == VTN)
		{
			for (int i = 0; i < (uvIndices.size() / 4); i++)
			{
				int index = i * 4 + 0;
				int index1 = uvIndices[index];     int index2 = uvIndices[index + 1];
				int index3 = uvIndices[index + 2]; int index4 = uvIndices[index + 3];
				newUVIndices.push_back(index1); newUVIndices.push_back(index2); newUVIndices.push_back(index3);
				newUVIndices.push_back(index4); newUVIndices.push_back(index1); newUVIndices.push_back(index3);
			}
		}

		if (Type == VN || Type == VTN)
		{
			for (int i = 0; i < (normalIndices.size() / 4); i++)
			{
				int index = i * 4 + 0;
				int index1 = normalIndices[index];     int index2 = normalIndices[index + 1];
				int index3 = normalIndices[index + 2]; int index4 = normalIndices[index + 3];
				newNormalIndices.push_back(index1); newNormalIndices.push_back(index2); newNormalIndices.push_back(index3);
				newNormalIndices.push_back(index4); newNormalIndices.push_back(index1); newNormalIndices.push_back(index3);
			}
		}

		vertexIndices = newVertexIndices;
		uvIndices = newUVIndices;
		normalIndices = newNormalIndices;
	}
}