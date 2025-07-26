
#include "gtest/gtest.h"

#include "Numbers/Segmenter.h"


class SegmenterTest : public ::testing::Test
{
  protected:
    Segmenter segmenter;

    void SetUp() override
    {
        segmenter.addSegment(0, 4);
        segmenter.addSegment(5, 2);
        segmenter.addSegment(9, 3);
        segmenter.addSegment(12, 1);
        segmenter.addSegment(14, 2);
    }
};

TEST_F(SegmenterTest, AddSegment)
{
    Segmenter newSegmenter;
    EXPECT_NO_THROW(newSegmenter.addSegment(0, 4));
    EXPECT_NO_THROW(newSegmenter.addSegment(5, 2));
    EXPECT_THROW(newSegmenter.addSegment(5, 3), std::invalid_argument);  // Overlapping segment
    EXPECT_THROW(newSegmenter.addSegment(15, 0), std::invalid_argument); // Zero-length segment
}

TEST_F(SegmenterTest, ClearSegments)
{
    segmenter.clear();
    size_t index = 0;
    size_t insidePosition = 0;
    float normalizedPosition = 0.0f;
    EXPECT_FALSE(segmenter.getSegmentNumber(0, index, insidePosition, normalizedPosition)); // No segments left
}

TEST_F(SegmenterTest, GetSegmentNumberValid)
{
    size_t index = 0;
    size_t insidePosition = 0;
    float normalizedPosition = 0.0f;

    // Check position within the first segment
    EXPECT_TRUE(segmenter.getSegmentNumber(2, index, insidePosition, normalizedPosition));
    EXPECT_EQ(index, 0);
    EXPECT_EQ(insidePosition, 2);
    EXPECT_FLOAT_EQ(normalizedPosition, 2.0f / 3.0f);
    EXPECT_TRUE(segmenter.getSegmentNumber(3, index, insidePosition, normalizedPosition));
    EXPECT_EQ(insidePosition, 3);
    EXPECT_FLOAT_EQ(normalizedPosition, 1.0f);

    // Check position within the second segment
    EXPECT_TRUE(segmenter.getSegmentNumber(6, index, insidePosition, normalizedPosition));
    EXPECT_EQ(index, 1);
    EXPECT_EQ(insidePosition, 1);
    EXPECT_FLOAT_EQ(normalizedPosition, 1.0f); // Normalized position

    // Check position within the last segment
    EXPECT_TRUE(segmenter.getSegmentNumber(15, index, insidePosition, normalizedPosition));
    EXPECT_EQ(index, 4);
    EXPECT_EQ(insidePosition, 1);
    EXPECT_FLOAT_EQ(normalizedPosition, 1.0f); // Normalized position
}

TEST_F(SegmenterTest, GetSegmentNumberInvalid)
{
    size_t index = 0;
    size_t insidePosition = 0;
    float normalizedPosition = 0.0f;

    // Position before any segment
    EXPECT_FALSE(segmenter.getSegmentNumber(-1, index, insidePosition, normalizedPosition));

    // Position after all segments
    EXPECT_FALSE(segmenter.getSegmentNumber(20, index, insidePosition, normalizedPosition));

    // Position in a gap between segments
    EXPECT_FALSE(segmenter.getSegmentNumber(8, index, insidePosition, normalizedPosition));
}
