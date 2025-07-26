#pragma once

#include "Audio/AudioBuffer.h"
#include <type_traits>

// CRTP and SFINAE
template <typename Derived, size_t Channels, size_t FixedFrameSize>
class AudioProcessor
{
  public:
    using SampleType = float;
    using Buffer = AudioBuffer<Channels, FixedFrameSize>;

    explicit AudioProcessor(const float sampleRate = 48000.0f)
        : m_sampleRate(sampleRate)
    {
        static_assert(Channels >= 1u);
    }

    virtual ~AudioProcessor() = default;

    template <typename D = Derived>
    auto process(const Buffer& input, Buffer& output) noexcept -> decltype(std::declval<D>().process(input, output))
    {
        return static_cast<Derived*>(this)->process(input, output);
    }

    template <typename D = Derived>
    auto processInplace(Buffer& inplace) noexcept -> decltype(std::declval<D>().processInplace(inplace))
    {
        return static_cast<Derived*>(this)->processInplace(inplace);
    }

    virtual void setParameter(int paramIndex, float value) noexcept = 0;
    [[nodiscard]] virtual float getParameter(int paramIndex) const noexcept = 0;

    void prepare(const float sampleRate) noexcept
    {
        m_sampleRate = sampleRate;
    }

    [[nodiscard]] float sampleRate() const noexcept
    {
        return m_sampleRate;
    }

    static constexpr size_t fixedFrameSize() noexcept
    {
        return FixedFrameSize;
    }

    static constexpr size_t numChannels() noexcept
    {
        return Channels;
    }

  protected:
    AudioProcessor(const AudioProcessor&) = default;
    AudioProcessor& operator=(const AudioProcessor&) = default;

  private:
    float m_sampleRate;
};

template <typename Derived, size_t FixedFrameSize>
using StereoProcessor = AudioProcessor<Derived, 2, FixedFrameSize>;

template <typename Derived, size_t FixedFrameSize>
using MonoProcessor = AudioProcessor<Derived, 1, FixedFrameSize>;