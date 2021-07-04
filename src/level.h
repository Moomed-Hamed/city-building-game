#include "terrain.h"

struct Level
{
	Chunk chunks[NUM_CHUNKS];
};

void init(Level* level)
{
	// generate default chunks
	int i = 0;
	for (int x = 0; x < (16 * sqrt(NUM_CHUNKS)); x += 16) {
	for (int z = 0; z < (16 * sqrt(NUM_CHUNKS)); z += 16) {
		level->chunks[i].x = x;
		level->chunks[i].z = z;
		generate_chunk(level->chunks[i++].tiles, x * 16, z * 16);
	}}
}

void update_level(Level* level, float dtime)
{


}

// rendering

struct Level_Renderer
{
	Tile_Renderer chunk_renderers[NUM_CHUNKS];
};

void init(Level_Renderer* renderer)
{
	for (int i = 0; i < NUM_CHUNKS; i++)
	{
		init(renderer->chunk_renderers + i);
	}
}

void update_renderer(Level_Renderer* renderer, Level* level, float dtime)
{
	for (uint i = 0; i < NUM_CHUNKS; i++)
	{
		update_renderer(renderer->chunk_renderers + i, level->chunks[i].tiles, vec2(level->chunks[i].x, level->chunks[i].z), dtime);
	}
}

void draw(Level_Renderer* renderer, mat4 proj_view)
{
	for (int i = 0; i < NUM_CHUNKS; i++)
	{
		bind(renderer->chunk_renderers[i].land_shader);
		set_mat4(renderer->chunk_renderers[i].land_shader, "proj_view", proj_view);
		bind_texture(renderer->chunk_renderers[i].land_mesh, 3);
		draw(renderer->chunk_renderers[i].land_mesh, renderer->chunk_renderers[i].num_land_tiles);

		bind(renderer->chunk_renderers[i].fluid_shader);
		set_mat4(renderer->chunk_renderers[i].fluid_shader, "proj_view", proj_view);
		set_float(renderer->chunk_renderers[i].fluid_shader, "timer", renderer->chunk_renderers[i].fluid_timer);
		bind_texture(renderer->chunk_renderers[i].fluid_mesh, 3);
		draw(renderer->chunk_renderers[i].fluid_mesh, renderer->chunk_renderers[i].num_fluid_tiles);

		bind(renderer->chunk_renderers[i].shader);
		set_mat4(renderer->chunk_renderers[i].shader, "proj_view", proj_view);
		bind_texture(renderer->chunk_renderers[i].mesh, 3);
		draw(renderer->chunk_renderers[i].mesh, renderer->chunk_renderers[i].num_enemies);
	}
}