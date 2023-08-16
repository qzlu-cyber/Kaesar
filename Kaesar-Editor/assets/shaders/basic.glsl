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

layout(binding = 1) uniform Transform
{
	mat4 u_Trans;
	int u_ID;
} transform;

layout(location = 0) out vec3 v_FragPos;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec2 v_TexCroods;

void main()
{
	v_FragPos = vec3(transform.u_Trans * vec4(a_Position, 1.0));
	v_Normal = mat3(transpose(inverse(transform.u_Trans))) * a_Normal;
	v_TexCroods = a_TexCroods;
	gl_Position = camera.u_ViewProjection * transform.u_Trans * vec4(a_Position, 1.0);
}

#type fragment

#version 460

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec3 v_FragPos;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_TexCroods;

layout(binding = 0) uniform sampler2D texture_diffuse;
layout(binding = 1) uniform sampler2D texture_specular;
layout(binding = 2) uniform sampler2D texture_normal;

layout(binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
	vec3 u_CameraPos;
} camera;

layout(binding = 1) uniform Transform
{
    mat4 u_Trans;
	int u_ID;
} transform;

struct DLight
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PLight
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SLight
{
    vec3 position; 
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct LightsParams
{
    float pointLinear;
    float pointQuadratic;

    float spotLinear;
    float spotQuadratic;
    float innerCutOff;
    float outerCutOff;
};

layout(binding = 2) uniform Lights
{
    PLight pointLight;
    SLight spotLight;
    DLight directionalLight;
} lights;

layout(binding = 3) uniform Params
{
    float dirIntensity;

    float pointLinear;
    float pointQuadratic;

    float spotLinear;
    float spotQuadratic;
    float innerCutOff;
    float outerCutOff;
} params;

vec3 CaculateDirectionalLight(DLight light, vec3 normal, vec3 viewDir);
vec3 CaculatePointLight(PLight light, vec3 normal, vec3 viewDir);
vec3 CaculateSpotLight(SLight light, vec3 normal, vec3 viewDir);

void main()
{	
	vec3 normal = normalize(v_Normal);
    vec3 viewDir = normalize(camera.u_CameraPos - v_FragPos);

    vec3 result = vec3(0);
    result += CaculateDirectionalLight(lights.directionalLight, normal, viewDir);
    result += CaculatePointLight(lights.pointLight, normal, viewDir);
    result += CaculateSpotLight(lights.spotLight, normal, viewDir);

	FragColor = vec4(result, 1.0);
}

vec3 CaculateDirectionalLight(DLight light, vec3 normal, vec3 viewDir)
{
    vec3 ambient = params.dirIntensity * 0.05 * vec3(texture(texture_diffuse, v_TexCroods));

    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * params.dirIntensity * 0.4 * vec3(texture(texture_diffuse, v_TexCroods));

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64);
    vec3 specular = spec * params.dirIntensity * 0.5 * vec3(texture(texture_specular, v_TexCroods));

    return (ambient + diffuse + specular);
}

vec3 CaculatePointLight(PLight light, vec3 normal, vec3 viewDir)
{
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse, v_TexCroods));

    vec3 lightDir = normalize(light.position - v_FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * vec3(texture(texture_diffuse, v_TexCroods));

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64);
    vec3 specular = spec * light.specular * vec3(texture(texture_specular, v_TexCroods));

    float dist = length(light.position - v_FragPos);
    float attenuation = 1.0 / (1.0 + (params.pointLinear * dist) + (params.pointQuadratic * dist * dist));

    return (ambient + diffuse + specular) * attenuation;
}

vec3 CaculateSpotLight(SLight light, vec3 normal, vec3 viewDir)
{
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse, v_TexCroods));

    vec3 lightDir = normalize(light.position - v_FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * vec3(texture(texture_diffuse, v_TexCroods));

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64);
    vec3 specular = spec * light.specular * vec3(texture(texture_specular, v_TexCroods));

    float dist = length(light.position - v_FragPos);
    float attenuation = 1.0 / (1.0 + (params.spotLinear * dist) + (params.spotQuadratic * dist * dist));

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = params.innerCutOff - params.outerCutOff;
    float intensity = clamp((theta - params.outerCutOff) / epsilon, 0.0, 1.0);

    return (ambient + diffuse + specular) * attenuation * intensity;
}