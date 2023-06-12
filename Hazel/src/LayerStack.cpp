#include "hzpch.h"
#include "LayerStack.h"

namespace Hazel {

    LayerStack::LayerStack() 
    {

    }

    LayerStack::~LayerStack() 
    {
        for (Layer* layer : m_Layers)
            delete layer;
    }

    /// <summary>
    /// 将给定的 layer 添加到 m_Layers 数组的特定位置，这个位置是由 m_LayerInsertIndex 表示的。
    /// m_LayerInsertIndex 之前的是正常的层，之后的是覆盖层（Overlay）
    /// </summary>
    void LayerStack::PushLayer(Layer* layer) 
    {
        m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer); // 普通层被推到前半部分
        m_LayerInsertIndex++;
    }

    /// <summary>
    /// 将给定的 overlay 添加到 m_Layers 开始的位置，作为覆盖层之后的下一个层被添加。
    /// </summary>
    void LayerStack::PushOverlay(Layer* overlay) 
    {
        m_Layers.emplace_back(overlay); // 覆盖层被推到后半部分
    }

    /// <summary>
    /// 移除在 m_Layers 数组中找到的层，并将 m_LayerInsertIndex 移动到正常层的末尾（即可能被弹出的层的前一个层）。
    /// </summary>
    void LayerStack::PopLayer(Layer* layer) 
    {
        auto it = std::find(begin(), end(), layer);
        if (it != end()) {
            m_Layers.erase(it);
            m_LayerInsertIndex--;
        }
    }

    /// <summary>
    /// 移除在 m_Layers 数组中找到的覆盖层。
    /// </summary>
    void LayerStack::PopOverlay(Layer* overlay)
    {
        auto it = std::find(begin(), end(), overlay);
        if (it != end())
            m_Layers.erase(it);
    }
}
