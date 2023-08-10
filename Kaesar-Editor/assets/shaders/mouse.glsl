// Basic diffuse Shader
#type vertex

#version 460 core
	
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;
layout(location = 2) in vec3 a_Normal;

layout(std140, binding = 0) uniform camera
{
	mat4 u_ViewProjection;
};

layout(binding = 1) uniform Transform
{
	 mat4 u_Trans;
	 vec4 u_LightPos;
} transform;

out vec3 v_pos;
out vec2 v_uv;
out vec3 v_normal;

void main()
{
	v_normal = mat3(transpose(inverse(transform.u_Trans))) * a_Normal;
	v_pos = vec3(transform.u_Trans * vec4(a_Position, 1.0));
	v_uv = a_TexCoords;
	gl_Position = u_ViewProjection * transform.u_Trans * vec4(a_Position, 1.0);
}

#type fragment

#version 460 core

layout(location = 0) 

out int id;

in vec3 v_pos;
in vec2 v_uv;
in vec3 v_normal;

uniform int u_ID;

void main()
{		
	id = u_ID;
}