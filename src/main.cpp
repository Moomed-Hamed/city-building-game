#include "input.h"

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720

#define TARGET_FRAMERATE ((float)120)

#define KeyIsDown(key) (glfwGetKey(window.instance, key) == GLFW_PRESS)

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
	fill_render_data(&render_data, 0, "assets/models/flat_plane.model", VertSource, FragSource);

	/*---------------- Game World Setup --------------*/
	
	Game_World world = {};
	game_world_init(&world);

	World_Renderer world_renderer = {};
	fill_render_data(&world_renderer, WORLD_TILE_COUNT * sizeof(Tile_Renderable), "assets/models/cube.model", TileVertSource, TileFragSource);
	world_renderer.tiles = Alloc(Tile_Renderable, WORLD_TILE_COUNT);

	{
		GLuint pos_attrib = 2;
		glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, sizeof(Tile_Renderable), 0);
		glVertexAttribDivisor(pos_attrib, 1);
		glEnableVertexAttribArray(pos_attrib);

		GLuint col_attrib = 3;
		glVertexAttribPointer(col_attrib, 3, GL_FLOAT, GL_FALSE, sizeof(Tile_Renderable), (void*)sizeof(vec3));
		glVertexAttribDivisor(col_attrib, 1);
		glEnableVertexAttribArray(col_attrib);
	}

	/*---------------- Game Input --------------*/

	Game_Mouse mouse = {};

	Camera camera = {};
	init_camera(&camera);

	for (uint i = 0; i < 16; ++i) {
	for (uint j = 0; j < 16; ++j)
	{
		world.tiles[(i * 16) + j].position = vec3(i, 0, j);
	} }

	/*--------------- Timing Logic ---------------*/

	float DeltaTime = 1 / TARGET_FRAMERATE;
	int64 target_frame_milliseconds = DeltaTime * 1000.f;

	Timestamp frame_start, frame_end;
	frame_start = get_timestamp();

	while (!glfwWindowShouldClose(window.instance))
	{
		glfwPollEvents();

		mouse_update(window, &mouse);

		update_camera(&camera, mouse.dx, mouse.dy);
		camera.Position = vec3(-10.608428, 11.828352, 2.657687);
		camera.Front = vec3(0.804584, -0.544792, 0.236320);
		camera.Right = vec3(-0.281813, 0.000000, 0.959469);
		camera.Up    = vec3(0.522711, 0.838572, 0.153529);
		camera.Pitch = -0.576140;
		camera.Yaw   = 0.285683;

		//what is an unproject matrix?

		if (KeyIsDown(GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window.instance, GL_TRUE);

		if (KeyIsDown(GLFW_KEY_W)) camera_move(&camera, CAMERA_FORWARD , 8.0f, DeltaTime);
		if (KeyIsDown(GLFW_KEY_S)) camera_move(&camera, CAMERA_BACKWARD, 8.0f, DeltaTime);
		if (KeyIsDown(GLFW_KEY_A)) camera_move(&camera, CAMERA_LEFT    , 8.0f, DeltaTime);
		if (KeyIsDown(GLFW_KEY_D)) camera_move(&camera, CAMERA_RIGHT   , 8.0f, DeltaTime);

		if (KeyIsDown(GLFW_KEY_K))
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

		/*-------------------------------- Rendering -----------------------------*/
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		mat4 proj_view = Proj * glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);

		mouse.world_dir = get_mouse_word_dir(mouse, proj_view);
		world_set_selected_tile(mouse, camera.Position, world);

		//draw a flat plane
		draw_render_data(render_data, &proj_view, camera.Position);

		//draw world tiles
		tiles_to_renderable(&world, &world_renderer);
		draw_render_data(world_renderer, &proj_view, camera.Position, WORLD_TILE_COUNT * sizeof(Tile_Renderable),
			(byte*)world_renderer.tiles, world_renderer.num_tiles);

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

	return 0;
}