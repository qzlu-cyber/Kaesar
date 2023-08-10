#type vertex

#version 460 core
	
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoords;

layout(std140, binding = 0) uniform camera
{
	mat4 u_ViewProjection;
};

layout(binding = 1) uniform Transform
{
	 mat4 u_Transform;
	 vec4 u_LightPos;
} transform;

out vec2 v_TexCoords;

void main(){
	v_TexCoords = a_TexCoords;
	gl_Position = u_ViewProjection * transform.u_Transform * vec4(a_Position, 1.0);
}

#type fragment

#version 460 core

out vec4 FragColor;
out int id;

uniform sampler2D texture_diffuse1;

in vec2 v_TexCoords;

void main()
{	
	vec3 color = texture(texture_diffuse1, v_TexCoords).rgb;
	FragColor = vec4(color, 1.0f);
}
