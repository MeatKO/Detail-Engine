#pragma once

#include "ECS.hpp"
#include "AssetManager.hpp"
#include "dMath.hpp"
#include "Transformation.hpp"

namespace detailEngine
{
	struct AABB
	{
		dMath::vec3 dimensions = dMath::vec3(1.0f);
		dMath::vec3 color = dMath::vec3(1.0f);
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

	float PointDistance(dMath::vec3 pointOne, dMath::vec3 pointTwo)
	{
		dMath::vec3 product = pointTwo - pointOne;
		float xDist = product.x;
		float yDist = product.y;
		float zDist = product.z;

		return sqrt((xDist * xDist) + (yDist * yDist) + (zDist * zDist));
	}

	bool AABBCollision(AABB* boxOne, AABB* boxTwo, dMath::vec3 posOne, dMath::vec3 posTwo)
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