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

		int id = 0; // non-unique
		float brightness = 1.0f;
		vec3 position;
		vec3 color;
	};
}