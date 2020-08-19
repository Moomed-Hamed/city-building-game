#include "input.h"

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720

#define TARGET_FRAMERATE ((float)120)

//timer if u need 1
//Timestamp start = get_timestamp();
//out(calculate_microseconds_elapsed(start, get_timestamp()));
//stop; return 0;

int main()
{
	/*---------------- initializing GLEW & GLFW ----------------*/

	Game_Window window = {};
	game_window_init(&window, SCREEN_WIDTH, SCREEN_HEIGHT, "City-Building Game");

	/*--------------- Preparing Vertex Buffers ---------------*/
#define FOV ToRadians(45.0f)
#define DRAW_DISTANCE 256.0f

	mat4 proj = glm::perspective(FOV, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, DRAW_DISTANCE);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	/*---------------- Game World Setup --------------*/
	
	Game_Map map = {};
	game_map_init(&map);

	Tile_Renderer tile_renderer = {};
	tile_renderer_init(&tile_renderer);

	// --------------- Game Input --------------- //

	Game_Mouse mouse   = {};
	Game_Keyboard keys = {};
	game_keyboard_init(&keys);

	Camera camera = {};

	// --------------- Timing Logic --------------- //

	float DeltaTime = 1 / TARGET_FRAMERATE;
	int64 target_frame_milliseconds = DeltaTime * 1000.f;

	Timestamp frame_start, frame_end;
	frame_start = get_timestamp();

	while(1)
	{
		glfwPollEvents();
		keyboard_update(&keys, window);
		mouse_update(&mouse, window);

		if (keys.ESC.is_pressed) break; // out of the main game loop

		// camera update
		mat4 proj_view = proj * glm::lookAt(camera.position, camera.position + camera.front, camera.up);
		camera_update_dir(&camera, mouse.dx, mouse.dy);
		mouse.world_dir = get_mouse_world_dir(mouse, proj_view);
		if (keys.W.is_pressed) camera_update_pos(&camera, CAM_FORWARD , 8.0f * DeltaTime);
		if (keys.A.is_pressed) camera_update_pos(&camera, CAM_BACKWARD, 8.0f * DeltaTime);
		if (keys.S.is_pressed) camera_update_pos(&camera, CAM_LEFT    , 8.0f * DeltaTime);
		if (keys.D.is_pressed) camera_update_pos(&camera, CAM_RIGHT   , 8.0f * DeltaTime);

		camera.position = vec3(-8.282368, 14.204948, 5.592616);
		camera.front = vec3(0.756777, -0.650377, 0.065565);
		camera.right = vec3(-0.086313, 0.000000, 0.996268);
		camera.up = vec3(0.647949, 0.759612, 0.056136);
		camera.pitch = -0.708080;
		camera.yaw = 0.086421;

		if (keys.SPACE.is_pressed)
		{
			FILE* write = fopen("camera.txt", "w");

			fprintf(write, "camera.position = vec3(%f, %f, %f);\n", camera.position.x, camera.position.y, camera.position.z);
			fprintf(write, "camera.front = vec3(%f, %f, %f);\n", camera.front.x, camera.front.y, camera.front.z);
			fprintf(write, "camera.right = vec3(%f, %f, %f);\n", camera.right.x, camera.right.y, camera.right.z);
			fprintf(write, "camera.up    = vec3(%f, %f, %f);\n", camera.up.x, camera.up.y, camera.up.z);
			fprintf(write, "camera.pitch = %f;\n", camera.pitch);
			fprintf(write, "camera.yaw   = %f;\n", camera.yaw);

			fclose(write);
		}

		// ----------------- Rendering ----------------- //
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		tile_renderer_draw(&tile_renderer, map.tiles, &proj_view, camera.position);

		// ----------------- Finish Frame ----------------- //
		glfwSwapBuffers(window.instance);

		//Frame Time
		frame_end = get_timestamp();
		int64 milliseconds_elapsed = calculate_milliseconds_elapsed(frame_start, frame_end);

		//print("frame time: %02d ms | fps: %06f\n", milliseconds_elapsed, (1.f / (milliseconds_elapsed) * 1000));
		if (target_frame_milliseconds > milliseconds_elapsed) // frame finished early
		{
			uint sleep_milliseconds = target_frame_milliseconds - milliseconds_elapsed;
			os_sleep(sleep_milliseconds);
		}

		frame_start = get_timestamp();
	}

	glfwTerminate();
	return 0;
}