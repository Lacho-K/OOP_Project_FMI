#include "CaesarCipher.h"

Cipher* CaesarCipher::makeFromArgs(const std::vector<std::string>& args)
{
    int s = std::atoi(args[0].c_str());
    return new CaesarCipher(s);
}

std::string CaesarCipher::name() const
{
    return ID;
}

static AutoCreator<CaesarCipher> _;
