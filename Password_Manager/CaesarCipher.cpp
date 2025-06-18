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

CaesarCipher::CaesarCipher(int shift) : shift(shift)
{}

// създава валиден обект, не променя текста при encrypt и decrypt
CaesarCipher::CaesarCipher() : shift(0)
{}

std::string CaesarCipher::encrypt(const std::string& plain) const
{
    std::string result;
    for (char c : plain)
    {
        result += shiftSymbol(c, ShiftDirection::LEFT);
    }
    return result;
}

std::string CaesarCipher::decrypt(const std::string& coded) const
{
    std::string result;
    for (char c : coded)
    {
        result += shiftSymbol(c, ShiftDirection::RIGHT);
    }
    return result;
}

std::string CaesarCipher::name() const
{
    return ID;
}

void CaesarCipher::writeConfig(std::ostream& out) const
{
    out << shift << '\n';
}

void CaesarCipher::readConfig(std::istream& in)
{
    in >> shift;
}

char CaesarCipher::shiftSymbol(char c, ShiftDirection dir) const
{
    if (!AsciiUtils::inAsciiRange(c))
        return c;

    int offset = shift * static_cast<int>(dir);
    int index = AsciiUtils::decodeChar(c);
    int shifted = AsciiUtils::modRange(index + offset);
    return AsciiUtils::encodeChar(shifted);
}

static AutoCreator<CaesarCipher> _;
