#include "tiles.h"

struct Level
{
	TileID tiles[NUM_MAP_TILES];
	vec3 path_nodes[16]; // path enemies travel on
};

void update_level(Level* level, float dtime)
{
}

// terrain generation

void test_gen(Level* level, uint offset = 0, float flatness = 16)
{
	uint* blocks = level->tiles;

	for (uint i = 0; i < NUM_MAP_TILES; ++i) blocks[i] = 0;

	float n = 1.f / flatness;

	for (uint x = 0; x < MAP_X; ++x) {
	for (uint z = 0; z < MAP_Z; ++z) {

		float noise_value = perlin(n * (x + offset), n * (z + offset)) * 5;
		uint height = noise_value;
		//out(height);
		
		for (uint y = 0; y < MAP_Y; ++y)
		{
			uint index = TILE_INDEX(x, y, z);
			blocks[index] = NULL;
		
			if (height > 5)
			{
				out("height is: " << height);
				height = 5;
			}

			if (height == 1)
			{
				blocks[index] = TILE_WATER;
			}
			else if (height == 2)
			{
				blocks[index] = TILE_SAND;
			}
			else
			{
				blocks[index] = TILE_GRASS;
			}

			//switch (height)
			//{
			//case 0: blocks[index] = TILE_WATER; break;
			//case 1: blocks[index] = TILE_SAND; break;
			//case 2: blocks[index] = TILE_DIRT; break;
			//case 3: blocks[index] = TILE_GRASS; break;
			//case 4: blocks[index] = TILE_GRASS; break;
			//case 5: blocks[index] = TILE_GRASS; break;
			//}
		}
	} }
}