#pragma once
#include "maths_core.h"
#include "vectors.h"

struct mat3
{
	union
	{
		struct { vec3 col_1; vec3 col_2; vec3 col_3; };
		struct { float elements[9]; };
	};

	mat3() = default;
	mat3(float n)
	{
		*this = {};
		col_1.x = n;
		col_2.y = n;
		col_3.z = n;
	}

	mat3 operator * (mat3 mat)
	{
		vec3 row_1(col_1.x, col_2.x, col_3.x);
		vec3 row_2(col_1.y, col_2.y, col_3.y);
		vec3 row_3(col_1.z, col_2.z, col_3.z);

		mat3 ret = {};
		ret.col_1.x = dot(row_1, mat.col_1);
		ret.col_1.y = dot(row_2, mat.col_1);
		ret.col_1.z = dot(row_3, mat.col_1);

		ret.col_2.x = dot(row_1, mat.col_2);
		ret.col_2.y = dot(row_2, mat.col_2);
		ret.col_2.z = dot(row_3, mat.col_2);

		ret.col_3.x = dot(row_1, mat.col_3);
		ret.col_3.y = dot(row_2, mat.col_3);
		ret.col_3.z = dot(row_3, mat.col_3);
		return mat;
	}
};

mat3 inverse(mat3 mat);
mat3 point_at(vec3 dir, vec3 up)
{
	vec3 f = normalize(dir * -1.f); //front
	vec3 r = cross(up, f);  //right
	vec3 u = up;

	mat3 result(1);
	result.col_1.x = r.x;
	result.col_2.x = r.y;
	result.col_3.x = r.z;

	result.col_1.y = u.x;
	result.col_2.y = u.y;
	result.col_3.y = u.z;

	result.col_1.z = f.x;
	result.col_2.z = f.y;
	result.col_3.z = f.z;

	return inverse(result);
}
mat3 point_at(vec3 eye, vec3 center, vec3 up);
