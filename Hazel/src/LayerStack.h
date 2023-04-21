#pragma once

#include "Hazel/Core.h"
#include "Layer.h"

namespace Hazel {
    /// <summary>
    /// ���ڻ��кܶ�� Layer������Ҫ���һ�����ݽṹ�洢��Щ Layer��������һ�� vector ģ����һ�� Stack����Ϊ������
	/// vector[0] ��Ϊջ����ջ���� Layer ����Ļ������������� Layer���� Layer ���Ƚ����¼��������Ⱦ
    /// </summary>
    class HAZEL_API LayerStack {
	public:
		LayerStack();
		~LayerStack();

		// layer ������ͨ�� layer, Overlay ��������Ļ��Զ�� layer
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }
	private:
		std::vector<Layer*> m_Layers; // �������� layer ��ջ
		std::vector<Layer*>::iterator m_LayerInsert; // ��ǰ��ջ��
    };
}
