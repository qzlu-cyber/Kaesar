#type vertex

#version 330 core

layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec2 a_TexCoords;

out vec2 v_TexCoords;

void main()
{
    gl_Position = vec4(a_Position, 0.0, 1.0); 
    v_TexCoords = a_TexCoords;
}

#type fragment

#version 330 core

out vec4 FragColor;

in vec2 v_TexCoords;

uniform sampler2D u_ScreenTexture;

void main()
{
    vec3 result = texture(u_ScreenTexture, v_TexCoords).rgb;    
    FragColor = vec4(result, 1.0);
}