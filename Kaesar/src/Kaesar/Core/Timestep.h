#pragma once

namespace Kaesar {
    class Timestep 
    {
    public:
        Timestep(float time = 0.0f)
            : m_Time(time) {}

        inline float GetSeconds() const { return m_Time; }
        inline float GetMilliseconds() const { return m_Time * 1000.0f; }

        operator float() const { return m_Time; } // ���� float ���͵�ת����������ʹ�� Timestep ���Ϳ���ֱ��ת��Ϊ float ����

    private:
        float m_Time;
    };
}