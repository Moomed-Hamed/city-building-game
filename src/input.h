#pragma once
#include "world.h"

struct Game_Window
{
	GLFWwindow* instance;
	uint screen_width, screen_height;
};

void init_game_window(Game_Window* window, uint width, uint height, const char* name)
{
	window->screen_width = width;
	window->screen_height = height;

	if (!glfwInit()) { out("glfw init error!"); stop; return; }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window->instance = glfwCreateWindow(width, height, name, NULL, NULL);
	if (!window->instance) { glfwTerminate(); out("window error!"); stop; return; }

	glfwMakeContextCurrent(window->instance);
	glfwSwapInterval(1); // disable v-sync

	//Capture the cursor
	//glfwSetInputMode(WindowInstance, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//GLEW
	glewExperimental = GL_TRUE;
	glewInit();
}

struct Game_Mouse
{
	double raw_x, raw_y;   // pixel coordinates
	double norm_x, norm_y; // normalized screen coordinates
	double dx, dy;  // pos change since last frame in pixels
	vec3 world_dir; // direction mouse ray is facing in world space
};

void mouse_update(Game_Window window, Game_Mouse* mouse)
{
	static Game_Mouse prev_mouse = {};

	glfwGetCursorPos(window.instance, &mouse->raw_x, &mouse->raw_y);

	mouse->dx = mouse->raw_x - prev_mouse.raw_x; // what do these mean again?
	mouse->dy = prev_mouse.raw_y - mouse->raw_y;

	prev_mouse.raw_x = mouse->raw_x;
	prev_mouse.raw_y = mouse->raw_y;

	mouse->norm_x = ((uint)mouse->raw_x % window.screen_width)  / (double)window.screen_width;
	mouse->norm_y = ((uint)mouse->raw_y % window.screen_height) / (double)window.screen_height;

	mouse->norm_y = 1 - mouse->norm_y;
	mouse->norm_x = (mouse->norm_x * 2) - 1;
	mouse->norm_y = (mouse->norm_y * 2) - 1;
}

vec3 get_mouse_word_dir(Game_Mouse mouse, mat4 proj_view)
{
	proj_view = glm::inverse(proj_view);

	vec4 ray_near = vec4(mouse.norm_x, mouse.norm_y, -1, 1); // near plane is z = -1
	vec4 ray_far  = vec4(mouse.norm_x, mouse.norm_y,  0, 1);

	// these are actually using inverse_proj_view
	ray_near = proj_view * ray_near; ray_near /= ray_near.w;
	ray_far  = proj_view * ray_far;  ray_far  /= ray_far.w;

	return glm::normalize(ray_far - ray_near);
}

void world_set_selected_tile(Game_Mouse mouse, vec3 camera_pos, Game_World world)
{
	// WARNING TODO : please speed this up thanks

	vec3 test_pos = camera_pos;

	static uint last_selected_tile_index = 0;
	world.tiles[last_selected_tile_index].is_selected = false;

	for (uint i = 0; i < 128; ++i)
	{
		for (uint x = 0;  x < TILE_COUNT_X; ++x) {
		for (uint z = 0;  z < TILE_COUNT_Z; ++z)
		{
			uint index = (x * 16) + z;
			if (point_in_cube(test_pos, Cube_Collider{ ivec3(x, 0, z), ivec3(x + 1, 1, z + 1) }))
			{
				world.tiles[index].is_selected = true;
				last_selected_tile_index = index;
				return;
			}
		} }

		test_pos += mouse.world_dir * .5f;
	}
}