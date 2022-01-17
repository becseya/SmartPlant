#pragma once

#include <cstring>
#include <functional>
#include <map>
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

class Field
{
  public:
    Field(bool& var)
        : ptr(&var)
        , type(Type::Bool)
    {}

    Field(int& var)
        : ptr(&var)
        , type(Type::Int)
    {}

    Field(float& var)
        : ptr(&var)
        , type(Type::Float)
    {}

    bool parse(const char* str)
    {
        switch (type) {
            case Type::Bool:
                if (*str == 't') {
                    *static_cast<bool*>(ptr) = true;
                    return true;
                } else if (*str == 'f') {
                    *static_cast<bool*>(ptr) = false;
                    return true;
                } else
                    return false;
            case Type::Float: //
                *static_cast<float*>(ptr) = atof(str);
                return true;
            case Type::Int: //
                *static_cast<int*>(ptr) = atoi(str);
                return true;
            default: //
                return false;
        }
    }

  private:
    enum class Type
    {
        Bool,
        Int,
        Float,
    };

    void* ptr;
    Type  type;
};

class ObjectParser
{
  public:
    void addField(const char* key, bool& val)
    { //
        fields.emplace(key, Field(val));
    }

    void addField(const char* key, int& val)
    { //
        fields.emplace(key, Field(val));
    }

    void addField(const char* key, float& val)
    { //
        fields.emplace(key, Field(val));
    }

    bool parse(char* str)
    {
        bool error = false;

        if (!startObject(str))
            return false;

        do {
            const char* key = getNextKey(str);
            if (!key)
                return false;

            auto it = fields.find(key);
            if (it != fields.end()) {
                if (!it->second.parse(str))
                    return false;
            }

            skipField(str);
        } while (!isEndOfObject(str, error));

        return !error;
    }

  private:
    struct cmp_str
    {
        bool operator()(const char* a, const char* b) const
        { //
            return strcmp(a, b) < 0;
        }
    };

    using fields_t = std::map<const char*, Field, cmp_str>;

    void skipWhitespace(char*& str)
    {
        while ((*str == ' ') || (*str == '\t') || (*str == '\r') || (*str == '\n'))
            str++;
    }

    bool skipUpToChar(char*& str, char c)
    {
        while ((*str != c) && (*str != '\0'))
            str++;

        return (*str == c);
    }

    void skipField(char*& str)
    {
        while ((*str != FIELD_SEPARATOR) && (*str != OBJ_END) && (*str != '\0'))
            str++;
    }

    bool startObject(char*& str)
    {
        skipWhitespace(str);
        return (*str++ == OBJ_START);
    }

    const char* getNextKey(char*& str)
    {
        skipWhitespace(str);

        if (*str != ESCAPER)
            return nullptr;

        const char* keyBegin = ++str;

        if (!skipUpToChar(str, ESCAPER))
            return nullptr;

        *str++ = '\0';

        if (!skipUpToChar(str, KEY_VAL_SEPARATOR))
            return nullptr;

        str++;

        return keyBegin;
    }

    bool isEndOfObject(char*& str, bool& error)
    {
        skipWhitespace(str);

        if (*str == OBJ_END)
            return true;
        else if (*str == FIELD_SEPARATOR) {
            str++;
            return false;
        } else {
            error = true;
            return true;
        }
    }

    fields_t fields;
};

} // namespace Json
