#pragma once

#include "Core.h"
#include "Layer.h"

namespace Kaesar {
    /// <summary>
    /// 由于会有很多个 Layer，所以要设计一个数据结构存储这些 Layer，这里用一个 vector 模拟了一个 Stack，作为容器，
    /// vector[0] 作为栈顶，栈顶的 Layer 是屏幕上离我们最近的 Layer，该 Layer 最先接受事件，最后被渲染
    /// </summary>
    class Kaesar_API LayerStack {
    public:
        LayerStack();
        ~LayerStack();

        // layer 代表普通的 layer, Overlay 代表离屏幕最远的 layer
        // 覆盖层 overlay 最后被渲染，最先接收到事件
        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);
        void PopLayer(Layer* layer);
        void PopOverlay(Layer* overlay);

        std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
        std::vector<Layer*>::iterator end() { return m_Layers.end(); }
    private:
        std::vector<Layer*> m_Layers; // 保存所有 layer 的栈
        unsigned int m_LayerInsertIndex = 0; // 当前的栈针
    };
}
