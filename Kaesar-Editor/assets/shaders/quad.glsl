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

layout(binding = 0) uniform sampler2D u_Texture;

layout(push_constant) uniform pushConstants
{
	float exposure;
    float gamma;
} pc;

void main()
{
    vec4 antialiased = texture(u_Texture, v_TexCoords);

    vec3 hdrColor = antialiased.rgb;

    // Reinhard 色调映射
    vec3 mapped = vec3(1.0) - exp(-hdrColor * pc.exposure);
    // gamma 矫正 
    mapped = pow(mapped, vec3(1.0 / pc.gamma));

    FragColor = vec4(mapped, 1.0);
}