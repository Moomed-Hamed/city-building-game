#pragma once
#include "props.h"

#define MAX_UNITS 256

struct Unit : Tile
{
	u32 team; // u32 so we can support more than two teams
};

struct Unit_Renderable
{
	vec3 position;
	vec3 color;
};

struct Unit_Renderer : Render_Data
{
	uint num_units;
	Unit_Renderable* units;
};

void friendly_units_to_renderable(Unit* units, Unit_Renderer* renderer, uint max_units = MAX_UNITS)
{
	uint num_units = 0;

	for (uint i = 0; i < max_units; ++i)
	{
		u32 unit_type = units[i].type;

		if (unit_type == 0) continue;

		switch (unit_type)
		{
			default: renderer->units[num_units].color = vec3(0, 1, 0); break;
		}
		
		renderer->units[num_units].position = vec3(units[i].x, 1, units[i].z);
		
		num_units++;
	}

	renderer->num_units = num_units;
}

void enemy_units_to_renderable(Unit* units, Unit_Renderer* renderer, uint max_units = MAX_UNITS)
{
	uint num_units = 0;

	for (uint i = 0; i < max_units; ++i)
	{
		u32 unit_type = units[i].type;

		if (unit_type == 0) continue;

		switch (unit_type)
		{
			default: renderer->units[num_units].color = vec3(0, .5, 0); break;
		}
		
		renderer->units[num_units].position = vec3(units[i].x, 1, units[i].z);
		
		num_units++;
	}

	renderer->num_units = num_units;
}