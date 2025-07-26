#pragma once

#include "PedalBase.h"
#include "Audio/AudioBuffer.h"

#include "Filters/OnePoleFilter.h"

template <size_t BlockSize>
class DistortionPedal final : public PedalBase
{
public:
    DistortionPedal(const float sampleRate)
        : PedalBase(sampleRate)
    {
    }

    void setType(const size_t /* type */)
    {
    }

    void setPreboostLow(const float /* value */)
    {
    }

    void setPreboostHigh(const float /* value */)
    {
    }

    void setCut(const float /* value */)
    {
    }

    void setLevel(const float value)
    {
        m_gain = std::pow(10.f, value / 20.f);
    }

    void setCrossOver(const float /* value */)
    {
    }

    const std::vector<float>& visualizeWaveData()
    {
        m_preparedWavedata.resize(m_visualWavedata.size());
        m_preparedWavedata = m_visualWavedata;
        return m_preparedWavedata;
    }

    void processBlock(const AudioBuffer<2, BlockSize>& in, AudioBuffer<2, BlockSize>& out)
    {
        for (size_t i = 0; i < BlockSize; ++i)
        {
            out(i, 0) = in(i, 0) * m_gain;
            out(i, 1) = in(i, 1) * m_gain;
        }
    }

private:
    float m_gain{1.f};
    std::vector<float> m_visualWavedata;
    std::vector<float> m_preparedWavedata;
};