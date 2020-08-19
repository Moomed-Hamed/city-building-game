#pragma once
#include "maths_core.h"
#include "vectors.h"
#include "matrices.h"

struct quat
{
	float x, y, z, w;
};

mat3 quat_to_mat3(quat q);
quat mat3_to_quat(mat3 m);