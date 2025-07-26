#pragma once

#include <functional>

class SplitProcessing
{
  public:
    template <typename ProcessCallbackFunc> // std::function<bool(size_t index, size_t size)>&& procFunction
    static size_t run(const size_t blkSize, const size_t numSamples, const ProcessCallbackFunc& processCallback)
    {
        size_t index = 0;
        while (index < numSamples)
        {
            size_t size = numSamples - index < blkSize ? numSamples - index : blkSize;
            // return if the processing functions signals with false
            if (!processCallback(index, size))
            {
                // return the latest processed position
                return index + size;
            }
            index += size;
        }
        return index;
    }
};
