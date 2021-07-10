#include "camera.h"

struct Cursor_Drawable
{
	vec3 position;
	mat3 rotation;
};

struct Cursor_Renderer
{
	Drawable_Mesh_UV mesh;
	Shader shader;
};

void init(Cursor_Renderer* renderer)
{
	load(&renderer->mesh, "assets/meshes/cursor.mesh_uv", "assets/textures/palette.bmp", sizeof(Cursor_Drawable));
	mesh_add_attrib_vec3(3, sizeof(Cursor_Drawable), 0); // world pos
	mesh_add_attrib_mat3(4, sizeof(Cursor_Drawable), sizeof(vec3)); // rotation

	load(&(renderer->shader), "assets/shaders/mesh_uv_rot.vert", "assets/shaders/mesh_uv.frag");
	bind(renderer->shader);
	set_int(renderer->shader, "positions", 0);
	set_int(renderer->shader, "normals"  , 1);
	set_int(renderer->shader, "albedo"   , 2);
	set_int(renderer->shader, "texture_sampler", 4);
}
void update_renderer(Cursor_Renderer* renderer, vec3 position)
{
	Cursor_Drawable cursor = {};

	cursor.position = position;
	cursor.rotation = mat3(1.f);

	update(renderer->mesh, sizeof(Cursor_Drawable), (byte*)(&cursor));
}
void draw(Cursor_Renderer* renderer, mat4 proj_view)
{
	bind(renderer->shader);
	set_mat4(renderer->shader, "proj_view", proj_view);
	bind_texture(renderer->mesh, 4);
	draw(renderer->mesh, 1);
}