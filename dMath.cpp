#include "dMath.hpp"

namespace dMath
{
	vec3 dMath::normalize(vec3 a)
	{
		float vecLength = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
		return a / vecLength;
	}
	vec2 normalize(vec2 a)
	{
		float vecLength = sqrt(a.x * a.x + a.y * a.y);
		return a / vecLength;
	}
	vec3 dMath::cross(vec3 a, vec3 b)
	{
		return vec3((a.y * b.z - a.z * b.y), (a.x * b.z - a.z * b.x), (a.x * b.y - a.y * b.x));
	}
	float dMath::dot(vec3 a, vec3 b)
	{
		return (a.x * b.x + a.y * b.y + a.z * b.z);
	}
}

