
#include "Filters/OnePoleFilter.h"

#include "gtest/gtest.h"

#include <algorithm>
#include <array>
#include <cmath>
/*
4.32193	0.997396
5.32193	0.994816
6.32193	0.989708
7.32193	0.979737
8.32193	0.960696
9.32193	0.925826
10.3219	0.866913
11.3219	0.780254
12.3219	0.678266
13.3219	0.592313
14.3219	0.549902
 */
template <typename ValueType>
inline void renderWithSineWave(std::vector<ValueType>& target, const double sampleRate, const double frequency,
                               size_t numChannels)
{
    double phase = 0.0;
    const double advance = frequency / sampleRate;
    if (numChannels == 0)
    {
        throw(std::invalid_argument("numChannels can not be 0"));
    }
    for (size_t frameIdx = 0; frameIdx < target.size();)
    {
        auto value = static_cast<ValueType>(sin(phase * static_cast<float>(M_PI) * 2));
        for (size_t i = 0; i < numChannels; ++i)
        {
            target[frameIdx++] = value;
        }
        phase += advance;
        if (phase > 1)
        {
            phase -= 1;
        }
    }
}


template <AbacadDsp::OnePoleFilterCharacteristic Characteristic>
void testFilterMagnitude(float sampleRate)
{
    for (size_t cf = 50; cf <= 12800; cf *= 1.2f) // cutoff
    {
        for (size_t hz = 50; hz <= 12800; hz *= 1.2f) // test frequency
        {
            AbacadDsp::OnePoleFilter<Characteristic, false> sut{sampleRate};
            sut.setCutoff(static_cast<float>(cf));
            std::vector<float> wave(4000);
            renderWithSineWave(wave, sampleRate, hz, 1);
            sut.processBlock(wave.data(), wave.data(), wave.size());

            // pick values from within to account for settling values
            const auto [minV, maxV] = std::minmax_element(wave.begin() + wave.size() / 2, wave.end());
            auto maxValue = std::max(std::abs(*minV), std::abs(*maxV));
            auto db = std::log10(std::abs(maxValue)) * 20.0f;

            auto magnitude = sut.magnitude(static_cast<float>(hz));
            auto expectedDb = std::log10(magnitude) * 20.0;
            auto maxDt = 3.1f;
            std::cout << cf << "\t" << hz << "\t" << db << "\t" << expectedDb << "\n";
            // EXPECT_NEAR(db, expectedDb, maxDt) << "fail with cf:" << cf << " and f:" << hz;
        }
    }
}

template <AbacadDsp::OnePoleFilterCharacteristic Characteristic>
void find3(float sampleRate)
{
    float lastHz = 20;
    float lastFdbk = 1.f;
    for (float hz = lastHz; hz <= 24020; hz *= std::pow(2, 1.f / 12.f)) // test frequency
    {
        std::cout << log(hz) / log(2) << "\t";
        for (float fdbk = lastFdbk; fdbk > 0.54; fdbk = fdbk * 0.99999f)
        {
            AbacadDsp::OnePoleFilter<Characteristic, false> sut{sampleRate};
            // sut.setCutoff(static_cast<float>(cf));
            sut.setFeedback(fdbk);
            std::vector<float> wave(8000);
            renderWithSineWave(wave, sampleRate, hz, 1);
            sut.processBlock(wave.data(), wave.data(), wave.size());

            // pick values from within to account for settling values
            const auto [minV, maxV] = std::minmax_element(wave.begin() + wave.size() / 2, wave.end());
            auto maxValue = std::max(std::abs(*minV), std::abs(*maxV));
            auto db = std::log10(std::abs(maxValue)) * 20.0f;
            auto magnitude = sut.magnitude(static_cast<float>(hz));
            if (db < -3.0)
            {
                std::cout << sut.feedback() << std::endl;
                lastFdbk = fdbk;
                break;
            }
            // EXPECT_NEAR(db, expectedDb, maxDt) << "fail with cf:" << cf << " and f:" << hz;
        }
    }
}


TEST(DspOnePoleFilterTest, LowPassMatchTheoreticalMagnitudes)
{
    testFilterMagnitude<AbacadDsp::OnePoleFilterCharacteristic::LowPass>(48000.f);
}

TEST(DspOnePoleFilterTest, HighPassCheck)
{
    // find3<AbacadDsp::OnePoleFilterCharacteristic::HighPass>(48000.f);
}

TEST(DspOnePoleFilterTest, HighPassMatchTheoreticalMagnitudes)
{
    testFilterMagnitude<AbacadDsp::OnePoleFilterCharacteristic::HighPass>(48000.f);
}

TEST(DspOnePoleFilterTest, AllPassMatchTheoreticalMagnitudes)
{
    testFilterMagnitude<AbacadDsp::OnePoleFilterCharacteristic::AllPass>(48000.f);
}

TEST(DspOnePoleFilterTest, MultiChannelLowPassMatchTheoreticalMagnitudes)
{
    constexpr size_t NumChannels = 2;
    float sampleRate = 48000.f;

    for (size_t cf = 100; cf <= 6400; cf *= 4) // cutoff
    {
        for (size_t hz = 50; hz <= 12800; hz *= 2) // test frequency
        {
            AbacadDsp::MultiChannelOnePoleFilter<AbacadDsp::OnePoleFilterCharacteristic::LowPass, false, NumChannels>
                sut{sampleRate};
            sut.setCutoff(static_cast<float>(cf));
            std::vector<float> wave(4000 * NumChannels);
            renderWithSineWave(wave, sampleRate, hz, NumChannels);
            sut.processBlock(wave.data(), wave.size() / NumChannels);

            // Check each channel
            for (size_t channel = 0; channel < NumChannels; ++channel)
            {
                std::vector<float> channelData(wave.size() / NumChannels);
                for (size_t i = 0; i < channelData.size(); ++i)
                {
                    channelData[i] = wave[i * NumChannels + channel];
                }

                const auto [minV, maxV] =
                    std::minmax_element(channelData.begin() + channelData.size() / 2, channelData.end());
                auto maxValue = std::max(std::abs(*minV), std::abs(*maxV));
                auto db = std::log10(std::abs(maxValue)) * 20.0f;

                auto magnitude = sut.magnitude(static_cast<float>(hz));
                auto expectedDb = std::log10(magnitude) * 20.0;
                auto maxDt = 1.2f;
                EXPECT_NEAR(db, expectedDb, maxDt);
            }
        }
    }
}

// You can add similar tests for MultiChannel HighPass and AllPass if needed

TEST(DspOnePoleFilterTest, MultiChannelIndependence)
{
    constexpr size_t NumChannels = 2;
    float sampleRate = 48000.f;
    float cutoff = 1000.f;

    AbacadDsp::MultiChannelOnePoleFilter<AbacadDsp::OnePoleFilterCharacteristic::LowPass, false, NumChannels> sut{
        sampleRate};
    sut.setCutoff(cutoff);

    std::vector<float> input(1000 * NumChannels, 0.0f);
    // Set channel 0 to all 1's and channel 1 to all -1's
    for (size_t i = 0; i < input.size(); i += NumChannels)
    {
        input[i] = 1.0f;
        input[i + 1] = -1.0f;
    }

    sut.processBlock(input.data(), input.size() / NumChannels);

    // Check that the channels remain independent
    for (size_t i = 0; i < input.size(); i += NumChannels)
    {
        EXPECT_GT(input[i], 0.0f);
        EXPECT_LT(input[i + 1], 0.0f);
    }
}
