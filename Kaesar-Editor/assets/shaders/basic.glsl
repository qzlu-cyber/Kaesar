#type vertex

#version 330 core
	
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoords;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;
uniform mat4 u_Trans;

out vec2 v_TexCoords;

void main(){
	v_TexCoords = a_TexCoords;
	gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
}

#type fragment

#version 330 core

out vec4 FragColor;
out int id;

uniform int u_ID;
uniform sampler2D u_Texture;

in vec2 v_TexCoords;

void main()
{	
	FragColor = texture(u_Texture, v_TexCoords);
	id = u_ID;
}
