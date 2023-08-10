workspace "Kaesar"
	architecture "x64"
	startproject "Kaesar-Editor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

VULKAN_SDK = os.getenv("VULKAN_SDK")

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Kaesar/vendor/GLFW/include"
IncludeDir["Glad"] = "Kaesar/vendor/Glad/include"
IncludeDir["ImGui"] = "Kaesar/vendor/imgui"
IncludeDir["glm"] = "Kaesar/vendor/glm"
IncludeDir["stb_image"] = "Kaesar/vendor/stb_image"
IncludeDir["assimp"] = "Kaesar/vendor/assimp/include"
IncludeDir["entt"] = "Kaesar/vendor/entt/include"
IncludeDir["yaml_cpp"] = "Kaesar/vendor/yaml-cpp/include"
IncludeDir["ImGuizmo"] = "Kaesar/vendor/ImGuizmo"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDir["shaderc"] = "%{wks.location}/Kaesar/vendor/shaderc"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/Kaesar/vendor/SPIRV-Cross"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanSDK_Debug"] = "%{wks.location}/Kaesar/vendor/VulkanSDK/Lib"
LibraryDir["VulkanSDK_DLL"] = "%{wks.location}/Kaesar/vendor/VulkanSDK/Bin"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

group "Dependencies"
	include "Kaesar/vendor/GLFW"
	include "Kaesar/vendor/Glad"
	include "Kaesar/vendor/imgui"
	include "Kaesar/vendor/yaml-cpp"
group ""

project "Kaesar"
	location "Kaesar"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "krpch.h"
	pchsource "Kaesar/src/krpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.h",
		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.VulkanSDK}"
	}

	libdirs {
		"Kaesar/vendor/assimp/build/Debug",
		"Kaesar/vendor/assimp/build/Release"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"yaml-cpp",
		"opengl32.lib"
	}

	filter "files:Kaesar/vendor/ImGuizmo/**.cpp"
	flags { "NoPCH" }

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"KR_PLATFORM_WINDOWS",
			"KR_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "KR_DEBUG"
		runtime "Debug"
		symbols "on"
		links
		{
			"assimp-vc143-mtd.lib",
			"%{Library.ShaderC_Debug}",
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_GLSL_Debug}"
		}

	filter "configurations:Release"
		defines "KR_RELEASE"
		runtime "Release"
		optimize "on"
		links
		{
			"assimp-vc143-mt.lib",
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}	

	filter "configurations:Dist"
		defines "KR_DIST"
		runtime "Release"
		optimize "on"
		links
		{
			"assimp-vc143-mt.lib",
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}	

project "Kaesar-Editor"
	location "Kaesar-Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/Kaesar/vendor/spdlog/include",
		"%{wks.location}/Kaesar/src",
		"%{wks.location}/Kaesar/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.Imgui}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.ImGuizmo}"
	}

	links
	{
		"Kaesar"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"KR_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "KR_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "KR_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "KR_DIST"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Kaesar/vendor/spdlog/include",
		"Kaesar/vendor/GLFW/include",
		"Kaesar/src",
		"Kaesar/vendor",
		"%{IncludeDir.Imgui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.ImGuizmo}"
	}

	links
	{
		"Kaesar"
	}

	filter "system:windows"
		systemversion "latest"
		defines
		{
			"KR_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "KR_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "KR_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "KR_DIST"
		runtime "Release"
		optimize "on"