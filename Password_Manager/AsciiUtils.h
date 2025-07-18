#pragma once
#include <string>

namespace AsciiUtils
{
    static const int MIN_SYMBOL = 32;
    static const int MAX_SYMBOL = 126;
    static const int RANGE = MAX_SYMBOL - MIN_SYMBOL + 1;

    bool inAsciiRange(char c);

    int decodeChar(char c);
    char encodeChar(int index);

    int modRange(int x);

    std::string numberToString(int x);

    int parseInt(const std::string& s);
}
