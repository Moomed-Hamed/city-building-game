#include "vehicles.h"

struct Level
{
	TileID tiles[NUM_MAP_TILES];
	Enemy enemies[MAX_ENEMIES];
	vec3 path_nodes[16]; // path enemies travel on
};

void update_level(Level* level, float dtime)
{
	
}
