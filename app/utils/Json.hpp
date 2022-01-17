#pragma once

#include <functional>
#include <string>

namespace Json {

static constexpr char ESCAPER           = '"';
static constexpr char FIELD_SEPARATOR   = ',';
static constexpr char KEY_VAL_SEPARATOR = ':';
static constexpr char OBJ_START         = '{';
static constexpr char OBJ_END           = '}';

class Builder
{
  public:
    Builder()
    { //
        reset();
    }

    void reset()
    {
        buffer.clear();
        isTerminated = false;
        isStarted    = false;
    }

    const char* toString()
    {
        if (!isTerminated)
            buffer += OBJ_END;

        isTerminated = true;
        return buffer.c_str();
    }

    template<typename T>
    void append(const char* key, T val)
    {
        if (!isStarted)
            buffer += OBJ_START;
        else
            buffer += FIELD_SEPARATOR;

        buffer += ESCAPER;
        buffer += key;
        buffer += ESCAPER;
        buffer += KEY_VAL_SEPARATOR;
        buffer += std::to_string(val);

        isStarted = true;
    }

  private:
    std::string buffer;
    bool        isTerminated;
    bool        isStarted;
};

} // namespace Json
