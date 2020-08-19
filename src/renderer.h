#pragma once
#include "intermediary.h"

/*
 'model' binary file format
  - (uint)  num_verts
  - (uint)  num_indicies
  ~ (uint)  num_joints
  - (vec3)  positions
  - (vec3)  normals
  ~ (vec2)  textures
  ~ (vec3)  weights
  ~ (ivec3) joint_ids
  - (uint)  indicies
  ~ (int)   joint_parent_indicies
  ~ (mat4)  inverse-bind-matrices
*/

void debug_shader(const char* path, GLint vert, GLint frag)
{
	GLint logSize = 0;
	glGetShaderiv(vert, GL_INFO_LOG_LENGTH, &logSize);
	if (logSize)
	{
		char *ErrorLog = Alloc(char, logSize); ZeroMemory(ErrorLog, logSize);
		glGetShaderInfoLog(vert, logSize, NULL, ErrorLog);
		out(path << " Vertex Shader ERROR\n" << ErrorLog);
		Free(ErrorLog);
	} //else out(path << " Vert: Good");

	logSize = 0;
	glGetShaderiv(frag, GL_INFO_LOG_LENGTH, &logSize);
	if (logSize)
	{
		char *ErrorLog = Alloc(char, logSize); ZeroMemory(ErrorLog, logSize);
		glGetShaderInfoLog(frag, logSize, NULL, ErrorLog);
		out(path << " Fragment Shader ERROR\n" << ErrorLog);
		Free(ErrorLog);
	} //else out(path << " Frag: Good");
}

struct Model_Data
{
	uint num_verts, num_indicies;

	vec3* positions;
	vec3* normals;
	vec3* indicies;
};

struct Model_Data_UV
{
	uint num_verts, num_indicies;

	vec3* positions;
	vec3* normals;
	vec2* textures;
	uint* indicies;
};

void load_model(Model_Data* model_data, const char* path)
{
	FILE* model_file = fopen(path, "rb");
	if (!model_file) { print("could not open model file : %s\n", path); stop; return; }

	fread(&model_data->num_verts, sizeof(uint), 1, model_file);
	fread(&model_data->num_indicies, sizeof(uint), 1, model_file);

	model_data->positions = Alloc(vec3, model_data->num_verts);
	model_data->normals   = Alloc(vec3, model_data->num_verts);
	model_data->indicies  = Alloc(vec3, model_data->num_indicies);

	fread(model_data->positions, sizeof(vec3), model_data->num_verts, model_file);
	fread(model_data->normals  , sizeof(vec3), model_data->num_verts, model_file);
	fread(model_data->indicies , sizeof(uint), model_data->num_indicies, model_file);
}
void load_model(Model_Data_UV* model_data, const char* path)
{
	FILE* model_file = fopen(path, "rb");
	if (!model_file) { print("could not open model file: %s\n", path); stop; return; }

	fread(&model_data->num_verts   , sizeof(uint), 1, model_file);
	fread(&model_data->num_indicies, sizeof(uint), 1, model_file);

	model_data->positions = Alloc(vec3, model_data->num_verts);
	model_data->normals   = Alloc(vec3, model_data->num_verts);
	model_data->textures  = Alloc(vec2, model_data->num_verts);
	model_data->indicies  = Alloc(uint, model_data->num_indicies);

	fread(model_data->positions, sizeof(vec3), model_data->num_verts, model_file);
	fread(model_data->normals  , sizeof(vec3), model_data->num_verts, model_file);
	fread(model_data->textures , sizeof(vec2), model_data->num_verts, model_file);
	fread(model_data->indicies , sizeof(uint), model_data->num_indicies, model_file);

	fclose(model_file);
}

struct Render_Data
{
	GLuint VAO, VBO, EBO;
	GLuint shader_program;
	GLuint proj_view, view_pos;
	uint num_indicies;
};

struct Render_Data_UV
{
	GLuint VAO, VBO, EBO;
	GLuint shader_program;
	GLuint texture_sampler, texture_id;
	GLuint proj_view, view_pos;
	uint num_indicies;
};

void render_data_init(Render_Data* renderdata, uint reserved_mem_size, const char* path, const char* vertsource, const char* fragsource)
{
	Model_Data model_data;
	load_model(&model_data, path);
	renderdata->num_indicies = model_data.num_indicies;

	glGenVertexArrays(1, &renderdata->VAO);
	glBindVertexArray(renderdata->VAO);

	uint vertmemsize = model_data.num_verts * sizeof(vec3);
	uint offset = reserved_mem_size;

#define RENDER_MEM_SIZE (reserved_mem_size + (model_data.num_verts * sizeof(vec3) * 2)) // '2' for positions & normals
	glGenBuffers(1, &renderdata->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, renderdata->VBO);
	glBufferData(GL_ARRAY_BUFFER, RENDER_MEM_SIZE, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, offset, vertmemsize, model_data.positions);
	glBufferSubData(GL_ARRAY_BUFFER, offset + vertmemsize, vertmemsize, model_data.normals);
#undef RENDER_MEM_SIZE

	glGenBuffers(1, &renderdata->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderdata->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model_data.num_indicies * sizeof(uint), model_data.indicies, GL_STATIC_DRAW);

	Free(model_data.positions);
	Free(model_data.normals);
	Free(model_data.indicies);

	offset = reserved_mem_size;
	{
		GLint VertAttrib = 0; // position of a vertex
		glVertexAttribPointer(VertAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)offset);
		glEnableVertexAttribArray(VertAttrib);

		GLint NormAttrib = 1; offset += vertmemsize; // normal of a vertex
		glVertexAttribPointer(NormAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)offset);
		glEnableVertexAttribArray(NormAttrib);
	}

	GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_shader, 1, &vertsource, NULL);
	glCompileShader(vert_shader);

	GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &fragsource, NULL);
	glCompileShader(frag_shader);

	debug_shader(path, vert_shader, frag_shader);

	renderdata->shader_program = glCreateProgram();
	glAttachShader(renderdata->shader_program, vert_shader);
	glAttachShader(renderdata->shader_program, frag_shader);
	glBindFragDataLocation(renderdata->shader_program, 0, "FragColor");
	glLinkProgram(renderdata->shader_program);
	glUseProgram(renderdata->shader_program);

	renderdata->proj_view = glGetUniformLocation(renderdata->shader_program, "ProjView");
	renderdata->view_pos = glGetUniformLocation(renderdata->shader_program, "ViewPos");
}
void render_data_init(Render_Data_UV* renderdata, uint reserved_mem_size, const char* path, const char* vertsource, const char* fragsource, const char* texture_path, uint tex_unit = 0)
{
	Model_Data_UV model_data = {};
	load_model(&model_data, path);
	renderdata->num_indicies = model_data.num_indicies;

	glGenVertexArrays(1, &renderdata->VAO);
	glBindVertexArray(renderdata->VAO);

	uint vertmemsize = model_data.num_verts * sizeof(vec3);
	uint texmemsize  = model_data.num_verts * sizeof(vec2);
	uint offset = reserved_mem_size;

#define RENDER_MEM_SIZE (reserved_mem_size + (vertmemsize * 2) + texmemsize)
	glGenBuffers(1, &renderdata->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, renderdata->VBO);
	glBufferData(GL_ARRAY_BUFFER, RENDER_MEM_SIZE, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, offset, vertmemsize, model_data.positions);
	glBufferSubData(GL_ARRAY_BUFFER, offset + vertmemsize, vertmemsize, model_data.normals);
	glBufferSubData(GL_ARRAY_BUFFER, offset + vertmemsize + vertmemsize, texmemsize, model_data.textures);
#undef RENDER_MEM_SIZE

	glGenBuffers(1, &renderdata->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderdata->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model_data.num_indicies * sizeof(uint), model_data.indicies, GL_STATIC_DRAW);

	Free(model_data.positions);
	Free(model_data.normals);
	Free(model_data.textures);
	Free(model_data.indicies);

	offset = reserved_mem_size;
	{
		GLint vert_attrib = 0; // position
		glVertexAttribPointer(vert_attrib, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)offset);
		glEnableVertexAttribArray(vert_attrib);

		GLint norm_attrib = 1; offset += vertmemsize; // normal
		glVertexAttribPointer(norm_attrib, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)offset);
		glEnableVertexAttribArray(norm_attrib);

		GLint tex_attrib = 2; offset += vertmemsize; // texture coordinate
		glVertexAttribPointer(tex_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)offset);
		glEnableVertexAttribArray(tex_attrib);
	}

	GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_shader, 1, &vertsource, NULL);
	glCompileShader(vert_shader);

	GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &fragsource, NULL);
	glCompileShader(frag_shader);

	debug_shader(path, vert_shader, frag_shader);

	renderdata->shader_program = glCreateProgram();
	glAttachShader(renderdata->shader_program, vert_shader);
	glAttachShader(renderdata->shader_program, frag_shader);
	glBindFragDataLocation(renderdata->shader_program, 0, "FragColor");
	glLinkProgram(renderdata->shader_program);
	glUseProgram(renderdata->shader_program);

	renderdata->texture_sampler = glGetUniformLocation(renderdata->shader_program, "Texture");
	renderdata->proj_view       = glGetUniformLocation(renderdata->shader_program, "ProjView");
	renderdata->view_pos        = glGetUniformLocation(renderdata->shader_program, "ViewPos");

	int width, height, num_channels;
	byte* image;

	stbi_set_flip_vertically_on_load(true);

	image = stbi_load(texture_path, &width, &height, &num_channels, 0);

	glGenTextures(1, &renderdata->texture_id);
	glActiveTexture(GL_TEXTURE0 + tex_unit);
	glBindTexture(GL_TEXTURE_2D, renderdata->texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(image);

	glUniform1i(renderdata->texture_sampler, tex_unit);
}

void render_data_draw(Render_Data data, mat4* projview, vec3 viewpos, uint vb_size = NULL, byte* vb_data = NULL, uint num_instances = 1)
{
	glBindVertexArray(data.VAO);

	glUseProgram(data.shader_program);
	glUniformMatrix4fv(data.proj_view, 1, GL_FALSE, (float*)projview);
	glUniform3f(data.view_pos, viewpos.x, viewpos.y, viewpos.z);

	if (vb_data)
	{
		glBindBuffer(GL_ARRAY_BUFFER, data.VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vb_size, vb_data);
	}

	glDrawElementsInstanced(GL_TRIANGLES, data.num_indicies, GL_UNSIGNED_INT, 0, num_instances);
}
void render_data_draw(Render_Data_UV data, mat4* projview, vec3 viewpos, uint vb_size = NULL, byte* vb_data = NULL, uint num_instances = 1)
{
	glBindVertexArray(data.VAO);
	glBindTexture(GL_TEXTURE_2D, data.texture_id);

	glUseProgram(data.shader_program);
	glUniformMatrix4fv(data.proj_view, 1, GL_FALSE, (float*)projview);
	glUniform3f(data.view_pos, viewpos.x, viewpos.y, viewpos.z);

	if (vb_data)
	{
		glBindBuffer(GL_ARRAY_BUFFER, data.VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vb_size, vb_data);
	}

	glDrawElementsInstanced(GL_TRIANGLES, data.num_indicies, GL_UNSIGNED_INT, 0, num_instances);
}

// 2D rendering

struct Render_Data_2D
{
	GLuint VAO, VBO, EBO;
	GLuint shader_program;
};

struct Render_Data_2D_UV
{
	GLuint VAO, VBO, EBO;
	GLuint shader_program;
	GLuint texture_sampler, texture_id;
};

void render_data_init(Render_Data_2D* renderdata, uint reserved_mem_size, const char* vertsource, const char* fragsource)
{
	float verts[] = {
		// X     Y
		-.5f, -.5f, // 0  1-------3
		-.5f, 0.5f, // 1  |       |
		0.5f, -.5f, // 2  |       |
		0.5f, 0.5f  // 3  0-------2
	};

	uint indicies[] = {
		0,2,3,
		3,1,0
	};

	uint offset = reserved_mem_size;

	glGenVertexArrays(1, &renderdata->VAO);
	glBindVertexArray(renderdata->VAO);

#define RENDER_MEM_SIZE (reserved_mem_size + sizeof(verts))
	glGenBuffers(1, &renderdata->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, renderdata->VBO);
	glBufferData(GL_ARRAY_BUFFER, RENDER_MEM_SIZE, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(verts), verts);
#undef RENDER_MEM_SIZE

	glGenBuffers(1, &renderdata->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderdata->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

	offset = reserved_mem_size;
	{
		GLint vert_attrib = 0; // position of a vertex
		glVertexAttribPointer(vert_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)offset);
		glEnableVertexAttribArray(vert_attrib);
	}

	GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_shader, 1, &vertsource, NULL);
	glCompileShader(vert_shader);

	GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &fragsource, NULL);
	glCompileShader(frag_shader);

	debug_shader("2D model: ", vert_shader, frag_shader);

	renderdata->shader_program = glCreateProgram();
	glAttachShader(renderdata->shader_program, vert_shader);
	glAttachShader(renderdata->shader_program, frag_shader);
	glBindFragDataLocation(renderdata->shader_program, 0, "FragColor");
	glLinkProgram(renderdata->shader_program);
	glUseProgram(renderdata->shader_program);
}
void render_data_init(Render_Data_2D_UV* renderdata, uint reserved_mem_size, const char* vertsource, const char* fragsource, const char* texture_path, uint tex_unit = 0)
{
	float verts[] = {
		// X     Y
		-.5f, -.5f, // 0  1-------3
		-.5f,  .5f, // 1  |       |
		 .5f, -.5f, // 2  |       |
		 .5f,  .5f  // 3  0-------2
	};

	float tex_coords[]{
		// X     Y
		0.f, 0.f, // 0  1-------3
		0.f, 1.f, // 1  |       |
		1.f, 0.f, // 2  |       |
		1.f, 1.f  // 3  0-------2
	};

	uint indicies[] = {
		0,2,3,
		3,1,0
	};

	uint offset = reserved_mem_size;

	glGenVertexArrays(1, &renderdata->VAO);
	glBindVertexArray(renderdata->VAO);

#define RENDER_MEM_SIZE (reserved_mem_size + sizeof(verts) + sizeof(tex_coords))
	glGenBuffers(1, &renderdata->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, renderdata->VBO);
	glBufferData(GL_ARRAY_BUFFER, RENDER_MEM_SIZE, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(verts), verts);
	glBufferSubData(GL_ARRAY_BUFFER, offset + sizeof(verts), sizeof(tex_coords), tex_coords);
#undef RENDER_MEM_SIZE

	glGenBuffers(1, &renderdata->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderdata->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

	offset = reserved_mem_size;
	{
		GLint vert_attrib = 0; // vertex position
		glVertexAttribPointer(vert_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)offset);
		glEnableVertexAttribArray(vert_attrib);

		offset += sizeof(verts);
		GLint tex_attrib = 1; // texture coordinates
		glVertexAttribPointer(tex_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)offset);
		glEnableVertexAttribArray(tex_attrib);
	}

	GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_shader, 1, &vertsource, NULL);
	glCompileShader(vert_shader);

	GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &fragsource, NULL);
	glCompileShader(frag_shader);

	debug_shader("2D model: ", vert_shader, frag_shader);

	renderdata->shader_program = glCreateProgram();
	glAttachShader(renderdata->shader_program, vert_shader);
	glAttachShader(renderdata->shader_program, frag_shader);
	glBindFragDataLocation(renderdata->shader_program, 0, "FragColor");
	glLinkProgram(renderdata->shader_program);
	glUseProgram(renderdata->shader_program);

	renderdata->texture_sampler = glGetUniformLocation(renderdata->shader_program, "Texture");

	int width, height, num_channels;
	byte* image;

	stbi_set_flip_vertically_on_load(true);

	image = stbi_load(texture_path, &width, &height, &num_channels, 0);

	glGenTextures(1, &renderdata->texture_id);
	glActiveTexture(GL_TEXTURE0 + tex_unit);
	glBindTexture(GL_TEXTURE_2D, renderdata->texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(image);

	glUniform1i(renderdata->texture_sampler, tex_unit);
}

void render_data_draw(Render_Data_2D data, uint vb_size = NULL, byte* vb_data = NULL, uint num_instances = 1)
{
	glBindVertexArray(data.VAO);

	glUseProgram(data.shader_program);

	if (vb_data)
	{
		glBindBuffer(GL_ARRAY_BUFFER, data.VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vb_size, vb_data);
	}

	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, num_instances);
}
void render_data_draw(Render_Data_2D_UV data, uint vb_size = NULL, byte* vb_data = NULL, uint num_instances = 1)
{
	glBindVertexArray(data.VAO);
	glBindTexture(GL_TEXTURE_2D, data.texture_id);

	glUseProgram(data.shader_program);

	if (vb_data)
	{
		glBindBuffer(GL_ARRAY_BUFFER, data.VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vb_size, vb_data);
	}

	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, num_instances);
}

// utilities

void gl_add_attrib_float(uint id, uint stride, uint offset)
{
	glVertexAttribPointer(id, 1, GL_FLOAT, GL_FALSE, stride, (void*)offset);
	glVertexAttribDivisor(id, 1);
	glEnableVertexAttribArray(id);
}
void gl_add_attrib_vec2(uint id, uint stride, uint offset)
{
	glVertexAttribPointer(id, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset);
	glVertexAttribDivisor(id, 1);
	glEnableVertexAttribArray(id);
}
void gl_add_attrib_vec3(uint id, uint stride, uint offset)
{
	glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
	glVertexAttribDivisor(id, 1);
	glEnableVertexAttribArray(id);
}
void gl_add_attrib_mat3(uint id, uint stride, uint offset)
{
	glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
	glVertexAttribDivisor(id, 1);
	glEnableVertexAttribArray(id);

	++id;
	offset += sizeof(vec3);
	glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
	glVertexAttribDivisor(id, 1);
	glEnableVertexAttribArray(id);

	++id;
	offset += sizeof(vec3);
	glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
	glVertexAttribDivisor(id, 1);
	glEnableVertexAttribArray(id);
}