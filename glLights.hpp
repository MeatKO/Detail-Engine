#pragma once

using namespace glm;

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
		vec3 position;
		vec3 color;
	};
}