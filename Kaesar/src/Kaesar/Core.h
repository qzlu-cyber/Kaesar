#pragma once

#ifdef KR_PLATFORM_WINDOWS
#if KR_DYNAMIC_LINK
#ifdef KR_BUILD_DLL
#define Kaesar_API __declspec(dllexport)
#else
#define Kaesar_API __declspec(dllimport)
#endif // KR_BUILD_DLL
#else
#define Kaesar_API
#endif
#else
#error Kaesar only support Windows
#endif // KR_PLATFORM_WINDOWS

#ifdef KR_DEBUG
#define KR_ENABLE_ASSERTS
#endif // KR_DEBUG

#ifdef KR_ENABLE_ASSERTS
#define KR_ASSERT(x, ...) { if(!(x)) { KR_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define KR_CORE_ASSERT(x, ...) { if(!(x)) { KR_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define KR_ASSERT(x, ...)
#define KR_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)
#define KR_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)