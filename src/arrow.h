#include "camera.h"

#define MAX_ENEMIES 1

#define NATURE_FOREST 1

struct Enemy
{
	uint type;
	vec3 position;
};

void spawn_enemy(Enemy* enemies, vec3 pos, uint type = 1)
{
	for (uint i = 0; i < MAX_ENEMIES; i++)
	{
		if (enemies[i].type == NULL)
		{
			enemies[i].type = type;
			enemies[i].position = pos;

			return;
		}
	}
}

// rendering

struct Enemy_Drawable
{
	vec3 position;
};

struct Enemy_Renderer
{
	uint num_enemies;
	Enemy_Drawable enemies[MAX_ENEMIES];

	Drawable_Mesh_UV mesh;
	Shader shader;
};

void init(Enemy_Renderer* renderer)
{
	load(&renderer->mesh, "assets/meshes/arrow.mesh_uv", "assets/textures/palette.bmp", sizeof(renderer->enemies));
	mesh_add_attrib_vec3(3, sizeof(Enemy_Drawable), 0); // world pos

	load(&(renderer->shader), "assets/shaders/mesh_uv.vert", "assets/shaders/mesh_uv.frag");
	bind(renderer->shader);
	set_int(renderer->shader, "positions", 0);
	set_int(renderer->shader, "normals"  , 1);
	set_int(renderer->shader, "albedo"   , 2);
	set_int(renderer->shader, "texture_sampler", 5);
}
void update_renderer(Enemy_Renderer* renderer, Enemy* enemies)
{
	uint num_enemies = 0;
	Enemy_Drawable* enemy_mem = renderer->enemies;

	for (uint i = 0; i < MAX_ENEMIES; i++)
	{
		if (enemies[i].type != NULL)
		{
			enemy_mem->position = enemies[i].position;

			num_enemies++;
			enemy_mem++;
		}
	}

	renderer->num_enemies = num_enemies;
	update(renderer->mesh, num_enemies * sizeof(Enemy_Drawable), (byte*)renderer->enemies);
}
