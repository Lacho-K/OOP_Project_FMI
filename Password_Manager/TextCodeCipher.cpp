#include "TextCodeCipher.h"
#include "AutoCreator.hpp"

std::string TextCodeCipher::name() const
{
    return ID;
}

static AutoCreator<TextCodeCipher> _;