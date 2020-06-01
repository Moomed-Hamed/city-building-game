#pragma once
#include "renderer.h"

struct Camera
{
	vec3 Position;
	vec3 Velocity;

	vec3 Front, Right, Up;
	float Yaw, Pitch;
};

void init_camera(Camera* camera, vec3 pos = { -16, 16, 0 }, vec3 vel = { 0, 0, 0 })
{
	*camera = { pos, vel };
}

void update_camera(Camera* camera, float xoffset, float yoffset)
{
	camera->Yaw   += (xoffset * .005f) / TWOPI;
	camera->Pitch += (yoffset * .005f) / TWOPI;

	if (camera->Pitch >  PI / 2.01) camera->Pitch =  PI / 2.01;
	if (camera->Pitch < -PI / 2.01) camera->Pitch = -PI / 2.01;

	camera->Front.y = sin(camera->Pitch);
	camera->Front.x = cos(camera->Pitch) * cos(camera->Yaw);
	camera->Front.z = cos(camera->Pitch) * sin(camera->Yaw);

	camera->Front = glm::normalize(camera->Front);
	camera->Right = glm::normalize(glm::cross(camera->Front, vec3(0, 1, 0)));
	camera->Up    = glm::normalize(glm::cross(camera->Right, camera->Front));
}

#define CAMERA_FORWARD	0
#define CAMERA_BACKWARD	1
#define CAMERA_LEFT		2
#define CAMERA_RIGHT	   3
void camera_move(Camera* camera, int direction, float velocity, float DeltaTime)
{
	if (direction == CAMERA_FORWARD ) camera->Position += camera->Front * velocity * DeltaTime;
	if (direction == CAMERA_LEFT    ) camera->Position -= camera->Right * velocity * DeltaTime;
	if (direction == CAMERA_RIGHT   ) camera->Position += camera->Right * velocity * DeltaTime;
	if (direction == CAMERA_BACKWARD) camera->Position -= camera->Front * velocity * DeltaTime;
}