
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "ParameterSmoothing.h"

float setThisRight{1000.f};
void setThisPure(float value)
{
    setThisRight = value;
}

TEST(ParameterSmoothingTest, checkCallbackWithNew)
{
    Smoother<float>* sut = new Smoother<float>{};

    sut->preset(1.0, 1, 0.25);
    sut->setCallback(setThisPure);
    EXPECT_FLOAT_EQ(setThisRight, 0.999899983f);
    sut->handleTickHasNewValue();
    EXPECT_FLOAT_EQ(setThisRight, 1.0f);
    sut->passValue(0.0);
    sut->handleTickHasNewValue();
    EXPECT_FLOAT_EQ(setThisRight, 0.75f);
    sut->handleTickHasNewValue();
    EXPECT_FLOAT_EQ(setThisRight, 0.5f);
    delete sut;
}


TEST(ParameterSmoothingTest, checkCallbackPreset)
{
    Smoother<float> sut;
    float setThisRight{1000.f};
    sut.setCallback([&setThisRight](float value) { setThisRight = value; });
    sut.preset(1.0, 1, 0.25);
    sut.handleTickHasNewValue();
    EXPECT_FLOAT_EQ(setThisRight, 1.0f);
    sut.passValue(0.0);
    sut.handleTickHasNewValue();
    EXPECT_FLOAT_EQ(setThisRight, 0.75f);
}

TEST(ParameterSmoothingTest, callBackOnlyOnValueChange)
{
    Smoother<float> sut;
    float setThisRight{1000.f};
    int count = 0;
    sut.setCallback(
        [&setThisRight, &count](float value)
        {
            setThisRight = value;
            count++;
        });
    sut.preset(1.0f, 1, 0.25f);
    sut.passValue(0.1f);
    sut.handleTickHasNewValue();
    EXPECT_NE(setThisRight, 1000.f);
    EXPECT_EQ(count, 2);
    sut.handleTickHasNewValue();
    sut.handleTickHasNewValue();
    sut.handleTickHasNewValue();
    sut.handleTickHasNewValue();
    sut.handleTickHasNewValue();
    sut.handleTickHasNewValue();
    EXPECT_FLOAT_EQ(setThisRight, 0.1f);
    EXPECT_EQ(count, 5);
}

TEST(ParameterSmoothingTest, honorGranularity)
{
    Smoother<float> sut;
    int count = 0;
    sut.setCallback([&count](float value) { count++; });
    sut.preset(1.0, 3, 0.00025);
    sut.passValue(0.1f);
    sut.handleTickHasNewValue();
    sut.handleTickHasNewValue();
    sut.handleTickHasNewValue();
    sut.handleTickHasNewValue();
    sut.handleTickHasNewValue();
    EXPECT_EQ(count, 2);
    sut.handleTickHasNewValue();
    sut.handleTickHasNewValue();
    sut.handleTickHasNewValue();
    EXPECT_EQ(count, 3);
    sut.handleTickHasNewValue();
    sut.handleTickHasNewValue();
    sut.handleTickHasNewValue();
    EXPECT_EQ(count, 4);
}

TEST(ParameterSmoothingTest, test100Steps)
{
    Smoother<float> sut{};
    sut.setCallback([](float value) {});
    sut.preset(0.0, 1, 0.01);
    sut.passValue(1.0);
    for (int i = 0; i < 100; ++i)
    {
        sut.handleTickHasNewValue();
        EXPECT_LT(sut.getCurrentValue(), 1.0f);
    }
    sut.handleTickHasNewValue();
    EXPECT_FLOAT_EQ(sut.getCurrentValue(), 1.0f);
}

TEST(ParameterSmoothingTest, testIntermediateChange)
{
    Smoother<float> sut{};
    sut.preset(0.0, 1, 0.01);
    sut.passValue(1.0);
    for (int i = 0; i < 50; ++i)
    {
        sut.handleTickHasNewValue();
        EXPECT_LE(sut.getCurrentValue(), 0.5f);
    }
    sut.passValue(.0);
    sut.handleTickHasNewValue();
    for (int i = 0; i < 50; ++i)
    {
        sut.handleTickHasNewValue();
        EXPECT_GE(sut.getCurrentValue(), 0.0f);
    }
    sut.handleTickHasNewValue();
    EXPECT_LE(sut.getCurrentValue(), 0.0f);
}
