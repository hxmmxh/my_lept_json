#include "leptjson.h"
#include <string.h>
#include <cctype>
#include <errno.h>
#include <math.h>

namespace feiyan
{
    JsonContext::JsonContext(size_t length)
    {
        stack_ = static_cast<char *>(malloc(length));
        size_ = length;
        top_ = 0;
        data_ = nullptr;
    }

    void JsonContext::push(const char *string, size_t length)
    {
        //容量不足，扩大1.5倍
        while (top_ + length >= size_)
            size_ += size_ >> 1;
        stack_ = static_cast<char *>(realloc(stack_, size_));
        memcpy(stack_ + top_, string, length);
        top_ += length;
    }

    void JsonContext::pop(char *den, size_t length)
    {
        assert(top_ >= length);
        memcpy(den, stack_ + top_ - length, length);
        top_ -= length;
    }

    void Json::parseWs()
    {
        const char *p = context_.data_;
        while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
            p++;
        context_.data_ = p;
    }

    JsonStat Json::expect(const char *string)
    {
        const char *p = context_.data_;
        if (memcmp(p, string, strlen(string)))
            return InvalidValue;
        context_.data_ += strlen(string);
        return OK;
    }

    JsonStat Json::parseLiteral(const char *string, JsonType type)
    {
        if (OK == expect(string))
        {
            type_ = type;
            return OK;
        }
        else
        {
            return InvalidValue;
        }
    }

    //少了必要的元素会返回InvalidValue
    //如果多了元素，会在parseValue时返回RootNotSingular，例如开头有多个0
    JsonStat Json::parseNumber()
    {
        const char *p = context_.data_;
        if (*p == '-')
            ++p;
        if (*p == '0')
            ++p;
        else
        {
            if (!isdigit(*p))
                return InvalidValue;
            while (isdigit(*p))
                ++p;
        }
        if (*p == '.')
        {
            ++p;
            if (!isdigit(*p))
                return InvalidValue;
            while (isdigit(*p))
                ++p;
        }
        if (*p == 'e' || *p == 'E')
        {
            ++p;
            if (*p == '+' || *p == '-')
                ++p;
            if (!isdigit(*p))
                return InvalidValue;
            while (isdigit(*p))
                ++p;
        }

        //一些标准库函数通过写入正整数到 errno 指定错误。
        //通常，会将 errno 的值设置为错误码之一。
        //错误码作为以字母 E 后随大写字母或数字开始的宏常量，列于 <errno.h> 。
        errno = 0;
        n_ = strtod(context_.data_, nullptr);
        if (errno == ERANGE && (n_ == HUGE_VAL || n_ == -HUGE_VAL))
            return NumberTooBig;
        type_ = NumberType;
        context_.data_ = p;
        return OK;
    }

    JsonStat Json::parseValue()
    {
        switch (*context_.data_)
        {
        case 'n':
            return parseLiteral("null", NullType);
        case 't':
            return parseLiteral("true", TrueType);
        case 'f':
            return parseLiteral("false", FalseType);
        case '"':
            return parseString();
        case '\0':
            return ExpectValue;
        //无论default位置在前在后，都是先判断各个case, 最后进default。
        default:
            return parseNumber();
        }
    }

    JsonStat Json::parse(const char *json)
    {
        context_.reset();
        context_.data_ = json;
        parseWs();
        JsonStat res = parseValue();
        if (res == OK)
        {
            parseWs();
            if (*context_.data_ != '\0')
            {
                type_ = NullType;
                return RootNotSingular;
            }
        }
        return res;
    }

    void Json::LeptFree()
    {
        if (type_ == StringType)
            free(string_.s_);
        type_ == NullType;
    }

    double Json::getNumber()
    {
        assert(type_ == NumberType);
        return n_;
    }

} // namespace feiyan
