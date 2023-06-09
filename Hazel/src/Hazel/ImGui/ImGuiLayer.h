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
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& event); // ������갴���¼�
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& event); // ��������ɿ��¼�
		bool OnMouseMovedEvent(MouseMovedEvent& event); // ��������ƶ��¼�
		bool OnMouseScrolledEvent(MouseScrolledEvent& event); // �����������¼�
		bool OnKeyPressedEvent(KeyPressedEvent& event); // ������̰����¼�
		bool OnKeyReleasedEvent(KeyReleasedEvent& event); // ��������ɿ��¼�
		bool OnKeyTypedEvent(KeyTypedEvent& event); // ������������¼�
		bool OnWindowResizeEvent(WindowResizeEvent& event); // �����ڴ�С�ı��¼�

	private:
		float m_Time = 0.0f;
	};
}

