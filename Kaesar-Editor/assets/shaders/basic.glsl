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
layout(binding = 3) uniform sampler2DShadow shadowMap;
layout(binding = 4) uniform sampler1D distribution0;
layout(binding = 5) uniform sampler1D distribution1;

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

layout(push_constant) uniform pc
{
	float size;
} push;

struct VS_OUT 
{
	vec3 v_FragPos;
	vec3 v_Normal;
	vec2 v_TexCroods;
	vec4 v_FragPosLightSpace;
};

layout(location = 0) in VS_OUT fs_in;

const int numPCFSamples = 32; // 阴影映射中的 PCF 样本数
const int numBlockerSearchSamples = 32; // 阴影映射中的阴影搜索样本数

vec3 CaculateDirectionalLight(DLight light, vec3 normal, vec3 viewDir);
vec3 CaculatePointLight(PLight light, vec3 normal, vec3 viewDir);
vec3 CaculateSpotLight(SLight light, vec3 normal, vec3 viewDir);

// 通过在指定的分布中进行采样，生成一个在二维平面上的随机方向向量
vec2 RandomDirection(sampler1D distribution, float u)
{
    // 从采样器 sampler1D 中使用参数 u 进行采样，返回一个二维向量，其中 x 分量和 y 分量分别存储了从采样器中获取的数据
    // 将提取的二维向量的每个分量都乘以 2，从而将分量的范围从 [0, 1] 映射到 [0, 2]
    // 将得到的向量的每个分量都减去 1，将范围从 [0, 2] 映射到 [-1, 1]
   return texture(distribution, u).xy * 2 - vec2(1);
}

/// 计算在某个光源到接收器之间搜索宽度
/// params uvLightSize 表示光源的大小，通常在纹理坐标中定义
/// params receiverDistance 表示接收器（通常是相机或物体）与光源之间的距离
/// return 搜索宽度
float SearchWidth(float uvLightSize, float receiverDistance)
{
	return uvLightSize * (receiverDistance - 1.0) / receiverDistance;
}

/// 计算平行光源下阴影映射中的遮挡距离
/// params shadowCoords 表示从片段着色器中计算出的阴影坐标
/// params shadowMap 阴影贴图的采样器，用于从贴图中获取深度信息
/// params uvLightSize 表示光源的大小，通常在纹理坐标中定义
/// params bias 表示阴影映射中的偏移量
/// return 遮挡距离
float FindBlockerDistance_DirectionalLight(vec3 shadowCoords, sampler2DShadow shadowMap, float uvLightSize,float bias)
{
	int blockers = 0; // 遮挡者的数量
	float avgBlockerDistance = 0; // 遮挡者的深度值

    // 计算搜索宽度，用于确定在阴影贴图中搜索遮挡者的范围
	float searchWidth = SearchWidth(uvLightSize, shadowCoords.z);

    // 从阴影贴图中搜索遮挡者
	for (int i = 0; i < numBlockerSearchSamples; i++)
	{
        // 计算用于采样的纹理坐标，使用了 RandomDirection 将随机方向与光源大小结合，以便在光源区域内随机采样
		vec3 uvc =  vec3(shadowCoords.xy + RandomDirection(distribution0, i / float(numPCFSamples)) * uvLightSize, (shadowCoords.z - bias));
		float z = texture(shadowMap, uvc); // 从阴影贴图中获取遮挡者的深度
		// 如果深度大于 0.5，表示该位置有遮挡者
        if (z > 0.5) 
		{
			blockers++;
			avgBlockerDistance += z;
		}
	}

	if (blockers > 0)
		return avgBlockerDistance / blockers;
	else
		return -1;
}

/// 计算平行光源下阴影映射中的 PCF 阴影
/// params shadowCoords 表示从片段着色器中计算出的阴影坐标
/// params shadowMap 阴影贴图的采样器，用于从贴图中获取深度信息
/// params uvRadius 表示光源的半径，通常在纹理坐标中定义
/// params bias 表示阴影映射中的偏移量
float PCF_DirectionalLight(vec3 shadowCoords, sampler2DShadow shadowMap, float uvRadius, float bias)
{
	float sum = 0; // 存储多个采样点的深度之和

    // 在阴影贴图中进行多次采样，以计算平均深度
	for (int i = 0; i < numPCFSamples; i++)
	{
        // 在光源区域内随机采样
		vec3 uvc =  vec3(shadowCoords.xy + RandomDirection(distribution1, i / float(numPCFSamples)) * uvRadius, (shadowCoords.z - bias));
		float z = texture(shadowMap, uvc);
		sum += z;
	}

	return sum / numPCFSamples;
}

/// 计算平行光源下阴影映射中的 PCSS 阴影
/// params shadowCoords 表示从片段着色器中计算出的阴影坐标
/// params shadowMap 阴影贴图的采样器，用于从贴图中获取深度信息
/// params uvLightSize 表示光源的大小，通常在纹理坐标中定义
/// params bias 表示阴影映射中的偏移量
float PCSS_DirectionalLight(vec3 shadowCoords, sampler2DShadow shadowMap, float uvLightSize, float bias)
{
	// Blocker search
	float blockerDistance = FindBlockerDistance_DirectionalLight(shadowCoords, shadowMap, uvLightSize, bias);

	if (blockerDistance == -1) // 没有遮挡者，不存在阴影
		return 0;		

	// Penumbra estimation
	float penumbraWidth = (shadowCoords.z - blockerDistance) / blockerDistance;

	// PCF
	float uvRadius = penumbraWidth * uvLightSize * 1.0 / shadowCoords.z; // 计算用于 PCF 采样的半径

	return PCF_DirectionalLight(shadowCoords, shadowMap, uvRadius, bias);
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 执行透视除法，转为 NDC 坐标
    // 当在顶点着色器输出一个裁切空间顶点位置到 gl_Position 时，OpenGL 自动进行透视除法，将裁切空间坐标的范围 -w 到 w 转为 -1 到 1。
    // 由于裁切空间的 FragPosLightSpace 并不会通过 gl_Position 传到片段着色器里，所以必须自己做透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 转为 [0, 1] 范围的坐标
    projCoords = projCoords * 0.5 + 0.5;

    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;

    vec3 normal = normalize(fs_in.v_Normal);
    vec3 lightDir = normalize(-lights.directionalLight.direction);
    float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.001); 

    float shadow = PCSS_DirectionalLight(projCoords, shadowMap, push.size, bias);
    
    if(projCoords.z > 1.0) // 超出光源视锥，不考虑阴影
        shadow = 0.0;

    return shadow;
}

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

    float shadow = ShadowCalculation(fs_in.v_FragPosLightSpace);
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