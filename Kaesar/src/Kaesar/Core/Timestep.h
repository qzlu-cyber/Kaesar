#pragma once

namespace Kaesar {
    class Timestep 
    {
    public:
        Timestep(float time = 0.0f)
            : m_Time(time) {}

        inline float GetSeconds() const { return m_Time; }
        inline float GetMilliseconds() const { return m_Time * 1000.0f; }

        operator float() const { return m_Time; } // 重载 float 类型的转换操作符，使得 Timestep 类型可以直接转换为 float 类型

    private:
        float m_Time;
    };
}