#pragma once

#include "dMath.hpp"

namespace detailEngine
{
	class Light
	{
	public:
		Light()
		{

		}

		dMath::vec3 position;
		dMath::vec3 color;
	};
}