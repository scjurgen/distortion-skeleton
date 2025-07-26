#include <gtest/gtest.h>
#include "Numbers/ValueParser.h"
#include "Numbers/SequenceStore.h"

TEST(SequenceStoreTest, PredefinedSequences)
{
    SequenceStore store;

    EXPECT_EQ(store.getSequence("lin-up"), (std::vector{0.f, 1.f}));
    EXPECT_EQ(store.getSequence("lin-down"), (std::vector{1.f, 0.f}));
    EXPECT_EQ(store.getSequence("lin-up-down"), (std::vector{0.f, 1.f, 0.f}));
    EXPECT_EQ(store.getSequence("trapezium"), (std::vector{0.f, 1.f, 1.f, 0.f}));

    auto expo1down = store.getSequence("expo1down");
    EXPECT_EQ(expo1down.size(), 17);
    EXPECT_NEAR(expo1down.front(), 1.0f, 1e-5);
    EXPECT_NEAR(expo1down.back(), std::exp(-16.0f / 16.0f), 1e-5);

    auto expo1up = store.getSequence("expo1up");
    EXPECT_EQ(expo1up.size(), 17);
    EXPECT_NEAR(expo1up.front(), std::exp(-16.0f / 16.0f), 1e-5);
    EXPECT_NEAR(expo1up.back(), 1.0f, 1e-5);
}

TEST(SequenceStoreTest, ParseBasicSequences)
{
    SequenceStore store;

    auto result = store.parse("1,2,3");
    EXPECT_EQ(result, (std::vector{1.f, 2.f, 3.f}));

    result = store.parse("440Hz,_");
    EXPECT_EQ(result.size(), 2);
    EXPECT_FLOAT_EQ(result[0], 440.0f);
    EXPECT_FLOAT_EQ(result[1], 440.0f);

    result = store.parse("-6dB");
    EXPECT_FLOAT_EQ(result[0], std::pow(10.0f, -6.0f / 20.0f));
}

TEST(SequenceStoreTest, ParseKeyedSequences)
{
    SequenceStore store;

    auto result = store.parse("foo=1,2,3");
    EXPECT_EQ(result, (std::vector{1.f, 2.f, 3.f}));
    EXPECT_EQ(store.getSequence("foo"), (std::vector{1.f, 2.f, 3.f}));

    result = store.parse("bar=440Hz,_");
    EXPECT_EQ(result.size(), 2);
    EXPECT_FLOAT_EQ(result[0], 440.0f);
    EXPECT_FLOAT_EQ(result[1], 440.0f);

    result = store.parse("baz=-6dB");
    EXPECT_FLOAT_EQ(result[0], std::pow(10.0f, -6.0f / 20.0f));
}

TEST(SequenceStoreTest, RetrieveScaledSequences)
{
    SequenceStore store;

    auto result = store.getSequence("lin-up", 2.0f);
    EXPECT_EQ(result.size(), 2);
    EXPECT_FLOAT_EQ(result[0], 0.0f);
    EXPECT_FLOAT_EQ(result[1], 2.0f);

    EXPECT_EQ(store.getSequence("lin-up", 1, 1), (std::vector{1.f, 2.f}));
    EXPECT_EQ(store.getSequence("lin-up", -1, 0), (std::vector{0.f, -1.f}));
    EXPECT_EQ(store.getSequence("lin-up", -1, 1), (std::vector{1.f, 0.f}));

    result = store.getSequence("lin-down", -3.0f);
    EXPECT_EQ(result.size(), 2);
    EXPECT_FLOAT_EQ(result[0], -3.0f);
    EXPECT_FLOAT_EQ(result[1], -0.0f);

    auto expoScaled = store.getSequence("expo2down", 10.0f);
    for (size_t i = 0; i < expoScaled.size(); ++i)
    {
        EXPECT_NEAR(expoScaled[i], store.getSequence("expo2down")[i] * 10.0f, 1e-5);
    }
}

TEST(SequenceStoreTest, InvalidParse)
{
    SequenceStore store;
    EXPECT_THROW(store.parse("invalid-sequence"), std::invalid_argument);
    EXPECT_THROW(store.parse("sq:"), std::invalid_argument);
}

TEST(SequenceStoreTest, InvalidRetrieve)
{
    SequenceStore store;
    EXPECT_THROW(store.getSequence("non-existent"), std::out_of_range);
}

TEST(SequenceStoreTest, ParseAndRetrieveScaledNotes)
{
    SequenceStore store;

    auto result = store.parse("notes=A4,B4,C5");
    EXPECT_EQ(result.size(), 3);
    EXPECT_NEAR(result[0], 440.0f, 1e-2);
    EXPECT_NEAR(result[1], 493.88f, 1e-2);
    EXPECT_NEAR(result[2], 523.25f, 1e-2);

    auto scaledResult = store.getSequence("notes", 0.5f);
    EXPECT_EQ(scaledResult.size(), 3);
    EXPECT_NEAR(scaledResult[0], 220.0f, 1e-2);
    EXPECT_NEAR(scaledResult[1], 246.94f, 1e-2);
    EXPECT_NEAR(scaledResult[2], 261.63f, 1e-2);
}
