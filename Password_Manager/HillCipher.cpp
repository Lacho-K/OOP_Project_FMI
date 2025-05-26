#include "HillCipher.h"
#include "AutoCreator.hpp"

std::string HillCipher::name() const
{
    return ID;
}

static AutoCreator<HillCipher> _;