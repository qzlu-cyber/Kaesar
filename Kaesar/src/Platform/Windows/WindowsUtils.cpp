#include "krpch.h"

#include "Kaesar/Utils/PlatformUtils.h"
#include "Kaesar/Core/Application.h"

#include <commdlg.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace Kaesar {
    /// <summary>
    /// 显示一个加载文件对话框
    /// </summary>
    /// <param name="filter">
	///	在文件对话框中，文件过滤器（filter 参数）通常是一个字符串，用于指定文件类型的筛选。过滤器由一系列文件类型和相应的文件扩展名组成，用分号进行分隔。
	///	例如，常见的过滤器形式可能是 "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0"。
	/// </param>
    /// <returns></returns>
    std::optional<std::string> FileDialogs::OpenFile(const char* filter)
    {
		OPENFILENAMEA ofn; // 文件对话框结构体
		CHAR szFile[260] = { 0 }; 
		CHAR currentDir[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME); // 设置结构体大小
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow()); // 设置父窗口句柄
		ofn.lpstrFile = szFile; // 设置文件路径缓冲区
		ofn.nMaxFile = sizeof(szFile);
		if (GetCurrentDirectoryA(256, currentDir)) // 获取当前目录
			ofn.lpstrInitialDir = currentDir;
		ofn.lpstrFilter = filter; // 设置文件类型过滤器
		ofn.nFilterIndex = 1; // 设置默认过滤器
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE) // 显示文件对话框，并等待用户进行选择
			return ofn.lpstrFile; // 如果用户选择了文件并点击了确定按钮，函数将返回文件的路径
		return std::nullopt; // 如果用户取消了选择或发生了错误
    }

	std::optional<std::string> FileDialogs::SaveFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		CHAR currentDir[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		if (GetCurrentDirectoryA(256, currentDir))
			ofn.lpstrInitialDir = currentDir;
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

		/// strchr(filter, '\0') 表达式用于找到第一个空字符（\0）的位置，这标志着一个文件类型的结束。strchr 函数在字符串中查找指定字符第一次出现的位置。
		/// +1 表示获取该空字符之后的字符，即下一个文件类型的扩展名。这个扩展名将被用作默认扩展。
		/// 例如，如果 filter 的值为 "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0"，那么 strchr(filter, '\0') + 1 将返回 *.txt
		ofn.lpstrDefExt = strchr(filter, '\0') + 1; // 设置文件对话框的默认扩展

		if (GetSaveFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;
		return std::nullopt;
	}
}