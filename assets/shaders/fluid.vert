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

vec3 gerstner_wave(vec4 wave, vec2 origin, vec3 pos, inout vec3 tangent, inout vec3 binormal);
vec3 sin_wave(vec2 origin, vec3 pos, inout vec3 tangent, inout vec3 binormal);

void main()
{
	vec3 pos_ = position * 1.033333; // this is hacked garbage plz ignore
	pos_.y = position.y; // ignore this 2

	vec3 pos = vec3(pos_.x + world_position.x, pos_.y, pos_.z + world_position.z);
	vec3 tangent  = vec3(1, 0, 0);
	vec3 binormal = vec3(0, 0, 1);

	pos += gerstner_wave(vec4(1, 1.0, .2, 1.0), vec2(1, 0), pos, tangent, binormal);
	pos += gerstner_wave(vec4(1, 0.6, .2, .50), vec2(1, 1), pos, tangent, binormal);
	pos += gerstner_wave(vec4(1, 1.3, .2, .25), vec2(1, 1), pos, tangent, binormal);
	//pos += sin_wave(vec2(8,14), pos, tangent, binormal);

	vec3 adjusted_normal = normalize(cross(binormal, tangent));

	vs_out.frag_pos  = pos; vs_out.frag_pos.y +=.15;
	vs_out.normal    = adjusted_normal;
	vs_out.tex_coord = tex_coord;

	gl_Position = proj_view * vec4(vs_out.frag_pos, 1.0);
}

vec3 gerstner_wave(vec4 wave, vec2 origin, vec3 pos, inout vec3 tangent, inout vec3 binormal)
{
	float steepness  = wave.z;
	float wavelength = wave.w;
	float k = 6.283185 / wavelength;
	float c = sqrt(9.81 / k); // speed
	float a = steepness / k;  // amplitude
	vec2  d = normalize(wave.xy); // direction
	float f = k * (dot(d, pos.xz) - (c * timer));

	tangent += vec3(
		-d.x * (steepness * sin(f)) * d.x,
		 d.x * (steepness * cos(f)),
		-d.x * (steepness * sin(f)) * d.y);

	binormal += vec3(
		-d.x * (steepness * sin(f)) * d.y,
		 d.y * (steepness * cos(f)),
		-d.y * (steepness * sin(f)) * d.y);

	return vec3(d.x * (a * cos(f)), a * sin(f), d.y * (a * cos(f)));
}

vec3 sin_wave(vec2 origin, vec3 pos, inout vec3 tangent, inout vec3 binormal)
{
	float steepness  = .3;
	float k = (6.283185) * 3;
	float c = sqrt(9.81 / k); // speed
	float a = steepness / k;  // amplitude

	float x1 = pos.x - origin.x;
	float x2 = pos.z - origin.y;
	float offset  = sqrt( (x1 * x1) + (x2 * x2) );
	float f = k * (offset - (timer * 5));

	float factor = 1 / (offset * offset);

	tangent += vec3(
		(steepness * sin(f)),
		(steepness * cos(f)),
		(steepness * sin(f))) * factor;

	binormal += vec3(
		(steepness * sin(f)),
		(steepness * cos(f)),
		(steepness * sin(f))) * factor;

		float height = a * sin(f);
		if(offset > 1) height *= factor;

	return vec3(0, height, 0);
}