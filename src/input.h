#pragma once
#include "units.h"

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

void update_mouse(Game_Mouse* mouse, Game_Window window)
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

vec3 get_mouse_world_dir(Game_Mouse mouse, mat4 proj_view)
{
	proj_view = glm::inverse(proj_view); //what is an unproject matrix?

	vec4 ray_near = vec4(mouse.norm_x, mouse.norm_y, -1, 1); // near plane is z = -1
	vec4 ray_far  = vec4(mouse.norm_x, mouse.norm_y,  0, 1);

	// these are actually using inverse(proj_view)
	ray_near = proj_view * ray_near; ray_near /= ray_near.w;
	ray_far  = proj_view * ray_far;  ray_far  /= ray_far.w;

	return glm::normalize(ray_far - ray_near);
}

struct Game_Button
{
	char is_pressed;
	char was_pressed;
	u16  id;
};

#define NUM_GAME_KEYBOARD_BUTTONS 6
struct Game_Keyboard
{
	union
	{
		Game_Button buttons[NUM_GAME_KEYBOARD_BUTTONS];

		struct
		{
			Game_Button ESC, SPACE;
			Game_Button W, A, S, D;
		};
	};
};

force_inline void game_keyboard_init(Game_Keyboard* keyboard)
{
	keyboard->ESC   = { false, false, GLFW_KEY_ESCAPE };
	keyboard->SPACE = { false, false, GLFW_KEY_SPACE  };

	keyboard->W = { false, false, GLFW_KEY_W };
	keyboard->A = { false, false, GLFW_KEY_S };
	keyboard->S = { false, false, GLFW_KEY_A };
	keyboard->D = { false, false, GLFW_KEY_D };
}

void update_keyboard(Game_Keyboard* keyboard, Game_Window window)
{
	for (uint i = 0; i < NUM_GAME_KEYBOARD_BUTTONS; ++i)
	{
		bool is_pressed = (glfwGetKey(window.instance, keyboard->buttons[i].id) == GLFW_PRESS);

		if (is_pressed)
		{
			keyboard->buttons[i].was_pressed = (keyboard->buttons[i].is_pressed == true);
			keyboard->buttons[i].is_pressed = true;
		}
		else
		{
			keyboard->buttons[i].was_pressed = (keyboard->buttons[i].is_pressed == true);
			keyboard->buttons[i].is_pressed  = false;
		}
	}
}