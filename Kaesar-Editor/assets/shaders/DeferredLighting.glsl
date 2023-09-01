// Basic diffuse Shader
#type vertex

#version 460
	
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;

layout(location = 0) out vec2 v_TexCoords;

void main()
{
	v_TexCoords = a_TexCoords;

	gl_Position = vec4(a_Position, 1.0);
}

#type fragment

#version 460

layout(location = 0) out vec4 FragColor;

// GBuffer samplers
layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gAlbedoSpec;
layout(binding = 6) uniform sampler2D gRoughMetalAO;

// Shadow related samplers
layout(binding = 3) uniform sampler2DShadow shadowMap;
layout(binding = 4) uniform sampler1D distribution0;
layout(binding = 5) uniform sampler1D distribution1;

// IBL
layout(binding = 7) uniform samplerCube irradianceMap;
layout(binding = 8) uniform samplerCube prefilterMap;
layout(binding = 9) uniform sampler2D   brdfLUT;

//-----------------------------------------------UNIFORM BUFFERS-----------------------------------------//

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

struct DLight
{
    vec4 direction;
    vec4 color;
};

struct PLight
{
    vec4 position;
    vec4 color;
};

struct SLight
{
    vec4 position; 
    vec4 direction;
    vec4 color;
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
    PLight pointLight[5];
    SLight spotLight[5];
    DLight directionalLight;
} lights;

layout(binding = 3) uniform Params
{
    LightsParams lightsParams[5];
} params;

//-----------------------------------------------PUSH CONSTANT-----------------------------------------//
layout(push_constant) uniform pushConstants
{
    float exposure;
	float gamma;
	float size;
    float intensity;
    float near; // 阴影映射中的阴影搜索范围的近端
    int numPCFSamples; // 阴影映射中的 PCF 样本数
    int numBlockerSearchSamples; // 阴影映射中的阴影搜索样本数
    int softShadow; // 是否启用软阴影
} pc;

struct VS_OUT 
{
	vec3 v_FragPos;
	vec3 v_Normal;
	vec2 v_TexCroods;
	vec4 v_FragPosLightSpace;
};

layout(location = 0) in vec2 v_TexCoords;

//-----------------------------------------Shadow calculation functions-------------------------------//
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
	return uvLightSize * (receiverDistance - pc.near) / receiverDistance;
}

/// 计算平行光源下阴影映射中的遮挡距离
/// params shadowCoords 表示从片段着色器中计算出的阴影坐标
/// params shadowMap 阴影贴图的采样器，用于从贴图中获取深度信息
/// params uvLightSize 表示光源的大小，通常在纹理坐标中定义
/// params bias 表示阴影映射中的偏移量
/// return 遮挡距离
float FindBlockerDistance_DirectionalLight(vec3 shadowCoords, sampler2DShadow shadowMap, float uvLightSize, float bias)
{
	int blockers = 0; // 遮挡者的数量
	float avgBlockerDistance = 0; // 遮挡者的深度值

    // 计算搜索宽度，用于确定在阴影贴图中搜索遮挡者的范围
	float searchWidth = SearchWidth(uvLightSize, shadowCoords.z);

    // 从阴影贴图中搜索遮挡者
	for (int i = 0; i < pc.numBlockerSearchSamples; i++)
	{
        // 计算用于采样的纹理坐标，使用了 RandomDirection 将随机方向与光源大小结合，以便在光源区域内随机采样
		vec3 uvc = vec3(shadowCoords.xy + RandomDirection(distribution0, i / float(pc.numPCFSamples)) * uvLightSize, (shadowCoords.z - bias));
		float z = texture(shadowMap, uvc); // 从阴影贴图中获取遮挡者的深度
		
        if (z < 0.5) // 如果遮挡者的深度小于当前片段的深度，则表示该片段被遮挡，但是如果和 (shadowCoords.z - bias) 比较，阴影走样很严重😅 和 0.5 比较结果要好得多😅 Why? 🤔
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
	for (int i = 0; i < pc.numPCFSamples; i++)
	{
        // 在光源区域内随机采样
		vec3 uvc = vec3(shadowCoords.xy + RandomDirection(distribution1, i / float(pc.numPCFSamples)) * uvRadius, (shadowCoords.z - bias));
		float z = texture(shadowMap, uvc);
		sum += z;
	}

	return sum / pc.numPCFSamples;
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
	float uvRadius = penumbraWidth * uvLightSize * pc.near / shadowCoords.z; // 计算用于 PCF 采样的半径

	return PCF_DirectionalLight(shadowCoords, shadowMap, uvRadius, bias);
}

float SoftShadowCalculation(vec4 fragPosLightSpace, float bias)
{
    // 执行透视除法，转为 NDC 坐标
    // 当在顶点着色器输出一个裁切空间顶点位置到 gl_Position 时，OpenGL 自动进行透视除法，将裁切空间坐标的范围 -w 到 w 转为 -1 到 1。
    // 由于裁切空间的 FragPosLightSpace 并不会通过 gl_Position 传到片段着色器里，所以必须自己做透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 转为 [0, 1] 范围的坐标
    projCoords = projCoords * 0.5 + 0.5;

    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;
    
    float shadow = 0;

    if(projCoords.z > 1.0) // 超出光源视锥，不考虑阴影
        shadow = 0.0;

    shadow = PCSS_DirectionalLight(projCoords, shadowMap, pc.size, bias);

    return shadow;
}

float HardShadowCalculation(vec4 fragPosLightSpace, float bias)
{
    // 执行透视除法，转为 NDC 坐标
    // 当在顶点着色器输出一个裁切空间顶点位置到 gl_Position 时，OpenGL 自动进行透视除法，将裁切空间坐标的范围 -w 到 w 转为 -1 到 1。
    // 由于裁切空间的 FragPosLightSpace 并不会通过 gl_Position 传到片段着色器里，所以必须自己做透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 转为 [0, 1] 范围的坐标
    projCoords = projCoords * 0.5 + 0.5;

    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;

    float shadow = 0.0;
    // textureSize 返回一个给定采样器纹理的 0 级 mipmap 的 vec2 类型的宽和高
    // 用 1 除以它返回一个单独纹理像素的大小
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            // 检查当前片段周围 9 个片段的深度值
            vec3 uvc = vec3(projCoords.xy + vec2(x, y) * texelSize, (projCoords.z - bias));
            float pcfDepth = texture(shadowMap, uvc);
            shadow += (pcfDepth < (projCoords.z - bias)) ? 1 : 0;
        }
    }

    // 取平均值
    shadow /= 9.0;

    return shadow;
}

//--------------------------------------------------PBR------------------------------------------------//

const float PI = 3.14159265359;

/// 法线分布函数 D (Trowbridge-Reitz GGX)
/// params N 法线
/// params H 入射光线与视线的半程向量
/// params roughness 粗糙度
/// return 分布函数值
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a2 = roughness * roughness;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / denom;
}

/// 几何函数 G (Schlick GGX)
/// params NdotV 法线与视线的点积
/// params roughness 粗糙度
/// return 几何函数值
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

/// 几何函数 G (Smith Joint GGX)
/// params N 法线
/// params V 视线
/// params L 入射光线
/// params roughness 粗糙度
/// return 几何函数值
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

/// 菲涅尔方程 F (Fresnel-Schlick 近似)
/// params cosTheta 半程向量与视线（或法线）之间的夹角的余弦值
/// params F0 材质的基础反射率
/// return 菲涅尔反射率
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

/// 计算表面的光照颜色
/// params L 入射光线方向向量
/// params N 表面法线向量
/// params V 视线方向向量
/// params Ra 环境光颜色
/// params F0 材质的基础反射率
/// params R 粗糙度参数
/// params M 金属度参数
/// params A 环境光颜色
/// return 计算得到的光照颜色
vec3 CalculateL0(vec3 L, vec3 N, vec3 V, vec3 Ra, vec3 F0, float R, float M, vec3 A)
{
    vec3 H = normalize(L + V); // 半程向量

    // Cook-Torrance BRDF
    float D = DistributionGGX(N, H, R);
    float G = GeometrySmith(N, V, L, R);
    vec3  F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 F_lambert = A / PI; // 漫反射项

    vec3 Nominator = D * G * F;
    float Denominator = 4 * max(dot(V, N), 0.0) * max(dot(L, N), 0.0) + 0.001;
    vec3 F_cook = Nominator / Denominator; // 镜面反射项

    vec3 Ks = F; // 反射光线所占比率
    vec3 Kd = vec3(1.0) - Ks; // 折射（漫反射）光线所占比率

    Kd *= 1.0 - M; // 考虑金属度的调整

	float NdotL = max(dot(N, L), 0.0);

    vec3 FR = Kd * F_lambert + Ks * F_cook;

    return FR * Ra * NdotL;
}

void main()
{	
    vec3 fragPos    = texture(gPosition, v_TexCoords).rgb;
	vec3 N          = texture(gNormal, v_TexCoords).rgb;
	vec3 Albedo     = pow(texture(gAlbedoSpec, v_TexCoords).rgb, vec3(pc.gamma)); // Gamma 矫正
	float Roughness = texture(gRoughMetalAO, v_TexCoords).r;
	float Metallic  = texture(gRoughMetalAO, v_TexCoords).g;
	float AO		= texture(gRoughMetalAO, v_TexCoords).b;

    vec3 V = normalize(camera.u_CameraPos - fragPos);
    vec3 R = reflect(-V, N);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, Albedo, Metallic);

    vec3 L0 = vec3(0.0);

    vec4 FragPosLightSpace = shadow.u_LightViewProjection * vec4(fragPos, 1.0);

    vec3  L = normalize(-lights.directionalLight.direction.rgb);
    float bias = max(0.01 * (1.0 - dot(N, L)), 0.001);

    L0 += CalculateL0(L, N, V, lights.directionalLight.color.rgb, F0, Roughness, Metallic, Albedo);

    for (int i = 0; i < 5; i++)
    {
        vec3 L = normalize(lights.pointLight[i].position.rgb - fragPos);
        float dist = length(lights.pointLight[i].position.rgb - fragPos);
        float attenuation = 1.0 / (1.0 + dist * (params.lightsParams[i].pointLinear + params.lightsParams[i].pointQuadratic * dist));

        vec3 Ra = lights.pointLight[i].color.rgb * attenuation;

        L0 += CalculateL0(L, N, V, Ra, F0, Roughness, Metallic, Albedo);
    }

    float shadow = 0;
    if (pc.softShadow == 1)
    {
        shadow = SoftShadowCalculation(FragPosLightSpace, bias);
    }
    else
    {
        shadow = HardShadowCalculation(FragPosLightSpace, bias);
    }

	vec3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, Roughness);

	vec3 Ks = F;
	vec3 Kd = 1.0 - Ks;
	Kd *= 1.0 - Metallic;

	vec3 irradiance = texture(irradianceMap, N).rgb * pc.intensity;
	vec3 diffuse    = irradiance * Albedo;
	
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, Roughness * MAX_REFLECTION_LOD).rgb;
    vec2 BRDF  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), Roughness)).rg;
    vec3 specular = prefilteredColor * (F * BRDF.x + BRDF.y);

	vec3 ambient = (Kd * diffuse + specular) * AO;

    vec3 result = vec3(0);
	result = (1 - shadow) * L0 + ambient;

	vec3 hdrColor = result;

    // Reinhard 色调映射
    vec3 mapped = vec3(1.0) - exp(-hdrColor * pc.exposure);
    // gamma 矫正 
    mapped = pow(mapped, vec3(1.0 / pc.gamma));

    FragColor = vec4(mapped, 1.0);
}