#type vertex

#version 460
	
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCroods;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;

layout(binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
	vec3 u_CameraPos;
} camera;

layout(binding = 1) uniform Transform
{
	mat4 u_Trans;
	int u_ID;
} transform;

void main()
{
	gl_Position = camera.u_ViewProjection * transform.u_Trans * vec4(a_Position, 1.0);
}

#type fragment

#version 460

layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0);
}