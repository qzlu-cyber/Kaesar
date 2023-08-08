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

uniform sampler2DMS u_ScreenTexture;

void main()
{
    ivec2 texturePosition = ivec2(gl_FragCoord.x, gl_FragCoord.y);
    vec4 colorSample0 = texelFetch(u_ScreenTexture, texturePosition, 0);
	vec4 colorSample1 = texelFetch(u_ScreenTexture, texturePosition, 1);
	vec4 colorSample2 = texelFetch(u_ScreenTexture, texturePosition, 2);
	vec4 colorSample3 = texelFetch(u_ScreenTexture, texturePosition, 3);

	vec4 antialiased = (colorSample0 + colorSample1 + colorSample2 + colorSample3) / 4.0f;

    FragColor = antialiased;
}