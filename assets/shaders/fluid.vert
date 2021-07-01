#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex_coord;
layout (location = 3) in vec3 world_position;

struct VS_OUT
{
	vec3 normal;   // normal vector
	vec3 frag_pos; // position of this pixel in world space
	vec2 tex_coord;
};

uniform mat4 proj_view;
uniform float timer;

out VS_OUT vs_out;

void main()
{
	vs_out.normal = normal;
	vs_out.frag_pos = (position * 1.5) + world_position;
	vs_out.tex_coord = tex_coord;

	float x1 = (position.x + 1) + world_position.x;
	float x2 = (position.z + 1) + world_position.z;
	float factor  = sqrt( (x1 * x1) + (x2 * x2) );

	vs_out.frag_pos.y += .02 * sin( (-timer *  3.1415) + (3.1415 * factor) );

	gl_Position = proj_view * vec4(vs_out.frag_pos, 1.0);
}
