#include "CaesarCipher.h"
#include "AutoCreator.hpp"
#include <istream>
#include <ostream>

Cipher* CaesarCipher::makeFromArgs(const std::vector<std::string>& args)
{
    if (args.empty())
        throw std::invalid_argument("Missing shift ammount for CaesarCipher");

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
    // skip unsupported characters
    if (c < MIN_SYMBOL || c > MAX_SYMBOL)
        return c;

    int offset = shift * static_cast<int>(dir);

    // convert character to 0-based index
    int index = c - MIN_SYMBOL;

    // using modular arithmetic to handle very big shift
    int shiftedIndex = (index + offset + RANGE) % RANGE;

    // convert index back to actual character
    return static_cast<char>(MIN_SYMBOL + shiftedIndex);
}




static AutoCreator<CaesarCipher> _;
