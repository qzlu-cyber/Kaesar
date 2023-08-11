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

layout(binding = 2) uniform Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
} light;

void main()
{	
	// ambient
	vec3 ambient = light.ambient * texture(texture_diffuse, v_TexCroods).rgb;

	// diffuse
	vec3 normal = normalize(v_Normal);
	vec3 lightDir = normalize(light.position - v_FragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(texture_diffuse, v_TexCroods).rgb;

	// specular
	vec3 viewDir = normalize(camera.u_CameraPos - v_FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	// Phone 光照模型
	//vec3 reflectDir = reflect(-lightDir, normal);
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), 150);
	// Blinn-Phone 光照模型
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 150);
	vec3 specular = light.specular * spec * texture(texture_specular, v_TexCroods).rgb;

	vec3 color = ambient + diffuse + specular;

	FragColor = vec4(color, 1.0);
}