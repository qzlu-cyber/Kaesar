#pragma once

namespace Kaesar {
    /// <summary>
    /// �����ͼ�������Ľӿڣ�������װ��ͬƽ̨��ͼ��������
    /// </summary>
    class GraphicsContext
    {
    public:
        virtual void Init() = 0; // ��ʼ��ͼ��������
        virtual void SwapBuffers() = 0; // ����������
    };
}