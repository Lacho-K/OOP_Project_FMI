﻿#include "TextCodeCipher.h"
#include "AutoCreator.hpp"
#include "AsciiUtils.h"
#include <fstream>

const std::string TextCodeCipher::ID = "textcode";

std::string TextCodeCipher::name() const
{
    return ID;
}

Cipher* TextCodeCipher::makeFromArgs(const std::vector<std::string>& args)
{
    if (args.empty())
        throw std::invalid_argument("Missing file path for TextCodeCipher");

    return new TextCodeCipher(args[0]);
}

// създава валиден обект, не променя текста при encrypt и decrypt, използва се 
//за createFromStream
TextCodeCipher::TextCodeCipher()
{
    _encode.resize(128);
    _decode.resize(128);

    for (int i = 0; i < 128; i++)
    {
        _encode[i] = i;
        _decode[i] = static_cast<char>(i);
    }
}

// Създава шифъра от референтен файл
TextCodeCipher::TextCodeCipher(const std::string& referencePath)
    : _encode(128, -1)
{
    std::ifstream in(referencePath);
    if (!in)
        throw std::runtime_error("Cannot open reference file");

    buildFromText(in);
}

// Чете текст и запомня първата поява на всеки ASCII символ
void TextCodeCipher::buildFromText(std::istream& input)
{
    char c;
    int pos = 0;

    while (input.get(c))
    {
        if (AsciiUtils::inAsciiRange(c) && _encode[c] == -1)
        {
            _encode[c] = pos;          // запомняме позицията при първа поява
            _decode.push_back(c);      // позицията ще отговаря на символа
        }
        pos++;
    }
}

std::string TextCodeCipher::encrypt(const std::string& plain) const
{
    std::string result;

    for (char c : plain)
    {
        if (!AsciiUtils::inAsciiRange(c) || _encode[c] == -1)
            throw std::runtime_error("Unknown character in input");

        // Преобразуваме кода в число и го добавяме със space
        result += AsciiUtils::numberToString(_encode[c]) + ' ';
    }

    return result;
}

std::string TextCodeCipher::decrypt(const std::string& coded) const
{
    std::string result;
    int num = 0;

    for (char c : coded)
    {
        if (c == ' ')
        {
            if (num < 0 || num >= _decode.size())
                throw std::runtime_error("Invalid code");

            result += _decode[num];
            num = 0;
        }
        else if (c >= '0' && c <= '9')
        {
            num = num * 10 + (c - '0');
        }
        else
        {
            throw std::runtime_error("Invalid character in encoded input");
        }
    }

    return result;
}

void TextCodeCipher::writeConfig(std::ostream& out) const
{
    size_t count = _decode.size();
    if (count == 0)
        throw std::runtime_error("Cannot write empty cipher");

    out.write(reinterpret_cast<const char*>(&count), sizeof(count));
    out.write(&_decode[0], count);
}

void TextCodeCipher::readConfig(std::istream& in)
{
    size_t count = 0;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));
    if (count == 0)
        throw std::runtime_error("Invalid cipher");

    _decode.assign(count, '\0');
    _encode.assign(128, -1);

    in.read(&_decode[0], count);

    // Възстановяване на encode таблицата от decode
    for (size_t i = 0; i < count; i++)
    {
        char c = _decode[i];
        if (AsciiUtils::inAsciiRange(c))
            _encode[c] = static_cast<int>(i);
    }
}

static AutoCreator<TextCodeCipher> _;
