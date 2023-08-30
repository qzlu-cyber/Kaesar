#type vertex

#version 460

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;

layout(push_constant) uniform Camera
{
	mat4 u_View;
	mat4 u_Projection;
} camera;

layout(location = 0) out vec3 v_Position;

void main()
{
    v_Position = a_Position;

    mat4 view = mat4(mat3(camera.u_View)); // 移除观察矩阵中的位移部分，让移动不会影响天空盒的位置向量
    vec4 pos = camera.u_Projection * view * vec4(a_Position, 1.0);
    // 将输出位置的 z 分量等于它的 w 分量，当透视除法执行之后，z 分量会变为 w / w = 1.0，让 z 分量永远等于 1.0（最大的深度值），这样就能保证天空盒始终在场景的最后面
    gl_Position = pos.xyww;
}

#type fragment

#version 460

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec3 v_Position;

layout(binding = 0) uniform samplerCube u_Skybox;

layout(push_constant) uniform pushConstants
{
    float exposure;
    float gamma;
} pc;

void main()
{
    vec3 envColor = texture(u_Skybox, v_Position).rgb;

    vec3 hdrColor = envColor;

    // Reinhard 色调映射
    vec3 mapped = vec3(1.0) - exp(-hdrColor * 0.5);
    // gamma 矫正 
    mapped = pow(mapped, vec3(1.0 / 2.2));

    FragColor = vec4(mapped, 1.0);
}