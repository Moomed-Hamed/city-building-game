#include "camera.h"

#define MAP_X 16
#define MAP_Z 16
#define MAP_Y 6
#define NUM_MAP_TILES (MAP_X * MAP_Z * MAP_Y)
#define TILE_INDEX(x,y,z) (((x) + (MAP_X * (z))) + ((MAP_X * MAP_Z) * (y)))

#define TILE_AIR		0
#define TILE_GRASS	1
#define TILE_DIRT		2
#define TILE_SAND		3
#define TILE_STONE	4
#define TILE_WATER	5

typedef uint32 TileID;

// rendering

struct Tile_Drawable
{
	vec3 position;
};

struct Tile_Renderer
{
	uint num_land_tiles, num_fluid_tiles;
	Tile_Drawable land[NUM_MAP_TILES];
	Tile_Drawable fluid[NUM_MAP_TILES];

	Drawable_Mesh_UV land_mesh, fluid_mesh;
	Shader land_shader, fluid_shader;

	float fluid_timer;
};

void init(Tile_Renderer* renderer)
{
	renderer->fluid_timer = 0;

	load(&renderer->land_mesh, "assets/meshes/hex_tile.mesh_uv", "assets/textures/palette.bmp", sizeof(renderer->land));
	mesh_add_attrib_vec3(3, sizeof(Tile_Drawable), 0); // world pos

	load(&(renderer->land_shader), "assets/shaders/mesh_uv.vert", "assets/shaders/mesh_uv.frag");
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
	set_int(renderer->fluid_shader, "normals", 1);
	set_int(renderer->fluid_shader, "albedo", 2);
	set_int(renderer->fluid_shader, "texture_sampler", 3);
	set_float(renderer->fluid_shader, "timer", renderer->fluid_timer);
}
void update_renderer(Tile_Renderer* renderer, TileID* tiles, float dtime)
{
	uint num_land_tiles = 0;
	uint num_fluid_tiles = 0;

	Tile_Drawable* land_mem  = renderer->land;
	Tile_Drawable* fluid_mem = renderer->fluid;

	for (int x = 0; x < MAP_X; x++) {
	for (int z = 0; z < MAP_Z; z++) {
	for (int y = 0; y < MAP_Y; y++)
	{
		int height = tiles[TILE_INDEX(x, y, z)];

		if (height > 1)
		{
			land_mem->position = vec3(x, .2 * (float)height, z);
			if (z % 2) land_mem->position += vec3(0.5, 0, 0);

			num_land_tiles++;
			land_mem++;
		}
		else
		{
			fluid_mem->position = vec3(x, .2 * (float)height, z);
			if (z % 2) fluid_mem->position += vec3(0.5, 0, 0);

			num_fluid_tiles++;
			fluid_mem++;
		}
	} } }

	renderer->num_land_tiles = num_land_tiles;
	update(renderer->land_mesh, num_land_tiles * sizeof(Tile_Drawable), (byte*)renderer->land);

	renderer->num_fluid_tiles = num_fluid_tiles;
	update(renderer->fluid_mesh, num_fluid_tiles * sizeof(Tile_Drawable), (byte*)renderer->fluid);

	renderer->fluid_timer += dtime;
}