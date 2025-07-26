#pragma once

#include <array>
#include <algorithm>
#include <cmath>
#include <type_traits>

#include "Numbers/MathApproximations.h"


// tag
struct SkipSmoothing_t
{
};
constexpr auto skipSmoothing = SkipSmoothing_t{};


namespace AbacadDsp
{

inline void blockScale(const float gain, float* inPlace, const size_t n_samples)
{
    for (size_t index = 0; index < n_samples; ++index)
    {
        inPlace[index] *= gain;
    }
}

inline void blockSum(const float* in, float* target, const size_t n_samples)
{
    for (size_t index = 0; index < n_samples; ++index)
    {
        target[index] += in[index];
    }
}

inline void blockMult(const float* in, float* target, const size_t n_samples)
{
    for (size_t index = 0; index < n_samples; ++index)
    {
        target[index] *= in[index];
    }
}

inline void blockSum(const float* in1, const float* in2, float* target, const size_t n_samples)
{
    for (size_t index = 0; index < n_samples; ++index)
    {
        target[index] = in1[index] + in2[index];
    }
}

inline void blockSumStereo(const float* inLeft, const float* inRight, float* targetLeft, float* targetRight,
                           const size_t n_samples)
{
    std::transform(inLeft, inLeft + n_samples, targetLeft, targetLeft, [](float lhs, float rhs) { return lhs + rhs; });
    std::transform(inRight, inRight + n_samples, targetRight, targetRight,
                   [](float lhs, float rhs) { return lhs + rhs; });
}

inline void blockScaledSumStereo(const float gain, const float* inLeft, const float* inRight, float* targetLeft,
                                 float* targetRight, const size_t n_samples)
{
    for (size_t i = 0; i < n_samples; ++i)
    {
        targetLeft[i] += gain * inLeft[i];
        targetRight[i] += gain * inRight[i];
    }
}


inline void blockSub(const float* in, float* target, const size_t n_samples)
{
    for (size_t index = 0; index < n_samples; ++index)
    {
        target[index] -= in[index];
    }
}

inline void blockSubStereo(const float* inLeft, const float* inRight, float* targetLeft, float* targetRight,
                           const size_t n_samples)
{
    std::transform(inLeft, inLeft + n_samples, inLeft, targetLeft, [](float lhs, float rhs) { return lhs - rhs; });
    std::transform(inRight, inRight + n_samples, inRight, targetRight, [](float lhs, float rhs) { return lhs - rhs; });
}

inline void blockClip(float* in, const size_t n_samples)
{
    for (size_t index = 0; index < n_samples; ++index)
    {
        in[index] = std::clamp(in[index], -0.99999f, 0.99999f);
    }
}

inline void blockScaledSum(const float gain, const float* in, float* target, const size_t n_samples)
{
    for (size_t index = 0; index < n_samples; ++index)
    {
        target[index] += in[index] * gain;
    }
}

inline void blockScaledSum(const float* in, float* target, const float gain, const size_t n_samples)
{
    for (size_t index = 0; index < n_samples; ++index)
    {
        target[index] += in[index] * gain;
    }
}

inline void blockScaleCopy(const float gain, const float* in, float* target, const size_t n_samples)
{
    for (size_t index = 0; index < n_samples; ++index)
    {
        target[index] = gain * in[index];
    }
}

// pan between outs (stereo spread)
// result * wet  +  dry * in
template <size_t BlockSize>
void crossMixDryWet(const float* leftIn, const float* rightIn, float* leftOut, float* rightOut, float outLeftGain,
                    float outRightGain, float wetGain, float dryGain)
{
    for (size_t index = 0; index < BlockSize; ++index)
    {
        auto leftCrossMix = leftOut[index] * outLeftGain + rightOut[index] * outRightGain;
        auto rightCrossMix = rightOut[index] * outLeftGain + leftOut[index] * outRightGain;
        leftOut[index] = leftCrossMix * wetGain + leftIn[index] * dryGain;
        rightOut[index] = rightCrossMix * wetGain + rightIn[index] * dryGain;
    }
}

inline void blockClamp(const float maxValue, float* inPlace, const size_t n_samples)
{
    for (size_t index = 0; index < n_samples; ++index)
    {
        inPlace[index] = std::clamp(inPlace[index], -maxValue, maxValue);
    }
}

inline void blockInvert(float* inPlace, size_t n_samples)
{
    for (size_t index = 0; index < n_samples; ++index)
    {
        inPlace[index] = -inPlace[index];
    }
}

inline size_t getSamplesPerMillisecond(float milliseconds, float sampleRate, size_t maxValue)
{
    auto size = static_cast<size_t>(std::roundf(milliseconds / 1000.f * sampleRate));
    return std::clamp<size_t>(size, 1, maxValue);
}

template <typename T>
void getPanFactor(const T angleInPercent, T& left, T& right)
{
    const T f = static_cast<T>(std::sqrt(2.f) / 2.0f);
    T angle = angleInPercent * 0.007853981634f; // 1/4 PI/100
    T cosVal = std::cos(angle);
    T sinVal = std::sin(angle);
    left = f * (cosVal - sinVal);
    right = f * (cosVal + sinVal);
}

template <typename T>
void getPanFactorNormalized(const T angleNormalized, T& left, T& right)
{
    const T f = static_cast<T>(std::sqrt(2.f) / 2.0f);
    T angle = angleNormalized * 0.7853981634f;
    T cosVal = std::cos(angle);
    T sinVal = std::sin(angle);
    left = f * (cosVal - sinVal);
    right = f * (cosVal + sinVal);
}

// fast but good enough for a live pedal
template <typename T>
void getFastPanFactorNormalized(const T f, T& left, T& right)
{
    const auto a = (f + 1) / 2;
    T cosVal = quarterSineForFade(1 - a);
    T sinVal = quarterSineForFade(a);
    left = cosVal;
    right = sinVal;
}

template <typename T>
[[nodiscard]] T sigmoidSqrt(T x)
{
    return x / std::sqrt(1.f + x * x);
}

template <typename T>
[[nodiscard]] static T dbToGain(T dB)
{
    return pow(10.0f, dB / 20.0f);
}

template <typename T>
[[nodiscard]] static T gainToDB(T gain)
{
    if (gain <= 0.0f)
    {
        if (std::numeric_limits<T>::is_iec559)
        {
            return -std::numeric_limits<T>::infinity();
        }
        else // 23 bit last bit set.
        {
            return log10(1.0f / static_cast<T>(1 << 23)) * 20.0f;
        }
    }
    return log10f(gain) * 20.0f;
}
}