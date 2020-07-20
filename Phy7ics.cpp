#include "Phy7ics.hpp"

namespace detailEngine
{
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

	void Phy7ics::Update(EntityController* entityController, AssetManager* assetManager, double currentTime, double deltaTime)
	{
	}
}
