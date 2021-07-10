#include "terrain.h"

struct Level
{
	Chunk chunks[NUM_CHUNKS];
};

void init(Level* level, vec3 position)
{
	uint offset_x = ( ((uint)position.x) & 0xFFF0 ) - 48;
	uint offset_z = ( ((uint)position.z) & 0xFFF0 ) - 48;

	uint i = 0;
	for (uint x = 0; x < 8; ++x) {
	for (uint z = 0; z < 8; ++z)
	{
		uvec2 coords = uvec2((x * CHUNK_X) + offset_x, (z * CHUNK_Z) + offset_z);
		level->chunks[i].coords = coords;
		generate_chunk(level->chunks + i);
		i++;
	} }
}

void update_level(Level* level, vec3 position, float dtime)
{
	uint chunk_x =  (((uint)position.x) & 0xFFF0 ) - 16;
	uint chunk_z =  (((uint)position.z) & 0xFFF0 ) - 16;

	static uvec2 last_coords = uvec2(0, 0);
	
	// rn i reload everything if we move to a new chunk
	// later i'll move unloaded chunks to border chunks &
	// only generate what we need
	if (last_coords != uvec2(chunk_x, chunk_z))
	{
		uint offset_x = ( ((uint)position.x) & 0xFFF0 ) - 48;
		uint offset_z = ( ((uint)position.z) & 0xFFF0 ) - 48;

		uint i = 0;
		for (uint x = 0; x < 8; ++x) {
		for (uint z = 0; z < 8; ++z)
		{
			uvec2 coords = uvec2((x * CHUNK_X) + offset_x, (z * CHUNK_Z) + offset_z);
			level->chunks[i].coords = coords;
			generate_chunk(level->chunks + i);
			i++;
		} }
	}

	last_coords = uvec2(chunk_x, chunk_z);
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
		bind_texture(renderer->chunk_renderers[i].mesh, 5);
		draw(renderer->chunk_renderers[i].mesh, renderer->chunk_renderers[i].num_forests);
	}
}