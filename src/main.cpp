#include "level.h"

#define TARGET_FRAMES_PER_SECOND ((float)120)
#define DRAW_DISTANCE 512.0f
#define FOV ToRadians(45.0f)

int main()
{
	Window   window = {};
	Mouse    mouse  = {};
	Keyboard keys   = {};

	init_window(&window, 1920, 1080, "city building game");
	init_keyboard(&keys);

	Camera camera = { vec3(5,5,5) };
	camera.height = 5;

	Level* level = Alloc(Level, 1);
	level->path_nodes[0] = { vec3(0,1,8)   };
	level->path_nodes[1] = { vec3(15,1,8)  };
	level->path_nodes[2] = { vec3(15,1,15) };
	level->path_nodes[3] = { vec3(-1,0,0)  };

	Tile_Renderer*   tile_renderer   = Alloc(Tile_Renderer  , 1);
	Enemy_Renderer*  enemy_renderer  = Alloc(Enemy_Renderer , 1);

	init(tile_renderer);
	init(enemy_renderer);

	G_Buffer g_buffer = {};
	init_g_buffer(&g_buffer, window);
	Shader lighting_shader = make_lighting_shader();
	mat4 proj = glm::perspective(FOV, (float)window.screen_width / window.screen_height, 0.1f, DRAW_DISTANCE);

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

		// game state updates //
		update_level(level, frame_time);

		// rendering updates //
		update_renderer(tile_renderer  , level->tiles);
		update_renderer(enemy_renderer , level->enemies);

		mat4 proj_view = proj * glm::lookAt(camera.position, camera.position + camera.front, camera.up);

		// calculating which tile is selected by the mouse
		vec3 intersect_point = {};
		{
			vec3 mouse_dir = get_mouse_world_dir(mouse, proj_view);
			vec3 p0 = camera.position + vec3(mouse.norm_x, mouse.norm_y, 0);
			float lambda = (1 - p0.y) / mouse_dir.y;

			float x = p0.x + (lambda * mouse_dir.x);
			float z = p0.z + (lambda * mouse_dir.z);

			intersect_point = vec3(x, 1, z);

			if (intersect_point.x > 0 && intersect_point.x < 16 && intersect_point.z > 0 && intersect_point.z < 16)
			{
				int i = TILE_INDEX((int)intersect_point.x, (int)intersect_point.z);
	
			}
		}

		static float enemy_spawn_timer = 4;
		enemy_spawn_timer -= frame_time;
		if (enemy_spawn_timer < 0)
		{
			spawn_enemy(level->enemies, vec3(0, 1, 0));
			enemy_spawn_timer = 4;
		}

		// Geometry pass
		glBindFramebuffer(GL_FRAMEBUFFER, g_buffer.FBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		bind(tile_renderer->tile_shader);
		set_mat4(tile_renderer->tile_shader, "proj_view", proj_view);
		bind_texture(tile_renderer->tile_mesh, 3);
		draw(tile_renderer->tile_mesh, tile_renderer->num_tiles);

		bind(enemy_renderer->shader);
		set_mat4(enemy_renderer->shader, "proj_view", proj_view);
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