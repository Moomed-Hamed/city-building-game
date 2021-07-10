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

	Camera camera = {};
	camera.height = 3;
	camera.theta = PI;
	camera.position = vec3(16, 3, 16);

	Level* level = Alloc(Level, 1);

	Level_Renderer* level_renderer = Alloc(Level_Renderer, 1);
	init(level_renderer);

	Cursor_Renderer* cursor_renderer = Alloc(Cursor_Renderer, 1);
	init(cursor_renderer);

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
		
		if (keys.W.is_pressed) camera_update_pos(&camera, DIR_FORWARD , 10 * frame_time);
		if (keys.S.is_pressed) camera_update_pos(&camera, DIR_BACKWARD, 10 * frame_time);
		if (keys.A.is_pressed) camera_update_pos(&camera, DIR_LEFT    , 10 * frame_time);
		if (keys.D.is_pressed) camera_update_pos(&camera, DIR_RIGHT   , 10 * frame_time);
		if (camera.position.x < 32) camera.position.x = 32;
		if (camera.position.z < 32) camera.position.z = 32;
		if(!keys.SHIFT.is_pressed) camera_update_dir(&camera, mouse.dx, mouse.dy);

		mat4 proj_view = proj * glm::lookAt(camera.position, camera.position + camera.front, camera.up);

		// calculating which tile is selected by the mouse
		vec3 intersect_point = {};
		{
			vec3 mouse_dir = get_mouse_world_dir(mouse, proj_view);
			vec3 p0 = camera.position + vec3(mouse.norm_x, mouse.norm_y, 0);
			float lambda = (1 - p0.y) / mouse_dir.y;

			float x = p0.x + (lambda * mouse_dir.x);
			float z = p0.z + (lambda * mouse_dir.z);

			x = (int)x;
			z = (int)z;
			if ((int)z % 2) x += .5;

			intersect_point = vec3(x, .8, z);
		}

		// game state updates //
		update_level(level, camera.position, frame_time);

		// rendering updates //
		update_renderer(level_renderer , level, frame_time);
		update_renderer(cursor_renderer, intersect_point);

		// Geometry pass
		glBindFramebuffer(GL_FRAMEBUFFER, g_buffer.FBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		draw(level_renderer , proj_view);
		draw(cursor_renderer, proj_view);

		// Lighting pass
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		bind(lighting_shader);
		static float theta = 0;
		theta += frame_time * TWOPI * .1;
		set_vec3(lighting_shader, "dir_light.direction", vec3(sin(theta), -0.6, cos(theta)));
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