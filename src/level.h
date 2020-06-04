#pragma once
#include "input.h"

struct Game_Level
{
	uint num_friendly_untis, num_enemy_units, num_props;
	uint turn_count;
	bool is_player_turn;

	Unit* friendly_untis;
	Unit* enemy_untis;
	Prop* props;

	Tile_Map terrain_map; // this is just scenery
	Tile_Map unit_map; // this is where the game is played
};

Game_Level make_game_level(uint x_length, uint z_length)
{
	Game_Level level = {};
	level.is_player_turn = true;

	level.friendly_untis = Alloc(Unit, MAX_UNITS);
	level.enemy_untis    = Alloc(Unit, MAX_UNITS);
	level.props          = Alloc(Prop, MAX_PROPS);

	tile_map_init(&level.terrain_map, x_length, z_length);
	tile_map_init(&level.unit_map   , x_length, z_length);

	return level;
}

void free_game_level(Game_Level* level)
{
	Free(level->friendly_untis);
	Free(level->enemy_untis);
	Free(level->props);

	*level = {};
}

void spawn_friendly_unit(Game_Level* level, uint x, uint z)
{
	if (level->num_friendly_untis >= MAX_UNITS) return;

	uint index = level->num_friendly_untis++;
	level->friendly_untis[index].type = 1;
	level->friendly_untis[index].team = 1;
	level->friendly_untis[index].x = x;
	level->friendly_untis[index].z = z;
}

void update_level(Game_Level* level)
{
	Game_Level lvl = *level;

	uint num_friendly_units;
	uint num_enemy_units;

	// units
	for (uint i = 0; i < lvl.unit_map.num_tiles; i++)
	{
		u32 type = lvl.unit_map.tiles[i].type;

		if (type == 0) continue;

		switch (type)
		{
			case 1: break;
		}
	}
}

struct Level_Renderer
{
	Terrain_Renderer terrain;
	Unit_Renderer friendly_units;
	Unit_Renderer enemy_units;
	Prop_Renderer props;
};

void level_to_renderable(Game_Level level, Level_Renderer* renderer)
{
	terrain_to_renderable(NULL, &renderer->terrain);
	friendly_units_to_renderable(level.friendly_untis, &renderer->friendly_units);
	enemy_units_to_renderable(level.enemy_untis, &renderer->enemy_units);
	props_to_renderable(level.props, &renderer->props);
}

void draw_level(Level_Renderer renderer, mat4* proj_view, vec3 position)
{
	draw(renderer.terrain, proj_view, position, renderer.terrain.num_blocks * sizeof(Terrain_Block_Renderable),
		(byte*)renderer.terrain.blocks, renderer.terrain.num_blocks);

	draw(renderer.friendly_units, proj_view, position, renderer.friendly_units.num_units * sizeof(Unit_Renderable),
		(byte*)renderer.friendly_units.units, renderer.friendly_units.num_units);

	draw(renderer.enemy_units, proj_view, position, renderer.enemy_units.num_units * sizeof(Unit_Renderable),
		(byte*)renderer.enemy_units.units, renderer.enemy_units.num_units);

	draw(renderer.props, proj_view, position, renderer.props.num_props * sizeof(Prop_Renderable),
		(byte*)renderer.props.props, renderer.props.num_props);
}