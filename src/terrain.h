#pragma once
#include "tile_map.h"

#define GRASS_BLOCK 1

struct Terrain_Block : Tile
{

};

struct Terrain_Block_Renderable
{
	vec3 position;
	vec3 color;
};

struct Terrain_Renderer : Render_Data
{
	uint num_blocks;
	Terrain_Block_Renderable* blocks;
};

void terrain_to_renderable(Terrain_Block* blocks, Terrain_Renderer* renderer)
{
	uint num_blocks = 0;

	for (uint x = 0; x < 16; ++x) {
	for (uint z = 0; z < 16; ++z)
	{
		uint index = (x * 16) + z;
	
		renderer->blocks[index].position = vec3(x, 0, z);
		renderer->blocks[index].color    = vec3(1, 0, 0);
	
		num_blocks++;
	} }

	renderer->num_blocks = num_blocks;

	//if (map->selected_tile_index >= 0) renderer->tiles[map->selected_tile_index].color = vec3(0, 1, 1);
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