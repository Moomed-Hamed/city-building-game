#include "tiles.h"

#define MAX_BUILDINGS 64

struct Building
{
	uint type;
	vec3 position;
};

void spawn_building(Building* buildings, vec3 pos, uint type = 1)
{
	for (uint i = 0; i < MAX_BUILDINGS; i++)
	{
		if (buildings[i].type == NULL)
		{
			buildings[i].type = type;
			buildings[i].position = pos;
			return;
		}
	}
}

void update_buildings(Building* buildings)
{
	for (uint i = 0; i < MAX_BUILDINGS; i++)
	{
		if (buildings[i].type != NULL)
		{
			// do stuff
		}
	}
}

// rendering

struct Building_Drawable
{
	vec3 position;
	vec3 color;
};

struct Building_Renderer
{
	uint num_buildings;
	Building_Drawable buildings[MAX_BUILDINGS];

	Drawable_Mesh mesh;
	Shader shader;
};

void init(Building_Renderer* renderer)
{
	load(&renderer->mesh, "assets/meshes/office.mesh", sizeof(renderer->buildings));
	mesh_add_attrib_vec3(2, sizeof(Building_Drawable), 0); // world pos
	mesh_add_attrib_vec3(3, sizeof(Building_Drawable), sizeof(vec3)); // color

	load(&(renderer->shader), "assets/shaders/cell.vert", "assets/shaders/cell.frag");
	bind(renderer->shader);
	set_int(renderer->shader, "positions", 0);
	set_int(renderer->shader, "normals"  , 1);
	set_int(renderer->shader, "albedo"   , 2);
}
void update_renderer(Building_Renderer* renderer, Building* enemies)
{
	uint num_buildings = 0;
	Building_Drawable* building_mem = renderer->buildings;

	for (uint i = 0; i < MAX_BUILDINGS; i++)
	{
		if (enemies[i].type != NULL)
		{
			building_mem->position = enemies[i].position + vec3(.5, 0, .5);
			building_mem->color = vec3(0, 1, 0);

			num_buildings++;
			building_mem++;
		}
	}

	renderer->num_buildings = num_buildings;
	update(renderer->mesh, num_buildings * sizeof(Building_Drawable), (byte*)renderer->buildings);
}