#pragma once
#include "intermediary.h"

/*
 'model' binary file format
  - (uint) num_verts
  - (uint) num_indicies
  ~ (uint) num_joints
  - (vec3) positions
  - (vec3) normals
  ~ (vec2) textures
  ~ (vec3) weights
  ~ (ivec3) joint_ids
  - (uint) indicies
  ~ (int)  joint_parent_indicies
  ~ (mat4) inverse-bind-matrices
*/

struct Model_Data
{
	uint num_verts, num_indicies;

	vec3* positions;
	vec3* normals;
	uint* indicies;
};

struct Model_Data_UV
{
	uint num_verts, num_indicies;

	vec3* positions;
	vec3* normals;
	vec2* textures;
	uint* indicies;
};

struct Model_Data_Anim
{
	uint num_verts, num_indicies, num_joints;

	vec3* positions;
	vec3* normals;
	vec2* textures;
	uint* indicies;
	vec3* weights;
	ivec3* influencing_joints;

	// skeleton information
	int* parents; // index of the parent joint
	mat4* ibm; // inverse-bind matrix

	// animation data
	uint fps, num_frames;
	mat4** local_transforms;
};

void load_model(Model_Data* model_data, const char* path)
{
	FILE* model_file = fopen(path, "rb");
	if (!model_file) { print("could not open model file: %s\n", path); stop; return; }

	fread(&model_data->num_verts, sizeof(uint), 1, model_file);
	fread(&model_data->num_indicies, sizeof(uint), 1, model_file);

	model_data->positions = Alloc(vec3, model_data->num_verts);
	model_data->normals = Alloc(vec3, model_data->num_verts);
	model_data->indicies = Alloc(uint, model_data->num_indicies);

	fread(model_data->positions, sizeof(vec3), model_data->num_verts, model_file);
	fread(model_data->normals, sizeof(vec3), model_data->num_verts, model_file);
	fread(model_data->indicies, sizeof(uint), model_data->num_indicies, model_file);

	fclose(model_file);
}
void load_model(Model_Data_UV* model_data, const char* path)
{
	FILE* model_file = fopen(path, "rb");
	if (!model_file) { print("could not open model file: %s\n", path); stop; return; }

	fread(&model_data->num_verts, sizeof(uint), 1, model_file);
	fread(&model_data->num_indicies, sizeof(uint), 1, model_file);

	model_data->positions = Alloc(vec3, model_data->num_verts);
	model_data->normals = Alloc(vec3, model_data->num_verts);
	model_data->textures = Alloc(vec2, model_data->num_verts);
	model_data->indicies = Alloc(uint, model_data->num_indicies);

	fread(model_data->positions, sizeof(vec3), model_data->num_verts, model_file);
	fread(model_data->normals, sizeof(vec3), model_data->num_verts, model_file);
	fread(model_data->textures, sizeof(vec2), model_data->num_verts, model_file);
	fread(model_data->indicies, sizeof(uint), model_data->num_indicies, model_file);

	fclose(model_file);
}
void load_model(Model_Data_Anim* model_data, const char* path)
{
	FILE* model_file = fopen(path, "rb");
	if (!model_file) { print("could not open model file: %s\n", path); stop; return; }

	fread(&model_data->num_verts, sizeof(uint), 1, model_file);
	fread(&model_data->num_indicies, sizeof(uint), 1, model_file);
	fread(&model_data->num_joints, sizeof(uint), 1, model_file);

	model_data->positions = Alloc(vec3, model_data->num_verts);
	model_data->normals = Alloc(vec3, model_data->num_verts);
	model_data->weights = Alloc(vec3, model_data->num_verts);
	model_data->influencing_joints = Alloc(ivec3, model_data->num_verts);
	model_data->indicies = Alloc(uint, model_data->num_indicies);
	model_data->parents = Alloc(int, model_data->num_joints);
	model_data->ibm = Alloc(mat4, model_data->num_joints);

	fread(model_data->positions, sizeof(vec3), model_data->num_verts, model_file);
	fread(model_data->normals, sizeof(vec3), model_data->num_verts, model_file);
	fread(model_data->weights, sizeof(vec3), model_data->num_verts, model_file);
	fread(model_data->influencing_joints, sizeof(ivec3), model_data->num_verts, model_file);
	fread(model_data->indicies, sizeof(uint), model_data->num_indicies, model_file);
	fread(model_data->parents, sizeof(int), model_data->num_joints, model_file);
	fread(model_data->ibm, sizeof(mat4), model_data->num_joints, model_file);

	fclose(model_file);

	//animation data
	FILE* anim_file = fopen("assets/models/animation_data.anim", "rb");
	if (!anim_file) { print("could not open anim file!\n"); stop; return; }

	fread(&model_data->fps, sizeof(uint), 1, anim_file);
	fread(&model_data->num_frames, sizeof(uint), 1, anim_file);

	model_data->local_transforms = Alloc(mat4*, model_data->num_frames);

	for (uint i = 0; i < model_data->num_frames; ++i)
		model_data->local_transforms[i] = Alloc(mat4, model_data->num_joints);

	for (uint i = 0; i < model_data->num_frames; ++i)
		fread(model_data->local_transforms[i], sizeof(mat4), model_data->num_joints, anim_file);

	fclose(anim_file);
}

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
	GLuint texture_sampler;
	GLuint proj_view, view_pos;
	uint num_indicies;
};

struct Animation
{
	float time_per_frame;
	float time_since_last_keyframe;
	uint num_frames, current_frame;

	mat4* ibm;
	int* parents;

	mat4** local_transforms;
	mat4* current_transform;
};

struct Render_Data_Anim
{
	GLuint VAO, VBO, EBO, UBO;
	GLuint shader_program;
	GLuint proj_view, view_pos;
	uint num_indicies, num_joints;

	Animation animation;
};

mat4 interpolate_matrices(mat4 m1, mat4 m2, float progression)
{
	quat rot_1 = glm::quat_cast(m1);
	quat rot_2 = glm::quat_cast(m2);

	vec3 pos_1 = vec3(m1[0][3], m1[1][3], m1[2][3]);
	vec3 pos_2 = vec3(m2[0][3], m2[1][3], m2[2][3]);

	quat rot = glm::mix(rot_1, rot_2, 0.f);
	vec3 pos = glm::mix(pos_1, pos_2, 0.f);

	mat4 ret = glm::toMat4(rot);
	ret[0][3] = pos.x;
	ret[1][3] = pos.y;
	ret[2][3] = pos.z;

	return ret;
}
void update_animation(Animation* anim, float DeltaTime)
{
	anim->time_since_last_keyframe += DeltaTime;

	if (anim->time_since_last_keyframe > anim->time_per_frame)
	{
		anim->current_frame++;
		anim->current_frame = anim->current_frame % anim->num_frames;
		anim->time_since_last_keyframe = 0;
	}

	if (anim->current_transform)
		Free(anim->current_transform);

	anim->current_transform = Alloc(mat4, 16);
	memcpy(anim->current_transform, anim->local_transforms[anim->current_frame], sizeof(mat4) * 16);

	for (uint i = 1; i < 16; ++i)
	{
		//interpolate_matrices(anim->local_transforms[anim->current_frame][i], anim->local_transforms[anim->current_frame][i + 1 % anim->num_frames], anim->time_since_last_keyframe / anim->time_per_frame)
		anim->current_transform[i] = anim->local_transforms[anim->current_frame][i] * anim->current_transform[anim->parents[i]];
	}

	for (uint i = 0; i < 16; ++i)
	{
		anim->current_transform[i] = anim->ibm[i] * anim->current_transform[i];
	}
}

void init(Render_Data* renderdata, uint reserved_mem_size, const char* path, const char* vertsource, const char* fragsource)
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
void init(Render_Data_UV* renderdata, uint reserved_mem_size, const char* path, const char* vertsource, const char* fragsource)
{
	Model_Data_UV model_data = {};
	load_model(&model_data, path);
	renderdata->num_indicies = model_data.num_indicies;

	glGenVertexArrays(1, &renderdata->VAO);
	glBindVertexArray(renderdata->VAO);

	uint vertmemsize = model_data.num_verts * sizeof(vec3);
	uint texmemsize = model_data.num_verts * sizeof(vec2);
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
	renderdata->proj_view = glGetUniformLocation(renderdata->shader_program, "ProjView");
	renderdata->view_pos = glGetUniformLocation(renderdata->shader_program, "ViewPos");
}
void init(Render_Data_Anim* renderdata, uint reserved_mem_size, const char* path, const char* vertsource, const char* fragsource)
{
	Model_Data_Anim model_data;
	load_model(&model_data, path);

	renderdata->num_indicies = model_data.num_indicies;
	renderdata->num_joints = model_data.num_joints;

	renderdata->animation = {};
	renderdata->animation.ibm = model_data.ibm;
	renderdata->animation.local_transforms = model_data.local_transforms;
	renderdata->animation.parents = model_data.parents;
	renderdata->animation.time_per_frame = 1.f / 24.f;
	renderdata->animation.time_since_last_keyframe = 0;
	renderdata->animation.num_frames = model_data.num_frames;

	glGenVertexArrays(1, &renderdata->VAO);
	glBindVertexArray(renderdata->VAO);

	uint vertmemsize = model_data.num_verts * sizeof(vec3);
	uint offset = reserved_mem_size;

#define RENDER_MEM_SIZE (reserved_mem_size + (model_data.num_verts * sizeof(vec3) * 4))
	glGenBuffers(1, &renderdata->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, renderdata->VBO);
	glBufferData(GL_ARRAY_BUFFER, RENDER_MEM_SIZE, NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, offset, vertmemsize, model_data.positions); offset += vertmemsize;
	glBufferSubData(GL_ARRAY_BUFFER, offset, vertmemsize, model_data.normals);   offset += vertmemsize;
	glBufferSubData(GL_ARRAY_BUFFER, offset, vertmemsize, model_data.weights);   offset += vertmemsize;
	glBufferSubData(GL_ARRAY_BUFFER, offset, vertmemsize, model_data.influencing_joints);
#undef RENDER_MEM_SIZE

	glGenBuffers(1, &renderdata->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderdata->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model_data.num_indicies * sizeof(uint), model_data.indicies, GL_STATIC_DRAW);

	Free(model_data.positions);
	Free(model_data.normals);
	Free(model_data.weights);
	Free(model_data.influencing_joints);
	Free(model_data.indicies);

	{
		GLint vert_pos = 0, vert_norm = 1, vert_weight = 2, vert_joint_id = 3;

		offset = reserved_mem_size;
		glVertexAttribPointer(vert_pos, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)offset); offset += vertmemsize;
		glVertexAttribPointer(vert_norm, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)offset); offset += vertmemsize;
		glVertexAttribPointer(vert_weight, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)offset); offset += vertmemsize;
		glVertexAttribIPointer(vert_joint_id, 3, GL_INT, sizeof(ivec3), (void*)offset);

		glEnableVertexAttribArray(vert_pos);
		glEnableVertexAttribArray(vert_norm);
		glEnableVertexAttribArray(vert_weight);
		glEnableVertexAttribArray(vert_joint_id);
	}

	// create final transforms
	mat4* final_transforms = Alloc(mat4, model_data.num_joints);

	final_transforms[0] = model_data.local_transforms[0][0];
	for (uint i = 1; i < model_data.num_joints; ++i)
	{
		final_transforms[i] = model_data.local_transforms[0][i] * final_transforms[model_data.parents[i]];
	}

	for (uint i = 0; i < model_data.num_joints; ++i)
	{
		final_transforms[i] = model_data.ibm[i] * final_transforms[i];
	}

	glGenBuffers(1, &renderdata->UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, renderdata->UBO);
	glBufferData(GL_UNIFORM_BUFFER, model_data.num_joints * sizeof(mat4), final_transforms, GL_DYNAMIC_DRAW);

	//glBindBufferRange(GL_UNIFORM_BUFFER, 0, renderdata->UBO, 0, model_data.num_joints * sizeof(glm::mat4));
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, renderdata->UBO);

	GLuint VertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertShader, 1, &vertsource, NULL);
	glCompileShader(VertShader);

	GLuint FragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragShader, 1, &fragsource, NULL);
	glCompileShader(FragShader);

	debug_shader(path, VertShader, FragShader);

	renderdata->shader_program = glCreateProgram();
	glAttachShader(renderdata->shader_program, VertShader);
	glAttachShader(renderdata->shader_program, FragShader);
	glBindFragDataLocation(renderdata->shader_program, 0, "FragColor");
	glLinkProgram(renderdata->shader_program);
	glUseProgram(renderdata->shader_program);

	GLint skeleton_id = glGetUniformBlockIndex(renderdata->shader_program, "Skeleton");
	glUniformBlockBinding(renderdata->shader_program, skeleton_id, 0);

	renderdata->proj_view = glGetUniformLocation(renderdata->shader_program, "ProjView");
	renderdata->view_pos = glGetUniformLocation(renderdata->shader_program, "ViewPos");
}

void draw(Render_Data data, mat4* projview, vec3 viewpos, uint vb_size = NULL, byte* vb_data = NULL, uint num_instances = 1)
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
void draw(Render_Data_UV data, mat4* projview, vec3 viewpos, uint vb_size = NULL, byte* vb_data = NULL, uint num_instances = 1)
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
void draw(Render_Data_Anim data, mat4* projview, vec3 viewpos, uint vb_size = NULL, byte* vb_data = NULL, uint num_instances = 1)
{
	glBindVertexArray(data.VAO);

	glUseProgram(data.shader_program);
	glUniformMatrix4fv(data.proj_view, 1, GL_FALSE, (float*)projview);
	glUniform3f(data.view_pos, viewpos.x, viewpos.y, viewpos.z);

	glBindBuffer(GL_UNIFORM_BUFFER, data.UBO);
	glBufferData(GL_UNIFORM_BUFFER, data.num_joints * sizeof(mat4), data.animation.current_transform, GL_DYNAMIC_DRAW);

	if (vb_data)
	{
		glBindBuffer(GL_ARRAY_BUFFER, data.VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vb_size, vb_data);
	}

	glDrawElementsInstanced(GL_TRIANGLES, data.num_indicies, GL_UNSIGNED_INT, 0, num_instances);
}

void gl_add_attrib_vec3(uint id, uint stride, uint offset)
{
	glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
	glVertexAttribDivisor(id, 1);
	glEnableVertexAttribArray(id);
}

/*------------------------------- Default haders -------------------------------*/
const char* VertSource = R"glsl(
#version 330 core

layout (location = 0) in vec3 Vertex;
layout (location = 1) in vec3 Normal;

uniform mat4 ProjView;
uniform vec3 ViewPos;

out vec3 normal;
out vec3 FragPos;
out vec3 viewpos;

void main()
{
	normal  = Normal;
	viewpos = ViewPos;
	FragPos = Vertex;

	gl_Position = ProjView * vec4(FragPos, 1.0);
}
	)glsl";

const char* FragSource = R"glsl(
#version 330 core

in vec3 normal;
in vec3 FragPos;
in vec3 viewpos;

out vec4 FragColor;

void main()
{
	vec3 LightColor  = vec3(.4,.4,.4);
	vec3 ObjectColor = vec3(1,1,1);

	vec3 LightPos = vec3(0, 50, 0);

	vec3 Ambient = 0.3 * ObjectColor;

	vec3 LightDir = normalize(LightPos - FragPos);

	vec3 Diffuse = max(dot(normal, LightDir), 0.0) * ObjectColor;

	vec3 ViewDir = normalize(viewpos - FragPos);
	vec3 ReflectDir = reflect(-LightDir, normal);
	float Spec = pow(max(dot(ViewDir, ReflectDir), 0.0), 8);
	vec3 Specular = .5 * Spec * LightColor;

	vec3 Result = (Ambient + Diffuse + Specular) * LightColor;
	FragColor = vec4(Result, 1.0);
}
	)glsl";

const char* VertSourceUV = R"glsl(
#version 330 core
layout (location = 0) in vec3 Vertex;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 Texture;

uniform mat4 ProjView;
uniform vec3 ViewPos;

out vec2 TexCoord;
out vec3 normal;
out vec3 FragPos;
out vec3 viewpos;

void main()
{
	normal  = Normal;
	viewpos = ViewPos;
	FragPos = Vertex + vec3(18,20,18);
	TexCoord = Texture;
	gl_Position = ProjView * vec4(FragPos, 1.0);
}
	)glsl";

const char* FragSourceUV = R"glsl(
#version 330 core
in vec2 TexCoord;
in vec3 normal;
in vec3 FragPos;
in vec3 viewpos;

uniform sampler2D Texture;

out vec4 FragColor;

void main()
{
	vec3 LightColor  = vec3(1,1,1);
	vec3 ObjectColor = texture(Texture, TexCoord).xyz;

	vec3 LightPos = vec3(5,5,5) + vec3(18,20,18);

	vec3 Ambient = 0.3 * ObjectColor;

	vec3 LightDir = normalize(LightPos - FragPos);

	vec3 Diffuse = max(dot(normal, LightDir), 0.0) * ObjectColor;

	vec3 ViewDir = normalize(viewpos - FragPos);
	vec3 ReflectDir = reflect(-LightDir, normal);
	float Spec = pow(max(dot(ViewDir, ReflectDir), 0.0), 8);
	vec3 Specular = .5 * Spec * LightColor;

	vec3 Result = (Ambient + Diffuse + Specular) * LightColor;
	FragColor = vec4(Result, 1.0);
}
	)glsl";

const char* VertSourceAnim = R"glsl(
#version 420 core

const int MAX_JOINTS = 50;

layout (location = 0) in vec3 Vertex;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 Weight;
layout (location = 3) in ivec3 Joint_ID;

layout (std140) uniform Skeleton
{
	uniform mat4 joint_transforms[MAX_JOINTS];
};

uniform mat4 ProjView;
uniform vec3 ViewPos;

out vec3 normal;
out vec3 FragPos;
out vec3 viewpos;

void main()
{
	vec4 final_pos    = vec4(0,0,0,0);
	vec4 final_normal = vec4(0,0,0,0);

	// 3 weights per vertex
	for(int i = 0; i < 3; i++)
	{
		if(Joint_ID[i] != -1)
		{
			mat4 joint_transform = joint_transforms[Joint_ID[i]];
			vec4 pose_position = vec4(Vertex, 1.0) * joint_transform;
			final_pos += pose_position * Weight[i];

			vec4 world_normal = vec4(Normal, 0.0) * joint_transform;
			final_normal += world_normal * Weight[i];
		}
	}

	FragPos = final_pos.xyz;
	normal  = final_normal.xyz;
	viewpos = ViewPos;

	gl_Position = ProjView * vec4(FragPos, 1.0);
}
	)glsl";

const char* FragSourceAnim = R"glsl(
#version 420 core

in vec3 normal;
in vec3 FragPos;
in vec3 viewpos;

out vec4 FragColor;

void main()
{
	vec3 LightColor  = vec3(.6,.6,.6);
	vec3 ObjectColor = vec3(.8,.2,.1);

	vec3 LightPos = vec3(-10, 10, -10);

	vec3 Ambient = 0.3 * ObjectColor;

	vec3 LightDir = normalize(LightPos - FragPos);

	vec3 Diffuse = max(dot(normal, LightDir), 0.0) * ObjectColor;

	vec3 ViewDir = normalize(viewpos - FragPos);
	vec3 ReflectDir = reflect(-LightDir, normal);
	float Spec = pow(max(dot(ViewDir, ReflectDir), 0.0), 8);
	vec3 Specular = .5 * Spec * LightColor;

	vec3 Result = (Ambient + Diffuse + Specular) * LightColor;
	FragColor = vec4(Result, 1.0);
}
	)glsl";