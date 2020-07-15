#pragma once

#include "dMath.hpp"

namespace detailEngine
{
	class Light
	{
	public:
		Light(int ID)
		{
			id = ID;
		}

		int id;
		dMath::vec3 position;
		dMath::vec3 color;
	};
}