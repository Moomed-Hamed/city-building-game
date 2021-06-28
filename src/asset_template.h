#include "tiles.h"

#define MAX_ASSETS 64

struct Asset
{
	uint type;
	vec3 position;
};

void spawn_asset(Asset* assets, vec3 pos, uint type = 1)
{
	for (uint i = 0; i < MAX_ASSETS; i++)
	{
		if (assets[i].type == NULL)
		{
			assets[i].type = type;
			assets[i].position = pos;
			return;
		}
	}
}

void update_assets(Asset* assets)
{
	for (uint i = 0; i < MAX_ASSETS; i++)
	{
		if (assets[i].type != NULL)
		{
			// do stuff
		}
	}
}

// rendering

struct Asset_Drawable
{
	vec3 position;
	vec3 color;
};

struct Asset_Renderer
{
	uint num_assets;
	Asset_Drawable assets[MAX_ASSETS];

	Drawable_Mesh mesh;
	Shader shader;
};

void init(Asset_Renderer* renderer)
{
	load(&renderer->mesh, "assets/meshes/asset.mesh", sizeof(renderer->assets));
	mesh_add_attrib_vec3(2, sizeof(Asset_Drawable), 0); // world pos
	mesh_add_attrib_vec3(3, sizeof(Asset_Drawable), sizeof(vec3)); // color

	load(&(renderer->shader), "assets/shaders/cell.vert", "assets/shaders/cell.frag");
	bind(renderer->shader);
	set_int(renderer->shader, "positions", 0);
	set_int(renderer->shader, "normals"  , 1);
	set_int(renderer->shader, "albedo"   , 2);
}
void update_renderer(Asset_Renderer* renderer, Asset* enemies)
{
	uint num_assets = 0;
	Asset_Drawable* asset_mem = renderer->assets;

	for (uint i = 0; i < MAX_ASSETS; i++)
	{
		if (enemies[i].type != NULL)
		{
			asset_mem->position = enemies[i].position + vec3(.5, 0, .5);
			asset_mem->color = vec3(0, 1, 0);

			num_assets++;
			asset_mem++;
		}
	}

	renderer->num_assets = num_assets;
	update(renderer->mesh, num_assets * sizeof(Asset_Drawable), (byte*)renderer->assets);
}