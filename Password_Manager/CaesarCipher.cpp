#include "CaesarCipher.h"
#include "AutoCreator.hpp"
#include <istream>
#include <ostream>

Cipher* CaesarCipher::makeFromArgs(const std::vector<std::string>& args)
{
    int s = std::atoi(args[0].c_str());
    return new CaesarCipher(s);
}

CaesarCipher::CaesarCipher(int shift) : shift(shift)
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
    if (c < MIN_SYMBOL || c > MAX_SYMBOL)
        return c;

    int targetIndex = c + (shift * static_cast<int>(dir));
    char targetSymbol;

    if (targetIndex > MAX_SYMBOL)
        targetSymbol = MIN_SYMBOL + (targetIndex - MAX_SYMBOL - 1);
    else if (targetIndex < MIN_SYMBOL)
        targetSymbol = MAX_SYMBOL - (MIN_SYMBOL - targetIndex - 1);
    else
        targetSymbol = targetIndex;

    return targetSymbol;
}


static AutoCreator<CaesarCipher> _;
