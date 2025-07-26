
#pragma once

#include <cmath>
#include <functional>


class ParameterSmoother
{
  public:
    ParameterSmoother(const float smoothingTimeInMs, const float samplingRate)
    {
        constexpr float c_minusTwoPi = -6.28318530717958f;
        a = exp(c_minusTwoPi / (smoothingTimeInMs * 0.001f * samplingRate));
        b = 1.0f - a;
        z = 0.0f;
    }

    [[nodiscard]] float process(const float in) const
    {
        return in * b + z * a;
    }

  private:
    float a{};
    float b{};
    float z{};
};


// update time should be 3kHz
// linear smoothing
template <typename T_>
class Smoother
{
  public:
    Smoother()
        : m_useCallback(false)
    {
    }

    Smoother(const T_ defaultValue, const size_t granularity, const float stepPerTick)
        : m_targetValue(defaultValue)
        , m_currentValue(defaultValue)
        , m_granularity(granularity)
        , m_stepPerTick(stepPerTick)
        , m_useCallback(false)
    {
    }

    using ParameterSetCallback = std::function<void(const T_ value)>;

    void setCallback(ParameterSetCallback callback)
    {
        m_useCallback = true;
        m_setCallback = callback;
        m_setCallback(m_currentValue);
    }

    void passValue(const float value)
    {
        if (value != m_targetValue)
        {
            m_targetValue = value;
        }
    }

    [[maybe_unused]] bool handleTickHasNewValue()
    {
        if (++m_granularityCount >= m_granularity)
        {
            m_granularityCount = 0;
            if (m_currentValue != m_targetValue)
            {
                if (m_currentValue < m_targetValue)
                {
                    m_currentValue += m_stepPerTick;
                    if (m_currentValue >= m_targetValue)
                    {
                        m_currentValue = m_targetValue;
                    }
                }
                else
                {
                    m_currentValue -= m_stepPerTick;
                    if (m_currentValue <= m_targetValue)
                    {
                        m_currentValue = m_targetValue;
                    }
                }
                if (m_useCallback)
                {
                    if (m_setCallback)
                    {
                        m_setCallback(m_currentValue);
                    }
                }
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] float getCurrentValue() const
    {
        return m_currentValue;
    }

    void preset(const T_ value, const size_t granularity, const float stepPerTick)
    {
        m_useCallback = false;
        m_targetValue = value;
        m_currentValue = value * 0.9999f;
        m_granularity = granularity;
        m_stepPerTick = stepPerTick;
    }

  private:
    size_t m_granularity{1};
    size_t m_granularityCount{0};
    float m_targetValue{1.0};
    float m_currentValue{1.0};
    float m_stepPerTick{0.01};
    bool m_useCallback{false};
    ParameterSetCallback m_setCallback{nullptr};
};
