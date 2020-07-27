#pragma once

#include <iostream>
#include <string>
#include <sstream>

#include "ECS.hpp"

namespace detailEngine
{
	struct fvec2
	{
		fvec2() : x(0.0f), y(0.0f) {}
		fvec2(float A) : x(A), y(A) {}
		fvec2(float X, float Y) : x(X), y(Y) {}
		float x, y;
	};

	struct fvec3
	{
		fvec3() : x(0.0f), y(0.0f), z(0.0f) {}
		fvec3(float A) : x(A), y(A), z(A) {}
		fvec3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
		float x, y, z;
	};

	struct ivec2
	{
		ivec2() : x(0), y(0) {}
		ivec2(int A) : x(A), y(A) {}
		ivec2(int X, int Y) : x(X), y(Y) {}
		int x, y;
	};

	struct ivec3
	{
		ivec3() : x(0), y(0), z(0) {}
		ivec3(int A) : x(A), y(A), z(A) {}
		ivec3(int X, int Y, int Z) : x(X), y(Y), z(Z) {}
		int x, y, z;
	};
}