
#include "gtest/gtest.h"

#include "Numbers/ValueSequenceInterpolator.h"


TEST(ValueFollowerTest, LagrangeInterpolationBetweenTwoPoints)
{
    ValueSequenceInterpolator<SeqIntType::Lagrange> sut;
    sut.setPoints({2.f, 3.f});

    // Expected interpolation points: {2, 2, 3, 3}
    EXPECT_FLOAT_EQ(sut.getValue(0.0f), 2.0f); // At start
    EXPECT_FLOAT_EQ(sut.getValue(0.5f), 2.5f); // Midpoint between 2 and 3
    EXPECT_FLOAT_EQ(sut.getValue(1.0f), 3.0f); // At end
}

TEST(ValueFollowerTest, LagrangeInterpolationWithMultiplePoints)
{
    ValueSequenceInterpolator<SeqIntType::Lagrange> sut;
    sut.setPoints({0.f, 3.f, 2.f, 0.f});

    // Expected interpolation points: {0, 0, 3, 2, 0, 0}
    EXPECT_FLOAT_EQ(sut.getValue(0.0f), 0.0f);
    EXPECT_FLOAT_EQ(sut.getValue(0.25f), 2.3515625f);
    EXPECT_FLOAT_EQ(sut.getValue(0.5f), 2.8125f);
    EXPECT_FLOAT_EQ(sut.getValue(0.75f), 1.4765625f);
    EXPECT_FLOAT_EQ(sut.getValue(1.0f), 0.0f);
}

TEST(ValueFollowerTest, LagrangeFinalizeWithoutAddingPoints)
{
    ValueSequenceInterpolator<SeqIntType::Lagrange> sut;
    sut.setPoints({});

    // Default points: {1, 1, 1}
    EXPECT_FLOAT_EQ(sut.getValue(0.0f), 1.0f);
    EXPECT_FLOAT_EQ(sut.getValue(1.0f), 1.0f);
}

TEST(ValueFollowerTest, LagrangeisConstantEvaluation)
{
    ValueSequenceInterpolator<SeqIntType::Lagrange> sut;
    sut.setPoints({0.5f, 0.5f, 0.5f});

    // Default points: {1, 1, 1}
    EXPECT_TRUE(sut.isConstant());
    EXPECT_FLOAT_EQ(sut.getByIndex(0), 0.5f);
    EXPECT_TRUE(sut.isConstant(0.5f));
    EXPECT_FALSE(sut.isConstant(1.0f));
}

TEST(ValueFollowerTest, LinearInterpolationWithMultiplePoints)
{
    ValueSequenceInterpolator<SeqIntType::Linear> sut({0.f, 3.f, 2.f, 1.f, 0.f});

    EXPECT_FLOAT_EQ(sut.getValue(0.0f), 0.0f);
    EXPECT_FLOAT_EQ(sut.getValue(0.125f), 1.5f);
    EXPECT_FLOAT_EQ(sut.getValue(0.25f), 3.f);
    EXPECT_FLOAT_EQ(sut.getValue(0.375f), 2.5f);
    EXPECT_FLOAT_EQ(sut.getValue(0.5f), 2.f);
    EXPECT_FLOAT_EQ(sut.getValue(0.625f), 1.5f);
    EXPECT_FLOAT_EQ(sut.getValue(0.75f), 1.f);
    EXPECT_FLOAT_EQ(sut.getValue(0.875f), 0.5f);
    EXPECT_FLOAT_EQ(sut.getValue(1.0f), 0.0f);
}
