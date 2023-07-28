#pragma once

namespace Kaesar {
    /// <summary>
    /// 抽象的图形上下文接口，用来封装不同平台的图形上下文
    /// </summary>
    class GraphicsContext
    {
    public:
        virtual void Init() = 0; // 初始化图形上下文
        virtual void SwapBuffers() = 0; // 交换缓冲区
    };
}