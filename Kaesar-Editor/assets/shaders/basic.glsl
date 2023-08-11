// Basic diffuse Shader
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
	vec4 u_CameraPos;
} camera;

layout(binding = 1) uniform Transform
{
	 mat4 u_Trans;
	 vec4 u_LightPos;
} transform;

layout(location = 0) out vec2 v_TexCroods;

void main()
{
	v_TexCroods = a_TexCroods;
	gl_Position = camera.u_ViewProjection * transform.u_Trans * vec4(a_Position, 1.0);
}

#type fragment

#version 460

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 v_TexCroods;	

layout(binding = 0) uniform sampler2D texture_diffuse;

void main()
{	
	vec3 color = texture(texture_diffuse, v_TexCroods).rgb;

	FragColor = vec4(color,1.0);
}