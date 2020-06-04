#pragma once
#include "physics.h"

struct Tile
{
	union { u64 id; struct { u32 x, z; }; };
	u32 type;
};

struct Tile_Map
{
	uint num_tiles, x_length, z_length;
	Tile* tiles;

	int selected_tile_index; // if < 0 then no tile selected
};

void tile_map_init(Tile_Map* map, uint x_length, uint z_length)
{
	*map = {};

	map->x_length = x_length;
	map->z_length = z_length;
	map->num_tiles = x_length * z_length;

	map->selected_tile_index = -1;

	map->tiles = Alloc(Tile, map->num_tiles);
}

void tile_map_set_tile(Tile_Map* map, uint x, uint z, u32 type)
{
	uint index = (x * map->x_length) + z;
	map->tiles[index].type = type;
}

u32 tile_map_get_tile(Tile_Map* map, uint x, uint z)
{
	uint index = (x * map->x_length) + z;
	return map->tiles[index].type;
}

int tile_map_get_tile_index_from_ray(Tile_Map map, vec3 pos, vec3 ray_dir)
{
	// WARNING TODO : please speed this up thanks

	for (uint i = 0; i < 128; ++i)
	{
		for (uint x = 0; x < map.x_length; ++x) {
		for (uint z = 0; z < map.z_length; ++z)
		{
			uint index = (x * 16) + z;
			Cube_Collider test_collider = Cube_Collider{ ivec3(x, 0, z), ivec3(x + 1, 1, z + 1) };

			if (point_in_cube(pos, test_collider)) return index;
		} }

		pos += ray_dir * .5f;
	}

	return -1;
}