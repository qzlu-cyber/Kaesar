#include "hzpch.h"
#include "LayerStack.h"

namespace Hazel {

    LayerStack::LayerStack() {
        m_LayerInsert = begin();
    }

    LayerStack::~LayerStack() {
        for (Layer* layer : m_Layers)
            delete layer;
    }

    /// <summary>
    /// �������� layer ��ӵ� m_Layers ������ض�λ�ã����λ������ m_LayerInsert ��ʾ�ġ�
    /// m_LayerInsert ֮ǰ���������Ĳ㣬֮����Ǹ��ǲ㣨Overlay��
    /// </summary>
    void LayerStack::PushLayer(Layer* layer) {
        m_LayerInsert = m_Layers.emplace(m_LayerInsert, layer); // ��ͨ�㱻�Ƶ�ǰ�벿��
    }

    /// <summary>
    /// �������� overlay ��ӵ� m_Layers ��ʼ��λ�ã���Ϊ���ǲ�֮�����һ���㱻��ӡ�
    /// </summary>
    void LayerStack::PushOverlay(Layer* overlay) {
        m_Layers.emplace_back(overlay); // ���ǲ㱻�Ƶ���벿��
    }

    /// <summary>
    /// �Ƴ��� m_Layers �������ҵ��Ĳ㣬���� m_LayerInsert �ƶ����������ĩβ�������ܱ������Ĳ��ǰһ���㣩��
    /// </summary>
    void LayerStack::PopLayer(Layer* layer) {
        auto it = std::find(begin(), end(), layer);
        if (it != end()) {
            m_Layers.erase(it);
            m_LayerInsert--;
        }
    }

    /// <summary>
    /// �Ƴ��� m_Layers �������ҵ��ĸ��ǲ㡣
    /// </summary>
    void LayerStack::PopOverlay(Layer* overlay)
    {
        auto it = std::find(begin(), end(), overlay);
        if (it != end())
            m_Layers.erase(it);
    }
}
