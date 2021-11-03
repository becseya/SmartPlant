#pragma once

#include "mbed.h"
#include <cstring>

template<size_t BUFFER_SIZE>
class LineBufferedSerial
{
  public:
    LineBufferedSerial(PinName tx, PinName rx)
        : uart(tx, rx)
        , lineIdx(0)
    {
        uart.set_blocking(false);
    }

    const char* readNextLine()
    {
        char c;

        while (uart.read(&c, 1) == 1) {
            if (c == '\r' || c == '\n') {
                if (lineIdx > 0) {
                    lineBuffer[lineIdx] = '\0';
                    lineIdx             = 0;
                    return lineBuffer;
                }
            } else if (lineIdx < (BUFFER_SIZE - 1))
                lineBuffer[lineIdx++] = c;
        }

        return nullptr;
    }

    void sendText(const char* str)
    {
        size_t len = strlen(str);
        uart.write(str, len);
    }

  protected:
    BufferedSerial uart;

  private:
    char   lineBuffer[BUFFER_SIZE];
    size_t lineIdx;
};
