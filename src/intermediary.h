// ----------------------------------------------
// Copyright Mohamed Hamed. All rights reserved.
//
// name        : City-Building Game
// started     : 31 May 2020
// updated     : 18 Aug 2020
// ----------------------------------------------
#pragma once

#pragma comment(lib, "winmm")
#pragma comment(lib, "opengl32")
#pragma comment(lib, "external/GLEW/glew32s")
#pragma comment(lib, "external/GLFW/glfw3")
#pragma comment(lib, "external/OpenAL/OpenAL32.lib")

#define _CRT_SECURE_NO_WARNINGS

#define STB_IMAGE_IMPLEMENTATION
#include <external/stb_image.h> // image loading

#define GLEW_STATIC
#include <external/GLEW\glew.h> // OpenGL functions
#include <external/GLFW\glfw3.h>// window & input

#include <external/OpenAL/al.h> // audio
#include <external/OpenAL/alc.h>

#include "proprietary/types.h"
#include "proprietary/timer.h"
#include "proprietary/memory.h"
#include "proprietary/debug.h"

#define rand_int __rdtsc
#define NULL 0
#define PI	  3.14159265359f
#define TWOPI 6.28318530718f
#define ToRadians(value) ( ((value) * PI) / 180.0f )
#define ToDegrees(value) ( ((value) * 180.0f) / PI )

#define GLM_FORCE_RADIAN
#define GLM_ENABLE_EXPERIMENTAL
#include <external/GLM/glm.hpp> //for math
#include <external/GLM/gtc/matrix_transform.hpp>
#include <external/GLM/gtc/type_ptr.hpp>
#include <external/GLM/gtc/quaternion.hpp> //for quaternions
#include <external/GLM/gtx/quaternion.hpp>
#include <external/GLM/gtc/noise.hpp> // for terrain generation

using glm::vec2;  using glm::vec3; using glm::vec4;
using glm::mat3;  using glm::mat4;
using glm::quat;
using glm::ivec2; using glm::ivec3;
using glm::uvec2; using glm::uvec3;

void print_matrix(mat3 mat)
{
	for (uint i = 0; i < 12; ++i)
	{
		print("%09f ", *((float*)(&mat) + i));
	} print("\n");
}
mat3 point_at(vec3 dir, vec3 up)
{
	//Assumed to be normalized
	vec3 f = dir * -1.f; //front
	vec3 r = cross(up, f); //right
	vec3 u = up;

	glm::mat3 result(1);
	result[0][0] = r.x;
	result[1][0] = r.y;
	result[2][0] = r.z;
	result[0][1] = u.x;
	result[1][1] = u.y;
	result[2][1] = u.z;
	result[0][2] = f.x;
	result[1][2] = f.y;
	result[2][2] = f.z;

	return glm::inverse(result);
}
#define vecmagnitude(vec) sqrt((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z))

// ----------- shaders ----------- //

/*------------------------------- Default shaders -------------------------------*/
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
	FragPos = Vertex + vec3(16,48,16);

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

	vec3 LightPos = vec3(2, 3, 2);

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

const char* PosVertSourceUV = R"glsl(
#version 330 core
layout (location = 0) in vec3 Vertex;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 Texture;
layout (location = 3) in vec3 Position;

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
	FragPos = Vertex + Position;
	TexCoord = Texture;
	gl_Position = ProjView * vec4(FragPos, 1.0);
}
	)glsl";

const char* PosFragSourceUV = R"glsl(
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

	if(ObjectColor.y < .01 || ObjectColor.x < .01)
	{
		discard;
	}


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

/*------------------------------- Default Shaders + Position -------------------------------*/
const char* PosVertSource = R"glsl(
#version 330 core

layout (location = 0) in vec3 Vertex;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 Position;

uniform mat4 ProjView;
uniform vec3 ViewPos;

out vec3 normal;
out vec3 FragPos;
out vec3 viewpos;

void main()
{
	normal  = Normal;
	viewpos = ViewPos;
	FragPos = Vertex + Position;

	gl_Position = ProjView * vec4(FragPos, 1.0);
}
	)glsl";

const char* PosFragSource = R"glsl(
#version 330 core

in vec3 normal;
in vec3 FragPos;
in vec3 viewpos;

out vec4 FragColor;

void main()
{
	vec3 LightColor  = vec3(.4,0,.4);
	vec3 ObjectColor = vec3(1,1,1);

	vec3 LightPos = vec3(2, 3, 2);

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

/*------------------------------- Solid Shaders -------------------------------*/
const char* SolidVertSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 Vertex;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 Texture;
layout (location = 3) in vec3 Position;
layout (location = 4) in vec2 TexOffset;

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
	FragPos = Vertex + Position;
	gl_Position = ProjView * vec4(FragPos, 1.0);
	TexCoord.x = (TexOffset.x + Texture.x);
	TexCoord.y = (TexOffset.y + Texture.y);
}
	)glsl";

const char* SolidFragSource = R"glsl(
#version 330 core
in vec2 TexCoord;
in vec3 normal;
in vec3 FragPos;
in vec3 viewpos;
uniform sampler2D Texture;
out vec4 FragColor;
void main()
{
	vec3 LightColor  = vec3(.97,.84,.11);
	vec3 ObjectColor = texture(Texture, TexCoord).xyz;
	if(ObjectColor == vec3(0,0,0)) { FragColor = vec4(0,1,1, 1.0); return; }
	vec3 LightPos   = vec3(0,1.5,0);
	vec3 Ambient    = 0.3 * ObjectColor;
	vec3 LightDir   = normalize(LightPos - FragPos);
	vec3 Diffuse    = max(dot(normal, LightDir), 0.0) * ObjectColor;
	vec3 ViewDir    = normalize(viewpos - FragPos);
	vec3 ReflectDir = reflect(-LightDir, normal);
	float Spec      = pow(max(dot(ViewDir, ReflectDir), 0.0), 8);
	vec3 Specular   = .5 * Spec * LightColor;
	vec3 Result     = (Ambient + Diffuse + Specular) * LightColor;
	FragColor = vec4(Result, 1.0);
}
	)glsl";

/*------------------------------- Fluid Shaders -------------------------------*/
const char* FluidVertSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 Vertex;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 Position;

uniform mat4 ProjView;
uniform vec3 ViewPos;
uniform float Timer;

out vec3 normal;
out vec3 FragPos;
out vec3 viewpos;
out float Height;

void main()
{
	normal  = Normal;
	viewpos = ViewPos;
	FragPos = Vertex + Position;
	float x1 = (Vertex.x + 1) + Position.x;
	float x2 = (Vertex.z + 1) + Position.z;
	float factor  = sqrt( (x1 * x1) + (x2 * x2) );
	vec3 FinalPos = Vertex + Position;
	FinalPos.y += .05 * sin( (-Timer *  3.14159) + (3.1415 * factor) );
	FinalPos.y -= .1;
	Height = sin( (-Timer * 3.14159) + (3.1415 * factor) );
	gl_Position = ProjView * vec4( FinalPos, 1);
}
	)glsl";

const char* FluidFragSource = R"glsl(
#version 330 core

out vec4 FragColor;

in vec3 normal;
in vec3 FragPos;
in vec3 viewpos;
in float Height;

void main()
{
	vec3 white = vec3(1,1,1) * (Height * .05);
	vec3 LightColor  = vec3(.97,.84,.11);
	vec3 ObjectColor = vec3(0,0,1);
	
	vec3 LightPos = vec3(32, 16, 32);
	vec3 Ambient  = 0.6 * ObjectColor;
	vec3 LightDir = normalize(LightPos - FragPos);
	vec3 Diffuse  = max(dot(normal, LightDir), 0.0) * ObjectColor;
	vec3 ViewDir  = normalize(viewpos - FragPos);
	vec3 ReflectDir = reflect(-LightDir, normal);
	float Spec = pow(max(dot(ViewDir, ReflectDir), 0.0), 32);
	vec3 Specular = .5 * Spec * LightColor;
	vec3 Result   = (Ambient + Diffuse + Specular + white) * LightColor;
	FragColor = vec4(Result, 1.0);
}
	)glsl";

/*------------------------------- Default Shaders + Position -------------------------------*/
const char* PosRotVertSource = R"glsl(
#version 330 core

layout (location = 0) in vec3 Vertex;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 Position;
layout (location = 3) in mat3 Rotation;

uniform mat4 ProjView;
uniform vec3 ViewPos;

out vec3 normal;
out vec3 FragPos;
out vec3 viewpos;

void main()
{
	normal  = Rotation * Normal;
	viewpos = ViewPos;
	FragPos = Rotation * Vertex;
	FragPos += Position;

	gl_Position = ProjView * vec4(FragPos, 1.0);
}
	)glsl";

const char* PosRotFragSource = R"glsl(
#version 330 core

in vec3 normal;
in vec3 FragPos;
in vec3 viewpos;

out vec4 FragColor;

void main()
{
	vec3 LightColor  = vec3(.4,0,.4);
	vec3 ObjectColor = vec3(1,1,1);

	vec3 LightPos = vec3(2, 3, 2);

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

/*------------------------------- Dropped Item Shaders -------------------------------*/
const char* DroppedItemVertSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 Vertex;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 Texture;
layout (location = 3) in vec3 Position;
layout (location = 4) in vec2 TexOffset;

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
	FragPos = (Vertex * vec3(.3,.3,.3)) + Position;
	gl_Position = ProjView * vec4(FragPos , 1.0);
	TexCoord.x = (TexOffset.x + Texture.x);
	TexCoord.y = (TexOffset.y + Texture.y);
}
	)glsl";

const char* DroppedItemFragSource = R"glsl(
#version 330 core

in vec2 TexCoord;
in vec3 normal;
in vec3 FragPos;
in vec3 viewpos;

uniform sampler2D Texture;

out vec4 FragColor;

void main()
{
	vec3 LightColor  = vec3(.97,.84,.11);
	vec3 ObjectColor = texture(Texture, TexCoord).xyz;
	if(ObjectColor == vec3(0,0,0)) { FragColor = vec4(0,1,1, 1.0); return; }

	vec3 LightPos   = vec3(0,1.5,0);
	vec3 Ambient    = 0.3 * ObjectColor;

	vec3 LightDir   = normalize(LightPos - FragPos);
	vec3 Diffuse    = max(dot(normal, LightDir), 0.0) * ObjectColor;

	vec3 ViewDir    = normalize(viewpos - FragPos);
	vec3 ReflectDir = reflect(-LightDir, normal);
	float Spec      = pow(max(dot(ViewDir, ReflectDir), 0.0), 8);
	vec3 Specular   = .5 * Spec * LightColor;

	vec3 Result     = (Ambient + Diffuse + Specular) * LightColor;

	FragColor = vec4(Result, 1.0);
}
	)glsl";

/*------------------------------- 2D Shaders -------------------------------*/

const char* VertSource2D = R"glsl(
	#version 330 core

	layout (location = 0) in vec2 Vertex;

	void main()
	{
		gl_Position = vec4( Vertex , 0.0 , 1.0 );
	}
		)glsl";

const char* FragSource2D = R"glsl(
	#version 330 core

	out vec4 FragColor;

	void main()
	{
		FragColor = vec4(1, 0, 1, 1);
	}
		)glsl";

const char* PosVertSource2D = R"glsl(
	#version 330 core

	layout (location = 0) in vec2 Vertex;
	layout (location = 1) in vec2 Position;

	void main()
	{
		gl_Position = vec4( Vertex + Position , 0.0 , 1.0 );
	}
		)glsl";

const char* PosFragSource2D = R"glsl(
	#version 330 core

	in  vec2 tex_coord;
	out vec4 FragColor;

	void main()
	{
		FragColor = vec4(1, 0, 1, 1);
	}
		)glsl";

const char* VertSource2DUV = R"glsl(
	#version 330 core

	layout (location = 0) in vec2 Vertex;
	layout (location = 1) in vec2 TexCoord;

	out vec2 tex_coord;

	void main()
	{
		tex_coord = TexCoord;
		gl_Position = vec4( Vertex , 0.0 , 1.0 );
	}
		)glsl";

const char* FragSource2DUV = R"glsl(
	#version 330 core

	in  vec2 tex_coord;
	out vec4 FragColor;

	uniform sampler2D Texture;

	void main()
	{
		FragColor = texture(Texture, tex_coord);
	}
		)glsl";

const char* PosVertSource2DUV = R"glsl(
	#version 330 core

	layout (location = 0) in vec2 Vertex;
	layout (location = 1) in vec2 TexCoord;
	layout (location = 2) in vec2 Position;

	out vec2 tex_coord;

	void main()
	{
		tex_coord = TexCoord;
		gl_Position = vec4( Vertex + Position , 0.0 , 1.0 );
	}
		)glsl";

const char* PosFragSource2DUV = R"glsl(
	#version 330 core

	in  vec2 tex_coord;
	out vec4 FragColor;

	uniform sampler2D Texture;

	void main()
	{
		FragColor = texture(Texture, tex_coord);
	}
		)glsl";

/*------------------------------- Seed Shaders -------------------------------*/

const char* SeedVertSource = R"glsl(
#version 330 core

layout (location = 0) in vec3 Vertex;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 Texture;
layout (location = 3) in vec3 Position;
layout (location = 4) in float Growth; // 0 = just planted, 1 = fully grown

uniform mat4 ProjView;
uniform vec3 ViewPos;

out vec3 normal;
out vec3 FragPos;
out vec3 viewpos;
out float growth;
out float height;
out vec2 TexCoord;

void main()
{
	growth   = Growth;
	height   = Vertex.y;
	normal   = Normal;
	viewpos  = ViewPos;
	TexCoord = Texture;
	FragPos  = Vertex + Position;

	gl_Position = ProjView * vec4(FragPos, 1.0);
}
	)glsl";

const char* SeedFragSource = R"glsl(
#version 330 core

in vec3 normal;
in vec3 FragPos;
in vec3 viewpos;
in float growth;
in float height;
in vec2 TexCoord;

uniform sampler2D Texture;

out vec4 FragColor;

void main()
{
	vec3 LightColor  = vec3(1,1,1);
	vec3 ObjectColor = texture(Texture, TexCoord).xyz;

	if( height > growth || (ObjectColor.y < .01 || ObjectColor.x < .01))
	{
		discard;
	}

	vec3 LightPos = vec3(2, 3, 2);

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