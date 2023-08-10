#type vertex

#version 460 core
	
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;
layout(location = 2) in vec3 a_Normal;

layout(binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
	vec4 u_CameraPos;
} camera;

layout(binding = 1) uniform Transform
{
	mat4 u_Trans;
	vec4 u_LightPos;
	int u_ID;
} transform;

void main()
{
	gl_Position = camera.u_ViewProjection * transform.u_Trans * vec4(a_Position, 1.0);
}

#type fragment

#version 460 core

layout(location = 0) out int id;

layout(binding = 1) uniform Transform
{
	mat4 u_Trans;
	vec4 u_LightPos;
	int u_ID;
} transform;

void main()
{		
	id = transform.u_ID;
}