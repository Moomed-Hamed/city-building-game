#pragma once
#include "renderer.h"

#define GRAVITY -9.81f

struct Cube_Collider
{
	vec3 Min;
	vec3 Max;
};

struct Sphere_Collider
{
	vec3 Position;
	float Radius;
};

struct Cyllinder_Collider
{
	vec3 Position;
	float Height;
	float Radius;
};

// --- Initializers --- //

/* Defined by pos of bottom left + dimentions */
void cube_collider_init(Cube_Collider* cube, vec3 position, vec3 dimentions)
{
	cube->Min = position;
	cube->Max = position + dimentions;
}

/* Defined by pos of center + redius */
void sphere_colider_init(Sphere_Collider* sphere, vec3 position, float radius)
{
	sphere->Position = position;
	sphere->Radius = radius;
}

/* Defined by pos of bottom center + height + redius */
void cyllinder_colider_init(Cyllinder_Collider* cyllinder, vec3 position, float height, float radius)
{
	cyllinder->Position = position;
	cyllinder->Height = height;
	cyllinder->Radius = radius;
}

// --- Point In Shape --- //

bool point_in_cube(vec3 point, Cube_Collider cube)
{
	if (point.x < cube.Min.x || point.x > cube.Max.x) return false;
	if (point.z < cube.Min.z || point.z > cube.Max.z) return false;
	if (point.y < cube.Min.y || point.y > cube.Max.y) return false;
	return true;
}
bool point_in_sphere(vec3 point, Sphere_Collider sphere)
{
	float distance = (point.x - sphere.Position.x) * (point.x - sphere.Position.x) +
		(point.y - sphere.Position.y) * (point.y - sphere.Position.y) +
		(point.z - sphere.Position.z) * (point.z - sphere.Position.z);
	return (distance < (sphere.Radius * sphere.Radius));
}
bool point_in_cyllinder(vec3 point, Cyllinder_Collider cyllinder)
{
	if (point.y > cyllinder.Position.y + cyllinder.Height) return false;
	if (point.y < cyllinder.Position.y - cyllinder.Height) return false;

	vec2 circle_1 = vec2(point.x, point.z);

	return true; //PointInCircle...
}

// --- Line in Shape --- //

//this algorithm can be sped up significantly
bool line_in_cube(vec3 start, vec3 dir, Cube_Collider cube)
{
	vec3 test_point = start;
	for(int i = 0; i < 1024; ++i)
	{
		if (point_in_cube(test_point, cube)) return true;
		else test_point += .5f * dir;
	}

	return false;
}

//this algorithm can be sped up significantly
bool line_in_sphere(vec3 start, vec3 dir, Sphere_Collider sphere)
{
	vec3 test_point = start;
	for (int i = 0; i < 1024; ++i)
	{
		if (point_in_sphere(test_point, sphere)) return true;
		else test_point += .5f * dir;
	}

	return false;
}

// --- Shape Intersections --- //

bool sphere_sphere_intersect(Sphere_Collider sphere_1, Sphere_Collider sphere_2)
{
	float Distance = (sphere_1.Position.x - sphere_2.Position.x) * (sphere_1.Position.x - sphere_2.Position.x) +
		(sphere_1.Position.y - sphere_2.Position.y) * (sphere_1.Position.y - sphere_2.Position.y) +
		(sphere_1.Position.z - sphere_2.Position.z) * (sphere_1.Position.z - sphere_2.Position.z);

	float RadiusSum = (sphere_1.Radius * sphere_1.Radius) + (sphere_2.Radius * sphere_2.Radius);

	return (Distance < RadiusSum);
}
bool sphere_cube_intersect(Sphere_Collider sphere, Cube_Collider cube)
{
	float xPos, yPos, zPos;

	if (sphere.Position.x < cube.Min.x) xPos = cube.Min.x;
	else if (sphere.Position.x > cube.Max.x) xPos = cube.Max.x;
	else xPos = sphere.Position.x;

	if (sphere.Position.y < cube.Min.y) yPos = cube.Min.y;
	else if (sphere.Position.y > cube.Max.y) yPos = cube.Max.y;
	else yPos = sphere.Position.y;

	if (sphere.Position.z < cube.Min.z) zPos = cube.Min.z;
	else if (sphere.Position.z > cube.Max.z) zPos = cube.Max.z;
	else zPos = sphere.Position.z;

	return point_in_sphere(vec3(xPos, yPos, zPos), sphere);
}