#pragma once
#include "intermediary.h" // should intermediary include math instead? (yes)
#include <intrin.h>

//__m128 ValueA = _mm_set1_ps(1.0);
//__m128 ValueB = _mm_set1_ps(2.0);
//__m128 Sum = _mm_add_ps(ValueA, ValueB);

typedef signed   char  int8;
typedef signed   short int16;
typedef signed   int   int32;
typedef signed   long  int64;

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;
typedef unsigned long  uint64;

typedef signed   int sint;
typedef unsigned int uint;
typedef unsigned int bool32;

#define NULL 0

#define PI	  3.14159265359f
#define TWOPI 6.28318530718f

#define ToRadians(value) ( ((value) * PI) / 180.0f )
#define ToDegrees(value) ( ((value) * 180.0f) / PI )

float max(float a, float b) { if (a > b) return a; else return b; }
float min(float a, float b) { if (a < b) return a; else return b; }

float sqrt(float n)
{
	int64 i;
	float x2, y;
	float three_halfs = 1.5F;

	x2 = n * 0.5F;
	y = n;
	i = *((int64 *)&y);			// evil floating point bit level hacking
	i = 0x5f3759df - (i >> 1);	// what the fuck? (Newton's Approximation)
	y = *(float *)&i;

	y = y * (three_halfs - (x2 * y * y)); // 1st iteration
	y = y * (three_halfs - (x2 * y * y)); // 2nd iteration (these can be removed) vvv
	y = y * (three_halfs - (x2 * y * y)); // 3rd iteration

	return 1 / y;
}

struct vec2
{
	union
	{
		struct
		{
			float x;
			float y;
		};
		struct
		{
			float u;
			float v;
		};
	};

	vec2(float X, float Y)
	{
		x = X;
		y = Y;
	}
};

struct vec3
{
	float x;
	float y;
	float z;

	vec3() = default;
	vec3(float X, float Y, float Z) { x = X; y = Y; z = Z; }

	vec3 operator + (float num)
	{
		vec3 ret = {};
		ret.x = x + num;
		ret.y = y + num;
		ret.z = z + num;
		return ret;
	}
	vec3 operator + (vec3 vec)
	{
		vec3 ret = {};
		ret.x = x + vec.x;
		ret.y = y + vec.y;
		ret.z = z + vec.z;
		return ret;
	}
	vec3 operator - (float num)
	{
		vec3 ret = {};
		ret.x = x - num;
		ret.y = y - num;
		ret.z = z - num;
		return ret;
	}
	vec3 operator - (vec3 vec)
	{
		vec3 ret = {};
		ret.x = x - vec.x;
		ret.y = y - vec.y;
		ret.z = z - vec.z;
		return ret;
	}
	vec3 operator * (float num)
	{
		vec3 ret = {};
		ret.x = x * num;
		ret.y = y * num;
		ret.z = z * num;
		return ret;
	}
	vec3 operator * (vec3 vec)
	{
		vec3 ret = {};
		ret.x = x * vec.x;
		ret.y = y * vec.y;
		ret.z = z * vec.z;
		return ret;
	}
	vec3 operator / (float num)
	{
		vec3 ret = {};
		ret.x = x / num;
		ret.y = y / num;
		ret.z = z / num;
		return ret;
	}
	vec3 operator / (vec3 vec)
	{
		vec3 ret = {};
		ret.x = x / vec.x;
		ret.y = y / vec.y;
		ret.z = z / vec.z;
		return ret;
	}

	vec3 operator += (float num)
	{
		vec3 ret = {};
		ret.x = x + num;
		ret.y = y + num;
		ret.z = z + num;
		return ret;
	}
	vec3 operator += (vec3 vec)
	{
		vec3 ret = {};
		ret.x = x + vec.x;
		ret.y = y + vec.y;
		ret.z = z + vec.z;
		return ret;
	}
	vec3 operator -= (float num)
	{
		vec3 ret = {};
		ret.x = x - num;
		ret.y = y - num;
		ret.z = z - num;
		return ret;
	}
	vec3 operator -= (vec3 vec)
	{
		vec3 ret = {};
		ret.x = x - vec.x;
		ret.y = y - vec.y;
		ret.z = z - vec.z;
		return ret;
	}
	vec3 operator *= (float num)
	{
		vec3 ret = {};
		ret.x = x * num;
		ret.y = y * num;
		ret.z = z * num;
		return ret;
	}
	vec3 operator *= (vec3 vec)
	{
		vec3 ret = {};
		ret.x = x * vec.x;
		ret.y = y * vec.y;
		ret.z = z * vec.z;
		return ret;
	}
	vec3 operator /= (float num)
	{
		vec3 ret = {};
		ret.x = x / num;
		ret.y = y / num;
		ret.z = z / num;
		return ret;
	}
	vec3 operator /= (vec3 vec)
	{
		vec3 ret = {};
		ret.x = x / vec.x;
		ret.y = y / vec.y;
		ret.z = z / vec.z;
		return ret;
	}

	bool operator == (vec3 vec) { return ((x == vec.x) & (y == vec.y) & (z == vec.z)); }
	bool operator != (vec3 vec) { return ((x != vec.x) | (y != vec.y) | (z != vec.z)); }

	float length() { return sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z)); }
};

float Dot(vec3 a, vec3 b) { return (a.x * b.x) + (a.y * b.y) + (a.z * b.z); }
float Length(vec3 vec) { return vec.length(); }
void Normalize(vec3* vec) { *vec = *vec / vec->length(); }
vec3 Normalize(vec3 vec) { return (vec / vec.length()); }
vec3 Cross(vec3 a, vec3 b)
{
	return vec3{ 1,1,1 }; // TODO IMPLEMENT
}

struct mat3
{
	union
	{
		struct { vec3 Col_1; vec3 Col_2; vec3 Col_3; };
		struct { float Elements[9]; };
	};

	mat3() = default;
	mat3(float n)
	{
		*this = {};
		Col_1.x = n;
		Col_2.y = n;
		Col_3.z = n;
	}

	mat3 operator * (mat3 mat)
	{
		vec3 Row_1(Col_1.x, Col_2.x, Col_3.x);
		vec3 Row_2(Col_1.y, Col_2.y, Col_3.y);
		vec3 Row_3(Col_1.z, Col_2.z, Col_3.z);

		mat3 ret = {};
		ret.Col_1.x = Dot(Row_1, mat.Col_1);
		ret.Col_1.y = Dot(Row_2, mat.Col_1);
		ret.Col_1.z = Dot(Row_3, mat.Col_1);

		ret.Col_2.x = Dot(Row_1, mat.Col_2);
		ret.Col_2.y = Dot(Row_2, mat.Col_2);
		ret.Col_2.z = Dot(Row_3, mat.Col_2);

		ret.Col_3.x = Dot(Row_1, mat.Col_3);
		ret.Col_3.y = Dot(Row_2, mat.Col_3);
		ret.Col_3.z = Dot(Row_3, mat.Col_3);
		return mat;
	}
};

mat3 inverse(mat3 mat);
mat3 pointAt(vec3 dir, vec3 up)
{
	vec3 f = Normalize(dir * -1.f); //front
	vec3 r = Cross(up, f);  //right
	vec3 u = up;

	mat3 Result(1);
	Result.Col_1.x = r.x;
	Result.Col_2.x = r.y;
	Result.Col_3.x = r.z;
	Result.Col_1.y = u.x;
	Result.Col_2.y = u.y;
	Result.Col_3.y = u.z;
	Result.Col_1.z = f.x;
	Result.Col_2.z = f.y;
	Result.Col_3.z = f.z;
	return inverse(Result);
}
mat3 pointAt(vec3 eye, vec3 center, vec3 up);

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
		__m128 first = _mm_set_ps(x, y, z, w);
		__m128 result = _mm_set_ps(num, num, num, num);
		result = _mm_add_ps(first, result);

		_mm_store_ps((float*)this, result);
	}
	void operator -= (float num)
	{
		__m128 first = _mm_set_ps(x, y, z, w);
		__m128 result = _mm_set_ps(num, num, num, num);
		result = _mm_sub_ps(first, result);

		_mm_store_ps((float*)this, result);
	}
	void operator *= (float num)
	{
		__m128 first = _mm_set_ps(x, y, z, w);
		__m128 result = _mm_set_ps(num, num, num, num);
		result = _mm_mul_ps(first, result);

		_mm_store_ps((float*)this, result);
	}
	void operator /= (float num)
	{
		__m128 first = _mm_set_ps(x, y, z, w);
		__m128 result = _mm_set_ps(num, num, num, num);
		result = _mm_div_ps(first, result);

		_mm_store_ps((float*)this, result);
	}

	vec4 operator + (float num)
	{
		__m128 first = _mm_set_ps(x, y, z, w);
		__m128 result = _mm_set_ps(num, num, num, num);
		result = _mm_add_ps(first, result);

		vec4 ret;
		_mm_store_ps((float*)&ret, result);
		return ret;
	}
	vec4 operator - (float num)
	{
		__m128 first = _mm_set_ps(x, y, z, w);
		__m128 result = _mm_set_ps(num, num, num, num);
		result = _mm_sub_ps(first, result);

		vec4 ret;
		_mm_store_ps((float*)&ret, result);
		return ret;
	}
	vec4 operator * (float num)
	{
		__m128 first = _mm_set_ps(x, y, z, w);
		__m128 result = _mm_set_ps(num, num, num, num);
		result = _mm_mul_ps(first, result);

		vec4 ret;
		_mm_store_ps((float*)&ret, result);
		return ret;
	}
	vec4 operator / (float num)
	{
		__m128 first = _mm_set_ps(x, y, z, w);
		__m128 result = _mm_set_ps(num, num, num, num);
		result = _mm_div_ps(first, result);

		vec4 ret;
		_mm_store_ps((float*)&ret, result);
		return ret;
	}

	vec4 operator + (vec4 vec)
	{
		__m128 first = _mm_load_ps((float*)&vec);
		__m128 result = _mm_load_ps((float*)this);
		result = _mm_add_ps(first, result);

		vec4 ret;
		_mm_store_ps((float*)&ret, result);
		return ret;
	}
	vec4 operator - (vec4 vec)
	{
		__m128 first = _mm_load_ps((float*)&vec);
		__m128 result = _mm_load_ps((float*)this);
		result = _mm_sub_ps(first, result);

		vec4 ret;
		_mm_store_ps((float*)&ret, result);
		return ret;
	}
	vec4 operator * (vec4 vec)
	{
		__m128 first = _mm_load_ps((float*)&vec);
		__m128 result = _mm_load_ps((float*)this);
		result = _mm_mul_ps(first, result);

		vec4 ret;
		_mm_store_ps((float*)&ret, result);
		return ret;
	}
	vec4 operator / (vec4 vec)
	{
		__m128 first = _mm_load_ps((float*)&vec);
		__m128 result = _mm_load_ps((float*)this);
		result = _mm_div_ps(first, result);

		vec4 ret;
		_mm_store_ps((float*)&ret, result);
		return ret;
	}
};

#define swap32(a,b) a = a ^ b; b = b ^ a; a = a ^ b;
#define swap64(a,b) a = a ^ b; b = b ^ a; a = a ^ b;