#include "tiles.h"

struct Chunk
{
	union { struct { uint32 x, z; }; uint64 id; uvec2 coords; };
	TileID tiles[NUM_CHUNK_TILES];
};

void generate_chunk(Chunk* chunk, uint offset = 0, float flatness = 16)
{
	uint x_pos = chunk->x & 0xFFF0;
	uint z_pos = chunk->z & 0xFFF0;

	TileID* tiles = chunk->tiles;

	for (uint i = 0; i < NUM_CHUNK_TILES; ++i) tiles[i] = TILE_AIR;

	float n = 1.f / 6.1;

	for (uint x = 0; x < CHUNK_X; ++x) {
	for (uint z = 0; z < CHUNK_Z; ++z) {

		float noise_value = perlin(n * (x + x_pos + offset), n * (z + z_pos + offset)) * 8;
		uint height = noise_value - 1;
		
		for (uint y = 0; y < CHUNK_Y; ++y)
		{
			uint index = TILE_INDEX(x, y, z);
			tiles[index] = NULL;

			if (y == height)
			{
				tiles[index] = TILE_GRASS;

				if (__rdtsc() % 2 == 0)
					tiles[TILE_INDEX(x, y + 1, z)] = TILE_DECORATION;
			}
			else if (y == height - 1)
			{
				tiles[index] = TILE_DIRT;
			}
			else
			{
				tiles[index] = TILE_STONE;
			}
			if (y == 0)
			{
				tiles[index] = TILE_STONE;
			}
			if (height == 1 && y == 1)
			{
				tiles[index] = TILE_WATER; // fill in water & shores after terrain gen??
			}
			if (y == 2 && height == 2)
			{
				tiles[index] = TILE_SAND;
			}
			
			if(y > height) tiles[index] = TILE_AIR;
		}
	} }

	for (uint x = 0; x < CHUNK_X; ++x) {
	for (uint z = 0; z < CHUNK_Z; ++z) {

		if (tiles[TILE_INDEX(x, 3, z)] == TILE_GRASS)
		{
			if (__rdtsc() % 2 == 0)
				tiles[TILE_INDEX(x, 4, z)] = TILE_DECORATION;
		}
	} }
}