#pragma once

#include "Layer.h"

#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"

namespace Hazel {

	class HAZEL_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& event);

	private:
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& event); // 处理鼠标按下事件
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& event); // 处理鼠标松开事件
		bool OnMouseMovedEvent(MouseMovedEvent& event); // 处理鼠标移动事件
		bool OnMouseScrolledEvent(MouseScrolledEvent& event); // 处理鼠标滚轮事件
		bool OnKeyPressedEvent(KeyPressedEvent& event); // 处理键盘按下事件
		bool OnKeyReleasedEvent(KeyReleasedEvent& event); // 处理键盘松开事件
		bool OnKeyTypedEvent(KeyTypedEvent& event); // 处理键盘输入事件
		bool OnWindowResizeEvent(WindowResizeEvent& event); // 处理窗口大小改变事件

	private:
		float m_Time = 0.0f;
	};
}

