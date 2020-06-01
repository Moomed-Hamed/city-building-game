#pragma once
#include "physics.h"

#define TILE_COUNT_X 16
#define TILE_COUNT_Z 16
#define WORLD_TILE_COUNT (TILE_COUNT_X * TILE_COUNT_Z)

#define GRASS_TILE 0
#define WATER_TILE 1

struct Tile
{
	vec3 position;
	uint type;
	bool is_selected;
};

struct Game_World
{
	Tile* tiles;
};

void game_world_init(Game_World* world)
{
	*world = {};
	world->tiles = Alloc(Tile, WORLD_TILE_COUNT);
	ZeroMemory(world->tiles, WORLD_TILE_COUNT * sizeof(Tile));

	for (uint i = 0; i < WORLD_TILE_COUNT; ++i) world->tiles[i].type = GRASS_TILE;
}

struct Tile_Renderable
{
	vec3 position;
	vec3 color;
};

struct World_Renderer : Render_Data
{
	uint num_tiles;
	Tile_Renderable* tiles;
};

void tiles_to_renderable(Game_World* world, World_Renderer* renderer)
{
	renderer->num_tiles = 0;

	for (uint x = 0;  x < TILE_COUNT_X; ++x) {
	for (uint z = 0;  z < TILE_COUNT_Z; ++z)
	{
		uint index = (x * 16) + z;

		renderer->tiles[index].position = world->tiles[index].position;

		if (world->tiles[index].is_selected)
			renderer->tiles[index].color = vec3(0, 1, 1);
		else
			renderer->tiles[index].color = vec3(1, 0, 0);

		renderer->num_tiles++;
	} }
}

const char* TileVertSource = R"glsl(
#version 330 core

layout (location = 0) in vec3 Vertex;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 Position;
layout (location = 3) in vec3 Color;

uniform mat4 ProjView;
uniform vec3 ViewPos;

out vec3 normal;
out vec3 FragPos;
out vec3 viewpos;
out vec3 color;

void main()
{
	normal  = Normal;
	viewpos = ViewPos;
	color   = Color;
	FragPos = Vertex + Position;

	gl_Position = ProjView * vec4(FragPos, 1.0);
}
	)glsl";

const char* TileFragSource = R"glsl(
#version 330 core

in vec3 normal;
in vec3 FragPos;
in vec3 viewpos;
in vec3 color;

out vec4 FragColor;

void main()
{
	vec3 LightColor  = vec3(.4,.4,.4);
	vec3 ObjectColor = color;

	vec3 LightPos = vec3(0, 50, 0);

	vec3 Ambient = 0.3 * ObjectColor;

	vec3 LightDir = normalize(LightPos - FragPos);

	vec3 Diffuse = max(dot(normal, LightDir), 0.0) * ObjectColor;

	vec3 ViewDir = normalize(viewpos - FragPos);
	vec3 ReflectDir = reflect(-LightDir, normal);
	float Spec = pow(max(dot(ViewDir, ReflectDir), 0.0), 8);
	vec3 Specular = .5 * Spec * LightColor;

	vec3 Result = (Ambient + Diffuse + Specular) * LightColor;
	FragColor = vec4(Result, 1.0);
}
	)glsl";