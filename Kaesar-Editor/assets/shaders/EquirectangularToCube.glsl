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
    gl_Position = camera.u_Projection * camera.u_View * vec4(a_Position, 1.0);
}

#type fragment

#version 460

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec3 v_Position;

layout(binding = 0) uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;

    return uv;
}

void main()
{		
    vec2 uv = SampleSphericalMap(normalize(v_Position));
    vec3 color = texture(equirectangularMap, uv).rgb;

    FragColor = vec4(color, 1.0);
}
