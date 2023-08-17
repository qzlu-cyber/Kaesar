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

layout(binding = 4) uniform Shadow
{
	mat4 u_LightViewProjection;
} shadow;

struct VS_OUT 
{
	vec3 v_FragPos;
	vec3 v_Normal;
	vec2 v_TexCroods;
	vec4 v_FragPosLightSpace;
};

layout(location = 0) out VS_OUT vs_out;

void main()
{
	vs_out.v_FragPos = vec3(transform.u_Trans * vec4(a_Position, 1.0));
	vs_out.v_Normal = mat3(transpose(inverse(transform.u_Trans))) * a_Normal;
	vs_out.v_TexCroods = a_TexCroods;
    vs_out.v_FragPosLightSpace = shadow.u_LightViewProjection * vec4(vs_out.v_FragPos, 1.0); // 将顶点从世界空间转换到光空间
	gl_Position = camera.u_ViewProjection * transform.u_Trans * vec4(a_Position, 1.0);
}

#type fragment

#version 460

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D texture_diffuse;
layout(binding = 1) uniform sampler2D texture_specular;
layout(binding = 2) uniform sampler2D texture_normal;
layout(binding = 3) uniform sampler2D shadowMap;

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

struct VS_OUT 
{
	vec3 v_FragPos;
	vec3 v_Normal;
	vec2 v_TexCroods;
	vec4 v_FragPosLightSpace;
};

layout(location = 0) in VS_OUT fs_in;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);
vec3 CaculateDirectionalLight(DLight light, vec3 normal, vec3 viewDir);
vec3 CaculatePointLight(PLight light, vec3 normal, vec3 viewDir);
vec3 CaculateSpotLight(SLight light, vec3 normal, vec3 viewDir);

void main()
{	
	vec3 normal = normalize(fs_in.v_Normal);
    vec3 viewDir = normalize(camera.u_CameraPos - fs_in.v_FragPos);

    vec3 result = vec3(0);
    result += CaculateDirectionalLight(lights.directionalLight, normal, viewDir);
    result += CaculatePointLight(lights.pointLight, normal, viewDir);
    result += CaculateSpotLight(lights.spotLight, normal, viewDir);

	FragColor = vec4(result, 1.0);
}

vec3 CaculateDirectionalLight(DLight light, vec3 normal, vec3 viewDir)
{
    vec3 color = vec3(texture(texture_diffuse, fs_in.v_TexCroods));
    vec3 lightColor = vec3(1.0);

    vec3 ambient = 0.3 * params.dirIntensity * color;

    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * params.dirIntensity * lightColor;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64);
    vec3 specular = spec * params.dirIntensity * lightColor;

    float shadow = ShadowCalculation(fs_in.v_FragPosLightSpace, normal, lightDir);       
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color; 

    return lighting;
}

vec3 CaculatePointLight(PLight light, vec3 normal, vec3 viewDir)
{
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse, fs_in.v_TexCroods));

    vec3 lightDir = normalize(light.position - fs_in.v_FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * vec3(texture(texture_diffuse, fs_in.v_TexCroods));

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64);
    vec3 specular = spec * light.specular * vec3(texture(texture_specular, fs_in.v_TexCroods));

    float dist = length(light.position - fs_in.v_FragPos);
    float attenuation = 1.0 / (1.0 + (params.pointLinear * dist) + (params.pointQuadratic * dist * dist));

    return (ambient + diffuse + specular) * attenuation;
}

vec3 CaculateSpotLight(SLight light, vec3 normal, vec3 viewDir)
{
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse, fs_in.v_TexCroods));

    vec3 lightDir = normalize(light.position - fs_in.v_FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * vec3(texture(texture_diffuse, fs_in.v_TexCroods));

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64);
    vec3 specular = spec * light.specular * vec3(texture(texture_specular, fs_in.v_TexCroods));

    float dist = length(light.position - fs_in.v_FragPos);
    float attenuation = 1.0 / (1.0 + (params.spotLinear * dist) + (params.spotQuadratic * dist * dist));

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = params.innerCutOff - params.outerCutOff;
    float intensity = clamp((theta - params.outerCutOff) / epsilon, 0.0, 1.0);

    return (ambient + diffuse + specular) * attenuation * intensity;
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // 执行透视除法，转为 NDC 坐标
    // 当在顶点着色器输出一个裁切空间顶点位置到 gl_Position 时，OpenGL 自动进行透视除法，将裁切空间坐标的范围 -w 到 w 转为 -1 到 1。
    // 由于裁切空间的 FragPosLightSpace 并不会通过 gl_Position 传到片段着色器里，所以必须自己做透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 转为 [0, 1] 范围的坐标
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近点的深度（使用 [0, 1] 范围的坐标对深度贴图采样）
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;

    float shadow = 0.0;
    // textureSize 返回一个给定采样器纹理的 0 级 mipmap 的 vec2 类型的宽和高
    // 用 1 除以它返回一个单独纹理像素的大小
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    float bias = 0.005;

    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            // 检查当前片段周围 9 个片段的深度值
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    // 取平均值
    shadow /= 9.0;

    return shadow;
}