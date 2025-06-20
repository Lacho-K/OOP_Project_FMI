#include "CaesarCipher.h"
#include "AutoCreator.hpp"
#include "AsciiUtils.h"
#include <istream>
#include <ostream>

const std::string CaesarCipher::ID = "caesar";

Cipher* CaesarCipher::makeFromArgs(const std::vector<std::string>& args)
{
    if (args.empty())
        throw std::invalid_argument("Missing shift amount for CaesarCipher");

    int s = AsciiUtils::parseInt(args[0]);
    return new CaesarCipher(s);
}

CaesarCipher::CaesarCipher(int shift) : shift(shift) {}

// създава валиден обект, не променя текста при encrypt и decrypt, използва се 
//за createFromStream
CaesarCipher::CaesarCipher() : shift(0) {}

std::string CaesarCipher::encrypt(const std::string& plain) const
{
    std::string result;
    for (char c : plain)
        result += shiftSymbol(c, ShiftDirection::LEFT); // местим налявo за encrypt
    return result;
}

std::string CaesarCipher::decrypt(const std::string& coded) const
{
    std::string result;
    for (char c : coded)
        result += shiftSymbol(c, ShiftDirection::RIGHT); // местим налявo за decrypt
    return result;
}

std::string CaesarCipher::name() const { return ID; }

void CaesarCipher::writeConfig(std::ostream& out) const
{
    out.write(reinterpret_cast<const char*>(&shift), sizeof(shift));
}

void CaesarCipher::readConfig(std::istream& in)
{
    in.read(reinterpret_cast<char*>(&shift), sizeof(shift));
}

char CaesarCipher::shiftSymbol(char c, ShiftDirection dir) const
{
    if (!AsciiUtils::inAsciiRange(c))
        return c;

    int offset = shift * static_cast<int>(dir);       // с колко ще местим
    int index = AsciiUtils::decodeChar(c);            // превръщаме символ в число
    int shifted = AsciiUtils::modRange(index + offset); // модулно изместване
    return AsciiUtils::encodeChar(shifted);           // връщаме обратно като символ
}

static AutoCreator<CaesarCipher> _;
