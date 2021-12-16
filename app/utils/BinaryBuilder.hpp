#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdint.h>

template<size_t MAX_BUFFER_SIZE>
class BinaryBuilder
{
  public:
    template<typename T>
    void append(const T& val)
    {
        constexpr size_t valSize = sizeof(T);
        const uint8_t*   valPtr  = (const uint8_t*)(void*)&val;

        if (MAX_BUFFER_SIZE - getSize() < valSize)
            return;

        // Big endian format
        for (int i = 0; i < valSize; i++)
            ptr[valSize - 1 - i] = valPtr[i];

        ptr += valSize;
    }

    uint8_t* getPtr()
    { //
        return buffer;
    }

    size_t getSize()
    { //
        return (ptr - buffer);
    }

    void reset()
    { //
        ptr = buffer;
    }

    int copyTo(void* destination, size_t destinationSize)
    {
        if (getSize() > destinationSize)
            return -1;

        memcpy(destination, getPtr(), getSize());
        return getSize();
    }

  private:
    uint8_t buffer[MAX_BUFFER_SIZE];

    uint8_t* ptr = buffer;
};
