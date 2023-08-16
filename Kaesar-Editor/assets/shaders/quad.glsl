#type vertex

#version 460 core

layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec2 a_TexCoords;

layout(location = 0) out vec2 v_TexCoords;

void main()
{
    gl_Position = vec4(a_Position, 0.0, 1.0);
    v_TexCoords = a_TexCoords;
}

#type fragment

#version 460 core

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 v_TexCoords;

layout(binding = 0) uniform sampler2DMS u_ScreenTexture;

layout(push_constant) uniform pushConstants
{
	float exposure;
} pc;

void main()
{
    ivec2 texturePosition = ivec2(gl_FragCoord.x, gl_FragCoord.y);
    vec4 colorSample0 = texelFetch(u_ScreenTexture, texturePosition, 0);
	vec4 colorSample1 = texelFetch(u_ScreenTexture, texturePosition, 1);
	vec4 colorSample2 = texelFetch(u_ScreenTexture, texturePosition, 2);
	vec4 colorSample3 = texelFetch(u_ScreenTexture, texturePosition, 3);

	vec4 antialiased = (colorSample0 + colorSample1 + colorSample2 + colorSample3) / 4.0f;

    float gamma = 2.2f;
    vec3 hdrColor = antialiased.rgb;

    // Reinhard 色调映射
    vec3 mapped = vec3(1.0) - exp(-hdrColor * pc.exposure);
    // gamma 矫正 
    // mapped = pow(mapped, vec3(1.0 / gamma));

    FragColor = vec4(mapped, 1.0);
}