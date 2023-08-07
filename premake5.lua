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

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Kaesar/vendor/GLFW/include"
IncludeDir["Glad"] = "Kaesar/vendor/Glad/include"
IncludeDir["ImGui"] = "Kaesar/vendor/imgui"
IncludeDir["glm"] = "Kaesar/vendor/glm"
IncludeDir["stb_image"] = "Kaesar/vendor/stb_image"
IncludeDir["assimp"] = "Kaesar/vendor/assimp/include"
IncludeDir["entt"] = "Kaesar/vendor/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/Kaesar/vendor/yaml-cpp/include"

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
	staticruntime "on"

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
		"%{prj.name}/vendor/stb_image/**.cpp"
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
		"%{IncludeDir.yaml_cpp}"
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
			"assimp-vc143-mtd.lib"
		}

	filter "configurations:Release"
		defines "KR_RELEASE"
		runtime "Release"
		optimize "on"
		links
		{
			"assimp-vc143-mt.lib",
		}	

	filter "configurations:Dist"
		defines "KR_DIST"
		runtime "Release"
		optimize "on"

project "Kaesar-Editor"
	location "Kaesar-Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

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
		"%{IncludeDir.assimp}"
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
	staticruntime "on"

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
		"%{IncludeDir.entt}"
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