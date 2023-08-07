#include "krpch.h"

#include "Kaesar/Utils/PlatformUtils.h"
#include "Kaesar/Core/Application.h"

#include <commdlg.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace Kaesar {
    /// <summary>
    /// ��ʾһ�������ļ��Ի���
    /// </summary>
    /// <param name="filter">
	///	���ļ��Ի����У��ļ���������filter ������ͨ����һ���ַ���������ָ���ļ����͵�ɸѡ����������һϵ���ļ����ͺ���Ӧ���ļ���չ����ɣ��÷ֺŽ��зָ���
	///	���磬�����Ĺ�������ʽ������ "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0"��
	/// </param>
    /// <returns></returns>
    std::optional<std::string> FileDialogs::OpenFile(const char* filter)
    {
		OPENFILENAMEA ofn; // �ļ��Ի���ṹ��
		CHAR szFile[260] = { 0 }; 
		CHAR currentDir[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME); // ���ýṹ���С
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow()); // ���ø����ھ��
		ofn.lpstrFile = szFile; // �����ļ�·��������
		ofn.nMaxFile = sizeof(szFile);
		if (GetCurrentDirectoryA(256, currentDir)) // ��ȡ��ǰĿ¼
			ofn.lpstrInitialDir = currentDir;
		ofn.lpstrFilter = filter; // �����ļ����͹�����
		ofn.nFilterIndex = 1; // ����Ĭ�Ϲ�����
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE) // ��ʾ�ļ��Ի��򣬲��ȴ��û�����ѡ��
			return ofn.lpstrFile; // ����û�ѡ�����ļ��������ȷ����ť�������������ļ���·��
		return std::nullopt; // ����û�ȡ����ѡ������˴���
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

		/// strchr(filter, '\0') ���ʽ�����ҵ���һ�����ַ���\0����λ�ã����־��һ���ļ����͵Ľ�����strchr �������ַ����в���ָ���ַ���һ�γ��ֵ�λ�á�
		/// +1 ��ʾ��ȡ�ÿ��ַ�֮����ַ�������һ���ļ����͵���չ���������չ����������Ĭ����չ��
		/// ���磬��� filter ��ֵΪ "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0"����ô strchr(filter, '\0') + 1 ������ *.txt
		ofn.lpstrDefExt = strchr(filter, '\0') + 1; // �����ļ��Ի����Ĭ����չ

		if (GetSaveFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;
		return std::nullopt;
	}
}