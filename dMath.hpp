#pragma once

#include <iostream>
#include <Windows.h>
#include <vector>
#include <string>

namespace dMath
{
	class vec2
	{
	public:
		vec2();
		vec2(float A) : x(A), y(A) {}
		vec2(float X, float Y) : x(X), y(Y) {}
		float x, y;
		vec2 operator + (vec2 Vec) { return vec2(x + Vec.x, y + Vec.y); }
		vec2 operator - (vec2 Vec) { return vec2(x - Vec.x, y - Vec.y); }
		vec2 operator * (vec2 Vec) { return vec2(x * Vec.x, y * Vec.y); }
		vec2& operator *= (vec2 Vec) { x *= Vec.x; y *= Vec.y; return *this; }
		vec2& operator *= (float mul) { x *= mul; y *= mul; return *this; }
		vec2 operator * (float mul) { return vec2(x * mul, y * mul); }
		vec2 operator / (vec2 Vec) { return vec2(x / Vec.x, y / Vec.y); }
		vec2 operator / (float div) { return vec2(x / div, y / div); }
		vec2& operator /= (vec2 Vec) { x /= Vec.x; y /= Vec.y; return *this; }
		vec2& operator /= (float mul) { x /= mul; y /= mul; return *this; }
	};

	inline std::ostream& operator<<(std::ostream& o, const vec2& v) {
		return o << "Vec2(" << v.x << ", " << v.y << ")";
	}

	class vec3
	{
	public:
		vec3();
		vec3(float A) : x(A), y(A), z(A) {}
		vec3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
		float x, y, z;
		vec3 operator + (vec3 Vec) { return vec3(x + Vec.x, y + Vec.y, z + Vec.z); }
		vec3 operator - (vec3 Vec) { return vec3(x - Vec.x, y - Vec.y, z - Vec.z); }
		vec3 operator * (vec3 Vec) { return vec3(x * Vec.x, y * Vec.y, z * Vec.z); }
		vec3& operator *= (vec3 Vec) { x *= Vec.x; y *= Vec.y; z *= Vec.z; return *this; }
		vec3& operator *= (float mul) { x *= mul; y *= mul; z *= mul; return *this; }
		vec3 operator * (float mul) { return vec3(x * mul, y * mul, z * mul); }
		vec3 operator / (vec3 Vec) { return vec3(x / Vec.x, y / Vec.y, z / Vec.z); }
		vec3 operator / (float div) { return vec3(x / div, y / div, z / div); }
		vec3& operator /= (vec3 Vec) { x /= Vec.x; y /= Vec.y; z /= Vec.z; return *this; }
		vec3& operator /= (float mul) { x /= mul; y /= mul; z /= mul; return *this; }
	};

	inline std::ostream& operator<<(std::ostream& o, const vec3& v) {
		return o << "Vec3(" << v.x << ", " << v.y << ", " << v.z << ")";
	}

	vec3 normalize(vec3 a);
	vec2 normalize(vec2 a);
	vec3 cross(vec3 a, vec3 b);
	float dot(vec3 a, vec3 b);
}