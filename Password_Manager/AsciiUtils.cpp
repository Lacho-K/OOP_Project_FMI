#include "AsciiUtils.h"
#include <stdexcept>

namespace AsciiUtils
{
    bool inAsciiRange(char c)
    {
        return c >= MIN_SYMBOL && c <= MAX_SYMBOL;
    }

    int decodeChar(char c)
    {
        return c - MIN_SYMBOL;
    }

    char encodeChar(int index)
    {
        return static_cast<char>(MIN_SYMBOL + modRange(index));
    }

    int modRange(int x)
    {
        int result = x % RANGE;
        return result < 0 ? result + RANGE : result;
    }

    std::string numberToString(int x)
    {
        if (x == 0)
            return "0";

        std::string result;
        bool negative = x < 0;
        if (negative)
            x = -x;

        while (x > 0)
        {
            result.insert(result.begin(), static_cast<char>('0' + (x % 10)));
            x /= 10;
        }

        if (negative)
            result.insert(result.begin(), '-');

        return result;
    }

    int parseInt(const std::string& s)
    {
        int result = 0;
        int i = 0;
        bool negative = false;

        if (!s.empty() && s[0] == '-')
        {
            negative = true;
            i = 1;
        }

        for (; i < static_cast<int>(s.size()); i++)
        {
            char c = s[i];
            if (c < '0' || c > '9')
                throw std::invalid_argument("Invalid integer format");

            result = result * 10 + (c - '0');
        }

        return negative ? -result : result;
    }
}
