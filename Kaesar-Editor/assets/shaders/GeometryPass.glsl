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
	vec3 u_CameraPos;
} camera;

layout(push_constant) uniform Transform
{
	mat4 u_Transform;
	int id;
} transform;

struct VS_OUT 
{
	vec3 v_FragPos;
	vec3 v_Normal;
	vec2 v_TexCroods;
    mat3 v_TBN;
};

layout(location = 0) out VS_OUT vs_out;
layout(location = 8) out flat int id;

void main()
{
    vs_out.v_FragPos = vec3(transform.u_Transform * vec4(a_Position, 1.0));
    vs_out.v_TexCroods = a_TexCroods;

    mat3 normalMatrix = mat3(transpose(inverse(transform.u_Transform)));
    vec3 T = normalize(normalMatrix * a_Tangent);
    vec3 N = normalize(normalMatrix * a_Normal);
    T = normalize(T - dot(T, N) * N); // 施密特正交化
    vec3 B = cross(N, T);
    vs_out.v_TBN = mat3(T, B, N);

	vs_out.v_Normal = normalMatrix * a_Normal;

	id = transform.id;

    gl_Position = camera.u_ViewProjection * transform.u_Transform * vec4(a_Position, 1.0);
}

#type fragment

#version 460

layout(location = 0) out vec3 gPosistion;	
layout(location = 1) out vec3 gNormal;	
layout(location = 2) out vec4 gAlbedoSpec;
layout(location = 3) out int gEntityID;

layout(binding = 0) uniform sampler2D DiffuseMap;
layout(binding = 1) uniform sampler2D SpecularMap;
layout(binding = 2) uniform sampler2D NormalMap;

struct Material
{
	vec4 color;
};

layout(push_constant) uniform pushConstant
{
	Material material;
	int id;
	int HasDiffuseMap;
	int HasNormalMap;
} pc;

struct VS_OUT 
{
	vec3 v_FragPos;
	vec3 v_Normal;
	vec2 v_TexCroods;
    mat3 v_TBN;
};

layout(location = 0) in VS_OUT fs_in;
layout(location = 8) in	flat int id;

void main()
{
	gPosistion = fs_in.v_FragPos;

	if (pc.HasDiffuseMap == 1)
	{
		gAlbedoSpec.rgb = texture(DiffuseMap, fs_in.v_TexCroods).rgb;
	}
	else
	{
		gAlbedoSpec.a = texture(SpecularMap, fs_in.v_TexCroods).r;
	}

	if (pc.HasNormalMap == 1)
	{
		vec3 normal = texture(NormalMap, fs_in.v_TexCroods).rgb;
		normal = normalize(normal * 2.0 - 1.0);
		gNormal = normalize(fs_in.v_TBN * normal); 

	}
	else
	{
		gNormal = fs_in.v_Normal;
	}

	gEntityID = id;
}