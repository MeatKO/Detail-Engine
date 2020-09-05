#pragma once

#include <iostream>
#include <string>
#include <sstream>

#include "ECS.hpp"
#include "dMath.hpp"

/*
				Model - vector of all vertices, uvs and normals
				/	\
			Mesh	Mesh - vector of faces
			/			\
		Material		Material - contains shading values and texture names

*/

namespace detailEngine
{
	enum TextureMap
	{
		MAP_DIFFUSE,
		MAP_ALBEDO,
		MAP_NORMAL,
		MAP_ROUGHNESS,
		MAP_BUMP,
		MAP_LIGHT,
		MAP_SPECULAR,
		MAP_METALLIC,
		MAP_HEIGHT,
		MAP_DISPLACEMENT,
		MAP_CUBE,
		MAP_OPACITY,
		MAP_EMISSIVE,
		MAP_LAST
	};

	struct fvec2
	{
		fvec2(float X, float Y) : x(X), y(Y) {}
		fvec2(float A) : x(A), y(A) {}
		float x = 0.0f, y = 0.0f;
	};

	struct fvec3
	{
		fvec3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
		fvec3(float A) : x(A), y(A), z(A) {}
		float x = 0.0f, y = 0.0f, z = 0.0f;
	};

	struct Texture
	{
		std::string textureName = "default";
		unsigned int id = 0; // Texture API id
	};

	struct Material
	{
		std::string materialName = "";
		std::vector<int> textureList = std::vector<int>(MAP_LAST, -1); // Texture asset id
		fvec3 Ka = fvec3(1.0f);
		fvec3 Kd = fvec3(1.0f);
		fvec3 Ks = fvec3(0.0f);
		float Ns = 0.0f;
	};

	struct Mesh
	{
		// even more
	};

	class Model
	{
	public:
		Model() {}


	};

	// getting over my head on this one
}