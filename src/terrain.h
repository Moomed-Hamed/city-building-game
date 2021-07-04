#include "tiles.h"

struct Chunk
{
	union { struct { uint32 x, z; }; uint64 id; uvec2 coords; };
	TileID tiles[NUM_CHUNK_TILES];
};

// terrain generation
void generate_chunk(TileID* tiles, uint xoffset = 0, uint zoffset = 0, float flatness = 16)
{
	for (uint i = 0; i < NUM_CHUNK_TILES; ++i) tiles[i] = TILE_AIR;

	float n = 1.f / 6.1;

	for (uint x = 0; x < CHUNK_X; ++x) {
	for (uint z = 0; z < CHUNK_Z; ++z) {

		float noise_value = perlin(n * (x + xoffset), n * (z + zoffset)) * 8;
		uint height = noise_value - 1;
		//out(height);
		
		for (uint y = 0; y < CHUNK_Y; ++y)
		{
			uint index = TILE_INDEX(x, y, z);
			tiles[index] = NULL;
		
			if (height > 5)
			{
				out("height is: " << height);
				height = 5;
			}

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

		// tried and failed to make shorelines
		//if (x > 0 && z > 0 && x < 15 && z < 15)
		//{
		//	if (z % 2)
		//	{
		//		if (TILE_INDEX(x, 1, z) != TILE_WATER)
		//		{
		//			if (tiles[TILE_INDEX(x + 1, 1, z + 0)] != TILE_WATER ||
		//				tiles[TILE_INDEX(x - 1, 1, z + 0)] != TILE_WATER ||
		//				tiles[TILE_INDEX(x + 0, 1, z + 1)] != TILE_WATER ||
		//				tiles[TILE_INDEX(x + 0, 1, z - 1)] != TILE_WATER ||
		//				tiles[TILE_INDEX(x + 1, 1, z + 1)] != TILE_WATER ||
		//				tiles[TILE_INDEX(x + 1, 1, z - 1)] != TILE_WATER)
		//			{
		//				if (tiles[TILE_INDEX(x + 1, 1, z + 0)] == TILE_WATER ||
		//					tiles[TILE_INDEX(x - 1, 1, z + 0)] == TILE_WATER ||
		//					tiles[TILE_INDEX(x + 0, 1, z + 1)] == TILE_WATER ||
		//					tiles[TILE_INDEX(x + 0, 1, z - 1)] == TILE_WATER ||
		//					tiles[TILE_INDEX(x + 1, 1, z + 1)] == TILE_WATER ||
		//					tiles[TILE_INDEX(x + 1, 1, z - 1)] == TILE_WATER)
		//				{
		//					tiles[TILE_INDEX(x, 1, z)] = TILE_SAND;
		//					tiles[TILE_INDEX(x, 0, z)] = TILE_STONE;
		//				}
		//			}
		//		}
		//	}
		//	else
		//	{
		//		if (TILE_INDEX(x, 1, z) != TILE_WATER)
		//		{
		//			if (tiles[TILE_INDEX(x + 1, 1, z + 0)] != TILE_WATER ||
		//				tiles[TILE_INDEX(x - 1, 1, z + 0)] != TILE_WATER ||
		//				tiles[TILE_INDEX(x + 0, 1, z + 1)] != TILE_WATER ||
		//				tiles[TILE_INDEX(x + 0, 1, z - 1)] != TILE_WATER ||
		//				tiles[TILE_INDEX(x - 1, 1, z - 1)] != TILE_WATER ||
		//				tiles[TILE_INDEX(x - 1, 1, z + 1)] != TILE_WATER)
		//			{
		//				if (tiles[TILE_INDEX(x + 1, 1, z + 0)] == TILE_WATER ||
		//					tiles[TILE_INDEX(x - 1, 1, z + 0)] == TILE_WATER ||
		//					tiles[TILE_INDEX(x + 0, 1, z + 1)] == TILE_WATER ||
		//					tiles[TILE_INDEX(x + 0, 1, z - 1)] == TILE_WATER ||
		//					tiles[TILE_INDEX(x - 1, 1, z - 1)] == TILE_WATER ||
		//					tiles[TILE_INDEX(x - 1, 1, z + 1)] == TILE_WATER)
		//				{
		//					tiles[TILE_INDEX(x, 1, z)] = TILE_SAND;
		//					tiles[TILE_INDEX(x, 0, z)] = TILE_STONE;
		//				}
		//			}
		//		}
		//	}
		//}

	} }
}