#pragma once

#include "Core.h"
#include "Layer.h"

namespace Kaesar {
    /// <summary>
    /// ���ڻ��кܶ�� Layer������Ҫ���һ�����ݽṹ�洢��Щ Layer��������һ�� vector ģ����һ�� Stack����Ϊ������
    /// vector[0] ��Ϊջ����ջ���� Layer ����Ļ������������� Layer���� Layer ���Ƚ����¼��������Ⱦ
    /// </summary>
    class Kaesar_API LayerStack {
    public:
        LayerStack();
        ~LayerStack();

        // layer ������ͨ�� layer, Overlay ��������Ļ��Զ�� layer
        // ���ǲ� overlay �����Ⱦ�����Ƚ��յ��¼�
        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);
        void PopLayer(Layer* layer);
        void PopOverlay(Layer* overlay);

        std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
        std::vector<Layer*>::iterator end() { return m_Layers.end(); }
    private:
        std::vector<Layer*> m_Layers; // �������� layer ��ջ
        unsigned int m_LayerInsertIndex = 0; // ��ǰ��ջ��
    };
}
