#pragma once
#include "maths_core.h"

struct vec2
{
	union
	{
		struct { float x, y; };
		struct { float u, v; };
	};

	vec2(float X, float Y)
	{
		x = X;
		y = Y;
	}
};
struct vec3
{
	float x, y, z;

	vec3() = default;
	vec3(float X, float Y, float Z) { x = X; y = Y; z = Z; }

	void operator += (float num)
	{
		*this = vec3{ x + num, y + num, z + num };
	}
	void operator -= (float num)
	{
		*this = vec3{ x - num, y - num, z - num };
	}
	void operator *= (float num)
	{
		*this = vec3{ x * num, y * num, z * num };
	}
	void operator /= (float num)
	{
		*this = vec3{ x / num, y / num, z / num };
	}

	void operator += (vec3 vec)
	{
		*this = vec3{ x + vec.x, y + vec.y, z + vec.z };
	}
	void operator -= (vec3 vec)
	{
		*this = vec3{ x - vec.x, y - vec.y, z - vec.z };
	}
	void operator *= (vec3 vec)
	{
		*this = vec3{ x * vec.x, y * vec.y, z * vec.z };
	}
	void operator /= (vec3 vec)
	{
		*this = vec3{ x / vec.x, y / vec.y, z / vec.z };
	}

	vec3 operator + (float num)
	{
		return vec3{ x + num, y + num, z + num };
	}
	vec3 operator - (float num)
	{
		return vec3{ x - num, y - num, z - num };
	}
	vec3 operator * (float num)
	{
		return vec3{ x * num, y * num, z * num };
	}
	vec3 operator / (float num)
	{
		return vec3{ x / num, y / num, z / num };
	}

	vec3 operator + (vec3 vec)
	{
		return vec3{ x + vec.x, y + vec.y, z + vec.z };
	}
	vec3 operator - (vec3 vec)
	{
		return vec3{ x - vec.x, y - vec.y, z - vec.z };
	}
	vec3 operator * (vec3 vec)
	{
		return vec3{ x * vec.x, y * vec.y, z * vec.z };
	}
	vec3 operator / (vec3 vec)
	{
		return vec3{ x / vec.x, y / vec.y, z / vec.z };
	}

	bool operator == (vec3 vec) { return ((x == vec.x) & (y == vec.y) & (z == vec.z)); }
	bool operator != (vec3 vec) { return ((x != vec.x) | (y != vec.y) | (z != vec.z)); }

	float length() { return sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z)); }
};
struct vec4
{
	float x, y, z, w;

	vec4() = default;
	vec4(float X, float Y, float Z, float W)
	{
		x = X;
		y = Y;
		z = Z;
		w = W;
	}

	bool32 operator == (vec4 vec)
	{
		__m128 first = _mm_set_ps(x, y, z, w);
		__m128 result = _mm_load_ps((float*)&vec);
		result = _mm_sub_ps(first, result);

		uint64 ret[2];
		_mm_store_ps((float*)ret, result);

		if (!ret[0] && !ret[1]) return true;
		else return false;
	}

	void operator += (float num)
	{
		f128 first = f128_load(x, y, z, w);
		f128 result = f128_add(first, num);

		f128_store(result, (float*)this);
	}
	void operator -= (float num)
	{
		f128 first  = f128_load(x, y, z, w);
		f128 result = f128_sub(first, num);

		f128_store(result, (float*)this);
	}
	void operator *= (float num)
	{
		f128 first = f128_load(x, y, z, w);
		f128 result = f128_mul(first, num);

		f128_store(result, (float*)this);
	}
	void operator /= (float num)
	{
		f128 first  = f128_load(x, y, z, w);
		f128 result = f128_div(first, num);

		f128_store(result, (float*)this);
	}

	vec4 operator + (float num)
	{
		f128 first  = f128_load(x, y, z, w);
		f128 result = f128_add(first, num);

		vec4 ret;
		f128_store(result, (float*)&ret);
		return ret;
	}
	vec4 operator - (float num)
	{
		f128 first  = f128_load(x, y, z, w);
		f128 result = f128_sub(first, num);

		vec4 ret;
		f128_store(result, (float*)&ret);
		return ret;
	}
	vec4 operator * (float num)
	{
		f128 first  = f128_load(x, y, z, w);
		f128 result = f128_mul(first, num);

		vec4 ret;
		f128_store(result, (float*)&ret);
		return ret;
	}
	vec4 operator / (float num)
	{
		f128 first  = f128_load(x, y, z, w);
		f128 result = f128_div(first, num);

		vec4 ret;
		f128_store(result, (float*)&ret);
		return ret;
	}

	vec4 operator + (vec4 vec)
	{
		f128 first  = f128_load((float*)&vec);
		f128 result = f128_load((float*)this);
		result = f128_add(first, result);

		vec4 ret = {};
		f128_store(result, (float*)&ret);

		return ret;
	}
	vec4 operator - (vec4 vec)
	{
		f128 first  = f128_load((float*)&vec);
		f128 result = f128_load((float*)this);
		result = f128_sub(first, result);

		vec4 ret = {};
		f128_store(result, (float*)&ret);

		return ret;
	}
	vec4 operator * (vec4 vec)
	{
		f128 first = f128_load((float*)&vec);
		f128 result = f128_load((float*)this);
		result = f128_mul(first, result);

		vec4 ret = {};
		f128_store(result, (float*)&ret);

		return ret;
	}
	vec4 operator / (vec4 vec)
	{
		f128 first = f128_load((float*)&vec);
		f128 result = f128_load((float*)this);
		result = f128_div(first, result);

		vec4 ret = {};
		f128_store(result, (float*)&ret);

		return ret;
	}
};

float dot(vec3 a, vec3 b) { return (a.x * b.x) + (a.y * b.y) + (a.z * b.z); }
float length(vec3 vec) { return vec.length(); }
void normalize(vec3* vec) { *vec = *vec / vec->length(); }
vec3 normalize(vec3 vec) { return (vec / vec.length()); }
vec3 cross(vec3 a, vec3 b)
{
	return vec3{ 1,1,1 }; // TODO IMPLEMENT
}