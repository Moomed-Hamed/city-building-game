#pragma once
#include "terrain.h"

#define MAX_PROPS 256

struct Prop : Tile
{

};

struct Prop_Renderable
{
	vec3 position;
};

struct Prop_Renderer : Render_Data
{
	uint num_props;
	Prop_Renderable* props;
};

void props_to_renderable(Prop* props, Prop_Renderer* renderer)
{
	uint num_props = 0;

	for (uint i = 0; i < MAX_PROPS; ++i)
	{
		if (props[i].type == 0) continue;

		renderer->props[num_props].position = vec3(props[i].x, 1, props[i].z);

		num_props++;
	}

	renderer->num_props = num_props;
}