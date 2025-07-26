#include <gtest/gtest.h>
#include "Numbers/ValueParser.h"
#include "SynthScript/SynthFunctions.h"

#include <cmath>
#include <stdexcept>

TEST(ValueParserTest, parseWaveType)
{
    EXPECT_EQ(ParseNumbers::parseWaveType("sine"), GeneratorFactory::GeneratorType::Sine);
    EXPECT_EQ(ParseNumbers::parseWaveType("square"), GeneratorFactory::GeneratorType::Square);
    EXPECT_EQ(ParseNumbers::parseWaveType("triangle"), GeneratorFactory::GeneratorType::Triangle);
    EXPECT_EQ(ParseNumbers::parseWaveType("saw"), GeneratorFactory::GeneratorType::Saw);
    EXPECT_EQ(ParseNumbers::parseWaveType("click"), GeneratorFactory::GeneratorType::DiracRandom);
    EXPECT_EQ(ParseNumbers::parseWaveType("dirac"), GeneratorFactory::GeneratorType::DiracRandom);
    EXPECT_EQ(ParseNumbers::parseWaveType("silence"), GeneratorFactory::GeneratorType::Silence);
    EXPECT_EQ(ParseNumbers::parseWaveType("white"), GeneratorFactory::GeneratorType::WhiteNoise);
    EXPECT_EQ(ParseNumbers::parseWaveType("noise"), GeneratorFactory::GeneratorType::WhiteNoise);
    EXPECT_EQ(ParseNumbers::parseWaveType("brown"), GeneratorFactory::GeneratorType::BrownNoise);
    EXPECT_EQ(ParseNumbers::parseWaveType("pink"), GeneratorFactory::GeneratorType::PinkNoise);
    EXPECT_EQ(ParseNumbers::parseWaveType("unknown"), GeneratorFactory::GeneratorType::Sine);
}

TEST(ValueParserTest, parseString)
{
    EXPECT_EQ(ParseNumbers::parseString("'hello'"), "hello");
    EXPECT_EQ(ParseNumbers::parseString("'12345'"), "12345");
    EXPECT_THROW(ParseNumbers::parseString("no_quotes"), std::runtime_error);
    EXPECT_THROW(ParseNumbers::parseString("'unmatched_start"), std::runtime_error);
    EXPECT_THROW(ParseNumbers::parseString("unmatched_end'"), std::runtime_error);
}

TEST(ValueParserTest, parseFrequency)
{
    EXPECT_FLOAT_EQ(ParseNumbers::parseFrequency("440"), 440.0f);
    EXPECT_FLOAT_EQ(ParseNumbers::parseFrequency("1k"), 1000.0f);
    EXPECT_FLOAT_EQ(ParseNumbers::parseFrequency("2.5k"), 2500.0f);
    EXPECT_THROW(ParseNumbers::parseFrequency("abc"), std::invalid_argument);
}

TEST(ValueParserTest, parseTime)
{
    EXPECT_FLOAT_EQ(ParseNumbers::parseTime("500ms"), 500.0f);
    EXPECT_FLOAT_EQ(ParseNumbers::parseTime("2s"), 2000.0f);
    EXPECT_FLOAT_EQ(ParseNumbers::parseTime("3.5s"), 3500.0f);
    EXPECT_FLOAT_EQ(ParseNumbers::parseTime("1000"), 1000.0f);
    EXPECT_THROW(ParseNumbers::parseTime("abcms"), std::invalid_argument);
}

TEST(ValueParserTest, parseGain)
{
    EXPECT_FLOAT_EQ(ParseNumbers::parseGain("-6dB"), std::pow(10.0f, -6.0f / 20.0f));
    EXPECT_FLOAT_EQ(ParseNumbers::parseGain("3dB"), std::pow(10.0f, 3.0f / 20.0f));
    EXPECT_FLOAT_EQ(ParseNumbers::parseGain("1.5dB"), std::pow(10.0f, 1.5f / 20.0f));
    EXPECT_FLOAT_EQ(ParseNumbers::parseGain("1.0"), 1.0f);
    EXPECT_FLOAT_EQ(ParseNumbers::parseGain("0.5"), 0.5f);
    EXPECT_THROW(ParseNumbers::parseGain("abcdb"), std::invalid_argument);
}

TEST(ValueParserTest, parseSequencePlain)
{
    EXPECT_EQ(ParseNumbers::parseSequence("1, -2.5, 3.14"), (std::vector<float>{1.0f, -2.5f, 3.14f}));
    EXPECT_EQ(ParseNumbers::parseSequence("1.4E-2"), (std::vector<float>{0.014f}));
}

TEST(ValueParserTest, parseSequenceDecibels)
{
    EXPECT_FLOAT_EQ(ParseNumbers::parseSequence("-6dB")[0], std::pow(10.0f, -6.0f / 20.0f));
    EXPECT_FLOAT_EQ(ParseNumbers::parseSequence("3dB")[0], std::pow(10.0f, 3.0f / 20.0f));
}

TEST(ValueParserTest, parseSequencePercentage)
{
    EXPECT_EQ(ParseNumbers::parseSequence("50%"), (std::vector<float>{0.5f}));
    EXPECT_EQ(ParseNumbers::parseSequence("-25%"), (std::vector<float>{-0.25f}));
}

TEST(ValueParserTest, parseSequenceFrequencies)
{
    EXPECT_EQ(ParseNumbers::parseSequence("440Hz"), (std::vector<float>{440.0f}));
    EXPECT_EQ(ParseNumbers::parseSequence("1kHz"), (std::vector<float>{1000.0f}));
}

TEST(ValueParserTest, parseSequenceFractions)
{
    EXPECT_EQ(ParseNumbers::parseSequence("1/2"), (std::vector<float>{0.5f}));
    EXPECT_THROW(ParseNumbers::parseSequence("1/0"), std::invalid_argument);
}

TEST(ValueParserTest, parseSequenceNotes)
{
    EXPECT_NEAR(ParseNumbers::parseSequence("A4")[0], 440.0f, 1e-2);
    EXPECT_NEAR(ParseNumbers::parseSequence("C4")[0], 261.63f, 1e-2);
    EXPECT_NEAR(ParseNumbers::parseSequence("Bb3")[0], 233.08f, 1e-2);
}

TEST(ValueParserTest, parseSequenceRepetition)
{
    EXPECT_EQ(ParseNumbers::parseSequence("1,_"), (std::vector<float>{1.0f, 1.0f}));
    EXPECT_EQ(ParseNumbers::parseSequence("2,___"), (std::vector<float>{2.0f, 2.0f, 2.0f, 2.0f}));
    EXPECT_THROW(ParseNumbers::parseSequence("_"), std::invalid_argument);
}

TEST(ValueParserTest, parseSequenceMixed)
{
    auto result = ParseNumbers::parseSequence("1, -6dB, 50%, 440Hz, A4");
    ASSERT_EQ(result.size(), 5);
    EXPECT_FLOAT_EQ(result[0], 1.0f);
    EXPECT_FLOAT_EQ(result[1], std::pow(10.0f, -6.0f / 20.0f));
    EXPECT_FLOAT_EQ(result[2], 0.5f);
    EXPECT_FLOAT_EQ(result[3], 440.0f);
    EXPECT_NEAR(result[4], 440.0f, 1e-2);
}

TEST(ValueParserTest, parseSequenceInvalid)
{
    EXPECT_THROW(ParseNumbers::parseSequence("abc"), std::invalid_argument);
    EXPECT_THROW(ParseNumbers::parseSequence("1,z,2"), std::invalid_argument);
}

TEST(ValueParserTest, parseSequenceIntervals)
{
    EXPECT_NEAR(ParseNumbers::parseSequence("+12st")[0], 2.0f, 1e-5f);
    EXPECT_NEAR(ParseNumbers::parseSequence("-12st")[0], 0.5f, 1e-5f);
    EXPECT_NEAR(ParseNumbers::parseSequence("+7st")[0], std::pow(2.0f, 7.0f / 12.0f), 1e-5f);
    EXPECT_NEAR(ParseNumbers::parseSequence("-7st")[0], std::pow(2.0f, -7.0f / 12.0f), 1e-5f);
    EXPECT_NEAR(ParseNumbers::parseSequence("+100cts")[0], centsToPitchFactor(100.f), 1e-5f);
    EXPECT_NEAR(ParseNumbers::parseSequence("-50cts")[0], centsToPitchFactor(-50.f), 1e-5f);
}