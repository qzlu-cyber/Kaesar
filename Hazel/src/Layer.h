#pragma once

#include "Hazel/Core.h"
#include "Events/Event.h"

namespace Hazel {

    class HAZEL_API Layer {
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {} // �����Ƶ� LayerStack ��ʱ����
		virtual void OnDetach() {} // �����Ƴ�ʱ���á������������ڴ��ڵ� Init() �� OnShundown()
		virtual void OnUpdate() {} // �������ʱ��ÿ֡��Ҫ����
		virtual void OnEvent(Event& event) {} // ÿ��һ���¼�λ���м��ʱ���ڴ˴�������

		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;

		bool isEnabled = true; // ��Ǹò��Ƿ񱻽��á�����Ȼ�������õĲ��޷�ִ�������ķ���
    };
}
