#include "cursor.h"

#define CHUNK_X 16
#define CHUNK_Z 16
#define CHUNK_Y 8
#define NUM_CHUNKS 64
#define NUM_CHUNK_TILES (CHUNK_X * CHUNK_Z * CHUNK_Y)
#define NUM_MAP_TILES (NUM_CHUNK_TILES * NUM_CHUNKS)
#define TILE_INDEX(x,y,z) (((x) + (CHUNK_X * (z))) + ((CHUNK_X * CHUNK_Z) * (y)))

#define TILE_AIR		0
#define TILE_GRASS	1
#define TILE_DIRT		2
#define TILE_SAND		3
#define TILE_STONE	4
#define TILE_WATER	5
#define TILE_DECORATION 6

typedef uint32 TileID;

// rendering

struct Land_Drawable
{
	vec3 position;
	float tex_offset;
};

struct Tile_Drawable
{
	vec3 position;
};

struct Forest_Drawable
{
	vec3 position;
};

struct Tile_Renderer
{
	uint num_land_tiles, num_fluid_tiles;
	Land_Drawable land [NUM_CHUNK_TILES];
	Tile_Drawable fluid[NUM_CHUNK_TILES];

	Drawable_Mesh_UV land_mesh, fluid_mesh;
	Shader land_shader, fluid_shader;

	float fluid_timer;

	//for decoration (for now)
	uint num_forests;
	Forest_Drawable forests[256];
	Drawable_Mesh_UV mesh;
	Shader shader;
};

void init(Tile_Renderer* renderer)
{
	renderer->fluid_timer = 0;

	load(&renderer->land_mesh, "assets/meshes/hex_tile.mesh_uv", "assets/textures/terrain.bmp", sizeof(renderer->land));
	mesh_add_attrib_vec3(3, sizeof(Land_Drawable), 0); // world pos
	mesh_add_attrib_float(4, sizeof(Land_Drawable), sizeof(vec3)); // texture offsets

	load(&(renderer->land_shader), "assets/shaders/hex_tile.vert", "assets/shaders/mesh_uv.frag");
	bind(renderer->land_shader);
	set_int(renderer->land_shader, "positions", 0);
	set_int(renderer->land_shader, "normals"  , 1);
	set_int(renderer->land_shader, "albedo"   , 2);
	set_int(renderer->land_shader, "texture_sampler", 3);

	load(&renderer->fluid_mesh, "assets/meshes/fluid_tile.mesh_uv", "assets/textures/palette.bmp", sizeof(renderer->fluid));
	mesh_add_attrib_vec3(3, sizeof(Tile_Drawable), 0); // world pos

	load(&(renderer->fluid_shader), "assets/shaders/fluid.vert", "assets/shaders/mesh_uv.frag");
	bind(renderer->fluid_shader);
	set_int(renderer->fluid_shader, "positions", 0);
	set_int(renderer->fluid_shader, "normals"  , 1);
	set_int(renderer->fluid_shader, "albedo"   , 2);
	set_int(renderer->fluid_shader, "texture_sampler", 3);
	set_float(renderer->fluid_shader, "timer", renderer->fluid_timer);

	load(&renderer->mesh, "assets/meshes/forest.mesh_uv", "assets/textures/palette.bmp", sizeof(renderer->forests));
	mesh_add_attrib_vec3(3, sizeof(Forest_Drawable), 0); // world pos

	load(&(renderer->shader), "assets/shaders/mesh_uv.vert", "assets/shaders/mesh_uv.frag");
	bind(renderer->shader);
	set_int(renderer->shader, "positions", 0);
	set_int(renderer->shader, "normals"  , 1);
	set_int(renderer->shader, "albedo"   , 2);
	set_int(renderer->shader, "texture_sampler", 3);
}
void update_renderer(Tile_Renderer* renderer, TileID* tiles, vec2 offset, float dtime)
{
	uint num_land_tiles = 0;
	uint num_fluid_tiles = 0;

	Land_Drawable* land_mem  = renderer->land;
	Tile_Drawable* fluid_mem = renderer->fluid;

	uint num_forests= 0;
	Forest_Drawable* forest_mem = renderer->forests;

	for (int x = 0; x < CHUNK_X; x++) {
	for (int z = 0; z < CHUNK_Z; z++) {
	for (int y = 0; y < CHUNK_Y; y++)
	{
		int tile = tiles[TILE_INDEX(x, y, z)];

		if (tile == TILE_AIR) continue;

		if (tile == TILE_DECORATION)
		{
			forest_mem->position = vec3(x + offset.x, .2 * (float)(y-1), z + offset.y);
			if (z % 2) forest_mem->position += vec3(0.5, 0, 0);

			num_forests++;
			forest_mem++;
		}
		else if (tile == TILE_WATER)
		{
			fluid_mem->position = vec3(x + offset.x, .2 * (float).6, z + offset.y);
			if (z % 2) fluid_mem->position += vec3(0.5, 0, 0);

			num_fluid_tiles++;
			fluid_mem++;
		}
		else // it must be a land tile
		{
			land_mem->position = vec3(x + offset.x, .2 * (float)y, z + offset.y);
			if (z % 2) land_mem->position += vec3(0.5, 0, 0);

			land_mem->tex_offset = (1.f / 16) * (tile - 1);

			num_land_tiles++;
			land_mem++;
		}
	} } }

	renderer->num_land_tiles = num_land_tiles;
	update(renderer->land_mesh, num_land_tiles * sizeof(Land_Drawable), (byte*)renderer->land);

	renderer->num_fluid_tiles = num_fluid_tiles;
	update(renderer->fluid_mesh, num_fluid_tiles * sizeof(Tile_Drawable), (byte*)renderer->fluid);

	renderer->fluid_timer += .1 * dtime;
	if (renderer->fluid_timer > 100000) renderer->fluid_timer = 0;

	//decoration
	renderer->num_forests = num_forests;
	update(renderer->mesh, num_forests * sizeof(Forest_Drawable), (byte*)renderer->forests);
}