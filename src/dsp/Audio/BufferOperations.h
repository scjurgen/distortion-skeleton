#pragma once
#include <array>
#include <vector>

namespace BOP
{
namespace Fill
{
    template <size_t BlockSize>
    void apply(float* buffer, const float gain) noexcept
    {
        std::fill_n(buffer, BlockSize, gain);
    }

    template <size_t BlockSize>
    void apply(std::array<float, BlockSize>& buffer, const float gain) noexcept
    {
        std::fill(buffer.begin(), buffer.end(), gain);
    }

    inline void apply(std::vector<float>& buffer, const float gain) noexcept
    {
        std::fill(buffer.begin(), buffer.end(), gain);
    }
}

namespace Gain
{
    template <size_t BlockSize>
    void apply(float* buffer, const float gain) noexcept
    {
        std::transform(buffer, buffer + BlockSize, buffer, [gain](const float v) { return v * gain; });
    }

    template <size_t BlockSize>
    void apply(const float* source, float* target, const float gain) noexcept
    {
        std::transform(source, source + BlockSize, target, [gain](const float v) { return v * gain; });
    }

    template <size_t BlockSize>
    void apply(std::array<float, BlockSize>& buffer, const float gain) noexcept
    {
        std::transform(buffer.begin(), buffer.end(), buffer.begin(), [gain](const float v) { return v * gain; });
    }

    template <size_t BlockSize>
    void apply(const std::array<float, BlockSize>& source, std::array<float, BlockSize>& target,
               const float gain) noexcept
    {
        std::transform(source.begin(), source.end(), target.begin(), [gain](const float v) { return v * gain; });
    }

    inline void apply(std::vector<float>& buffer, const float gain) noexcept
    {
        std::transform(buffer.begin(), buffer.end(), buffer.begin(), [gain](const float v) { return v * gain; });
    }

    inline void apply(const std::vector<float>& source, std::vector<float>& target, const float gain)
    {
        if (source.size() != target.size())
        {
            throw std::invalid_argument("source size does not match target size");
        }
        std::transform(source.begin(), source.end(), target.begin(), [gain](const float v) { return v * gain; });
    }
}

namespace Sum
{
    template <size_t BlockSize>
    void apply(std::array<float, BlockSize>& source, std::array<float, BlockSize>& target) noexcept
    {
        std::transform(source.begin(), source.end(), target.begin(), source.begin(),
                       [](const float lhs, const float rhs) { return lhs + rhs; });
    }

    inline void apply(std::vector<float>& source, std::vector<float>& target)
    {
        if (source.size() != target.size())
        {
            throw std::invalid_argument("source size does not match target size");
        }
        std::transform(source.begin(), source.end(), target.begin(), source.begin(),
                       [](const float lhs, const float rhs) { return lhs + rhs; });
    }
}
namespace Mul
{
    template <size_t BlockSize>
    void apply(std::array<float, BlockSize>& source, std::array<float, BlockSize>& target) noexcept
    {
        std::transform(source.begin(), source.end(), target.begin(), source.begin(),
                       [](const float lhs, const float rhs) { return lhs * rhs; });
    }

    inline void apply(std::vector<float>& source, std::vector<float>& target)
    {
        if (source.size() != target.size())
        {
            throw std::invalid_argument("source size does not match target size");
        }
        std::transform(source.begin(), source.end(), target.begin(), source.begin(),
                       [](const float lhs, const float rhs) { return lhs * rhs; });
    }
}

}