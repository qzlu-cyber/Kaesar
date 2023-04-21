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

    void LayerStack::PushLayer(Layer* layer) {
        m_LayerInsert = m_Layers.emplace(m_LayerInsert, layer); // 普通层被推到前半部分
    }

    void LayerStack::PushOverlay(Layer* overlay) {
        m_Layers.emplace_back(overlay); // 覆盖层被推到后半部分
    }

    void LayerStack::PopLayer(Layer* layer) {
        auto it = std::find(begin(), end(), layer);
        if (it != end()) {
            m_Layers.erase(it);
            m_LayerInsert--;
        }
    }

    void LayerStack::PopOverlay(Layer* overlay)
    {
        auto it = std::find(begin(), end(), overlay);
        if (it != end())
            m_Layers.erase(it);
    }
}
