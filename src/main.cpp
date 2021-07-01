#include "level.h"

#define TARGET_FRAMES_PER_SECOND ((float)120)
#define DRAW_DISTANCE 512.0f
#define FOV ToRadians(45.0f)

int main()
{
	Window   window = {};
	Mouse    mouse  = {};
	Keyboard keys   = {};

	init_window(&window, 1920, 1080, "tower defense game");
	init_keyboard(&keys);

	Camera camera = { vec3(5, 5, 5) };
	camera.height = 3;

	Level* level = Alloc(Level, 1);
	level->path_nodes[0] = { vec3(0, 1, 8.5)     };
	level->path_nodes[1] = { vec3(15.5, 1, 8.5)  };
	level->path_nodes[2] = { vec3(15.5, 1, 15.5) };
	level->path_nodes[3] = { vec3(-.5, 0, 0.5)   };

	Tile_Renderer*   tile_renderer   = Alloc(Tile_Renderer  , 1);

	init(tile_renderer);

	G_Buffer g_buffer = {};
	init_g_buffer(&g_buffer, window);
	Shader lighting_shader = make_lighting_shader();
	mat4 proj = glm::perspective(FOV, (float)window.screen_width / window.screen_height, 0.1f, DRAW_DISTANCE);

	test_gen(level);

	Enemy* enemies = Alloc(Enemy, MAX_ENEMIES);
	Enemy_Renderer* enemy_renderer = Alloc(Enemy_Renderer, 1);
	init(enemy_renderer);
	//spawn_enemy(enemies, vec3(1, .15, 0.5));

	// frame timer
	float frame_time = 1.f / 60;
	int64 target_frame_milliseconds = frame_time * 1000.f;
	Timestamp frame_start = get_timestamp(), frame_end;

	while (1)
	{
		update_window(window);
		update_mouse(&mouse, window);
		update_keyboard(&keys, window);

		if (keys.ESC.is_pressed) break;

		// camera control
		if (mouse.left_button.is_pressed && mouse.left_button.was_pressed && keys.SHIFT.is_pressed)
		{
			camera_pan(&camera, mouse.dx, mouse.dy, frame_time);
		}
		else if (mouse.left_button.is_pressed && mouse.left_button.was_pressed)
		{
			camera_rotate(&camera, mouse.dx, mouse.dy, frame_time);
		}

		camera_update(&camera);

		if (keys.F.is_pressed) // flashlight
		{
			bind(lighting_shader);
			set_vec3(lighting_shader, "spt_light.position" , camera.position);
			set_vec3(lighting_shader, "spt_light.direction", camera.front);
		}

		if (keys.X.is_pressed && !keys.X.was_pressed)
		{
			static float flat = 0;
			flat += .2;
			out(flat <<  '=' << perlin(flat, flat) * 5);
			test_gen(level, 0, flat);
		}

		if (keys.R.is_pressed && !keys.R.was_pressed)
		{
			static uint offset = 0;
			offset += 1;
			test_gen(level, offset, 4);
		}

		// game state updates //
		update_level(level, frame_time);

		// rendering updates //
		update_renderer(tile_renderer, level->tiles, frame_time);
		update_renderer(enemy_renderer, enemies);

		mat4 proj_view = proj * glm::lookAt(camera.position, camera.position + camera.front, camera.up);

		// Geometry pass
		glBindFramebuffer(GL_FRAMEBUFFER, g_buffer.FBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		bind(tile_renderer->land_shader);
		set_mat4(tile_renderer->land_shader, "proj_view", proj_view);
		bind_texture(tile_renderer->land_mesh, 3);
		draw(tile_renderer->land_mesh, tile_renderer->num_land_tiles);

		bind(tile_renderer->fluid_shader);
		set_mat4(tile_renderer->fluid_shader, "proj_view", proj_view);
		set_float(tile_renderer->fluid_shader, "timer", tile_renderer->fluid_timer);
		bind_texture(tile_renderer->fluid_mesh, 3);
		draw(tile_renderer->fluid_mesh, tile_renderer->num_fluid_tiles);

		bind(enemy_renderer->shader);
		set_mat4(enemy_renderer->shader, "proj_view", proj_view);
		bind_texture(enemy_renderer->mesh, 5);
		draw(enemy_renderer->mesh, enemy_renderer->num_enemies);

		// Lighting pass
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		bind(lighting_shader);
		set_vec3(lighting_shader, "view_pos", camera.position);
		draw_g_buffer(g_buffer);

		//Frame Time
		frame_end = get_timestamp();
		int64 milliseconds_elapsed = calculate_milliseconds_elapsed(frame_start, frame_end);

		//print("frame time: %02d ms | fps: %06f\n", milliseconds_elapsed, 1000.f / milliseconds_elapsed);
		if (target_frame_milliseconds > milliseconds_elapsed) // frame finished early
		{
			os_sleep(target_frame_milliseconds - milliseconds_elapsed);
		}
		
		frame_start = frame_end;
	}

	shutdown_window();
	return 0;
}