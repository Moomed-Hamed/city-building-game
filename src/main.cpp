#include "level.h"

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
	init_game_window(&window, SCREEN_WIDTH, SCREEN_HEIGHT, "City-Building Game");

	/*--------------- Preparing Vertex Buffers ---------------*/
#define FOV ToRadians(45.0f)
#define DRAW_DISTANCE 256.0f

	mat4 Proj = glm::perspective(FOV, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, DRAW_DISTANCE);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// flat plane setup
	Render_Data render_data = {};
	init(&render_data, 0, "assets/models/flat_plane.model", VertSource, FragSource);

	/*---------------- Game World Setup --------------*/
	
	Game_Level level = make_game_level(16, 16);
	spawn_friendly_unit(&level, 1, 1);

	Level_Renderer level_renderer = {};
	level_renderer.terrain.blocks = Alloc(Terrain_Block_Renderable, 256);
	level_renderer.friendly_units.units = Alloc(Unit_Renderable, MAX_UNITS);
	level_renderer.enemy_units.units    = Alloc(Unit_Renderable, MAX_UNITS);
	level_renderer.props.props = Alloc(Prop_Renderable, MAX_UNITS);

	init(&level_renderer.terrain, 256 * sizeof(Terrain_Block_Renderable), "assets/models/cube.model", TileVertSource, TileFragSource);
	gl_add_attrib_vec3(2, sizeof(Terrain_Block_Renderable), 0);
	gl_add_attrib_vec3(3, sizeof(Terrain_Block_Renderable), sizeof(vec3));

	init(&level_renderer.friendly_units, 256 * sizeof(Unit_Renderable), "assets/models/cube.model", TileVertSource, TileFragSource);
	gl_add_attrib_vec3(2, sizeof(Unit_Renderable), 0);
	gl_add_attrib_vec3(3, sizeof(Unit_Renderable), sizeof(vec3));

	init(&level_renderer.enemy_units, 256 * sizeof(Unit_Renderable), "assets/models/cube.model", TileVertSource, TileFragSource);
	gl_add_attrib_vec3(2, sizeof(Unit_Renderable), 0);
	gl_add_attrib_vec3(3, sizeof(Unit_Renderable), sizeof(vec3));

	init(&level_renderer.props, 256 * sizeof(Prop_Renderable), "assets/models/cube.model", TileVertSource, TileFragSource);
	gl_add_attrib_vec3(2, sizeof(Prop_Renderable), 0);
	gl_add_attrib_vec3(3, sizeof(Prop_Renderable), sizeof(vec3));

	/*---------------- Game Input --------------*/

	Game_Mouse mouse   = {};
	Game_Keyboard keys = {};
	game_keyboard_init(&keys);

	Camera camera = {};
	init_camera(&camera);

	/*--------------- Timing Logic ---------------*/

	float DeltaTime = 1 / TARGET_FRAMERATE;
	int64 target_frame_milliseconds = DeltaTime * 1000.f;

	Timestamp frame_start, frame_end;
	frame_start = get_timestamp();

	while(1)
	{
		glfwPollEvents();
		update_keyboard(&keys, window);
		update_mouse(&mouse, window);

		mat4 proj_view = Proj * glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);

		mouse.world_dir = get_mouse_world_dir(mouse, proj_view);

		// update the camera
		update_camera(&camera, mouse.dx, mouse.dy);
		camera.Position = vec3(-10.608428, 11.828352, 2.657687);
		camera.Front = vec3( 0.804584, -0.544792, 0.236320);
		camera.Right = vec3(-0.281813,  0.000000, 0.959469);
		camera.Up    = vec3( 0.522711,  0.838572, 0.153529);
		camera.Pitch = -0.576140;
		camera.Yaw   =  0.285683;

		if (keys.ESC.is_pressed) break; // out of the main game loop
		if (keys.W.is_pressed) camera_move(&camera, CAMERA_FORWARD , 8.0f, DeltaTime);
		if (keys.A.is_pressed) camera_move(&camera, CAMERA_BACKWARD, 8.0f, DeltaTime);
		if (keys.S.is_pressed) camera_move(&camera, CAMERA_LEFT    , 8.0f, DeltaTime);
		if (keys.D.is_pressed) camera_move(&camera, CAMERA_RIGHT   , 8.0f, DeltaTime);
		if (keys.SPACE.is_pressed)
		{
			FILE* write = fopen("camera.txt", "w");

			fprintf(write, "camera.Position = vec3(%f, %f, %f);\n", camera.Position.x, camera.Position.y, camera.Position.z);
			fprintf(write, "camera.Front = vec3(%f, %f, %f);\n", camera.Front.x, camera.Front.y, camera.Front.z);
			fprintf(write, "camera.Right = vec3(%f, %f, %f);\n", camera.Right.x, camera.Right.y, camera.Right.z);
			fprintf(write, "camera.Up    = vec3(%f, %f, %f);\n", camera.Up.x, camera.Up.y, camera.Up.z);
			fprintf(write, "camera.Pitch = %f;\n", camera.Pitch);
			fprintf(write, "camera.Yaw   = %f;\n", camera.Yaw);

			fclose(write);
		}
		
		// input has been gathered, now update the world
		//terrain.selected_tile_index = world_get_selected_tile(mouse, camera.Position, terrain);

		/*-------------------------------- Rendering -----------------------------*/
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//draw flat plane
		draw(render_data, &proj_view, camera.Position);

		//draw world tiles
		level_to_renderable(level, &level_renderer);
		draw_level(level_renderer, &proj_view, camera.Position);

		/*------------------------------- Finish Frame -------------------------------*/
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