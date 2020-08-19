#pragma once
#include "physics.h"

#define GAME_MAP_X 16
#define GAME_MAP_Z 16
#define GAME_MAP_Y 1
#define GAME_MAP_SIZE ( GAME_MAP_X * GAME_MAP_Z * GAME_MAP_Y )
#define GAME_MAP_INDEX(x,y,z) (((x) + (16 * (z))) + (256 * (y)))

#define AIR_TILE   0
#define STONE_TILE 1
#define DIRT_TILE  2
#define GRASS_TILE 3
#define SAND_TILE  4
#define WATER_TILE 5
#define LAVA_TILE  6

typedef uint8 Tile;

struct Game_Map
{
	Tile* tiles;

	int selected_tile_index; // < 0 = none selected
};

void game_map_init(Game_Map* map)
{
	map->selected_tile_index = -1;
	map->tiles = (Tile*)malloc(GAME_MAP_SIZE * sizeof(Tile));
	memset(map->tiles, 1, GAME_MAP_SIZE * sizeof(Tile));
}

// ------ Rendering ------ //

struct Tile_Renderable
{
	vec3 position;
	vec2 tex_coord;
};

struct Tile_Renderer : Render_Data_UV
{
	uint num_tiles;
	Tile_Renderable* memory;
};

vec2 tile_get_tex_offset(Tile tile)
{
	switch (tile)
	{
	case 1: return vec2(0.0, 0.00);  // stone
	case 2: return vec2(.25, 0.00);  // dirt
	case 3: return vec2(.50, 0.00);  // grass
	case 4: return vec2(.75, 0.00);  // sand
	case 5: return vec2(0.0, -.25);  // wood
	case 6: return vec2(.25, -.25);  // wood plank
	case 7: return vec2(.50, -.25);  // clay brick
	case 8: return vec2(.75, -.25);  // stone brick
	case 9: return vec2(0.0, -.50);  // ore
	case 10: return vec2(.25, -.50);
	case 11: return vec2(.50, -.50);
	case 12: return vec2(.75, -.50);
	case 13: return vec2(0.0, -.75);
	case 14: return vec2(.25, -.75);
	case 15: return vec2(.50, -.75);

	default: return vec2(.75, -.75); //invalid block
	}
}

void tiles_to_renderable(Tile* tiles, Tile_Renderer* renderer)
{
	renderer->num_tiles= 0;

	Tile_Renderable* tile_mem = renderer->memory;

	for (uint x = 0; x < GAME_MAP_X; ++x) {
	for (uint z = 0; z < GAME_MAP_Z; ++z) {
	for (uint y = 0; y < GAME_MAP_Y; ++y)
	{
		uint index = GAME_MAP_INDEX(x, y, z);
	
		Tile tile = tiles[index];
		if (!tile) continue; // don't render air

		tile_mem->position  = vec3(x, y, z);
		tile_mem->tex_coord = tile_get_tex_offset(tile);
		tile_mem++;
		renderer->num_tiles++;
	} } }
}
void tile_renderer_init(Tile_Renderer* renderer)
{
	renderer->memory = Alloc(Tile_Renderable, GAME_MAP_SIZE);

	render_data_init(renderer, sizeof(Tile_Renderable) * GAME_MAP_SIZE,
		"assets/models/block.model", SolidVertSource, SolidFragSource, "assets/textures/texture_atlas.bmp");

	// vert, norm, tex
	gl_add_attrib_vec3(3, sizeof(Tile_Renderable), 0); // pos
	gl_add_attrib_vec2(4, sizeof(Tile_Renderable), sizeof(vec3)); // tex coord
}
void tile_renderer_draw(Tile_Renderer* renderer, Tile* tiles, mat4* proj_view, vec3 viewpos)
{
	tiles_to_renderable(tiles, renderer);

	glBindTexture(GL_TEXTURE_2D, renderer->texture_id);
	render_data_draw(*renderer, proj_view, viewpos, renderer->num_tiles * sizeof(Tile_Renderable),
		(byte*)renderer->memory, renderer->num_tiles);
}