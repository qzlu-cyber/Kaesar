// Basic diffuse Shader
#type vertex

#version 330 core
	
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_uv;
layout(location = 2) in vec3 a_normal;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

out vec3 v_pos;
out vec2 v_uv;
out vec3 v_normal;

void main()
{
	v_normal = mat3(transpose(inverse(u_Model))) * a_normal;
	v_pos = vec3(u_Model*vec4(a_pos, 1.0));
	v_uv = a_uv;
	gl_Position = u_ViewProjection * u_Model * vec4(a_pos, 1.0);
}

#type fragment

#version 330 core

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