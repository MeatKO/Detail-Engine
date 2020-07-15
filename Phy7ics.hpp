#pragma once

#include "ECS.hpp"
#include "AssetManager.hpp"
#include "Transformation.hpp"

using namespace glm;

namespace detailEngine
{
	struct AABB
	{
		vec3 dimensions = vec3(1.0f);
		vec3 color = vec3(1.0f);
		unsigned int VAO = 0, VBO = 0, EBO = 0;
		bool initialized = false;
		float lines[72]
		{
			1.0f, 1.0f, 1.0f,   -1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,   -1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,   1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,   1.0f, 1.0f, 1.0f,

			1.0f, 1.0f, -1.0f,   -1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,   -1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,   1.0f, 1.0f, -1.0f,

			1.0f, 1.0f, 1.0f,   1.0f, 1.0f, -1.0f,
			1.0f, -1.0f, 1.0f,   1.0f, -1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,   -1.0f, 1.0f, -1.0f,
			-1.0f, -1.0f, 1.0f,   -1.0f, -1.0f, -1.0f
		};
	};

	float PointDistance(vec3 pointOne, vec3 pointTwo)
	{
		vec3 product = pointTwo - pointOne;
		float xDist = product.x;
		float yDist = product.y;
		float zDist = product.z;

		return sqrt((xDist * xDist) + (yDist * yDist) + (zDist * zDist));
	}

	bool AABBCollision(AABB* boxOne, AABB* boxTwo, vec3 posOne, vec3 posTwo)
	{

		return false;
	}

	class Phy7ics : public Publisher, public Subscriber
	{
	public:
		Phy7ics() {}
		void Update(EntityController* entityController, AssetManager* assetManager, double currentTime, double deltaTime)
		{

		}

	private:
		float gravityForce = 0.0005f;

	};
}