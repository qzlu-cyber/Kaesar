#type vertex

#version 460
	
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCroods;

layout(binding = 4) uniform Shadow
{
	mat4 u_LightViewProjection;
} shadow;

layout(push_constant) uniform Transform
{
	mat4 u_Transform;
} transform;

void main()
{
    gl_Position = shadow.u_LightViewProjection * transform.u_Transform * vec4(a_Position, 1.0);
}

#type fragment

#version 460

void main()
{
    // 没有颜色缓冲，最后的片段不需要任何处理
    // gl_FragDepth = gl_FragCoord.z;
}