#include "Cipher.h"

CipherCreator::CipherCreator(std::string name):_name(name)
{
}

const std::string& CipherCreator::getName() const
{
    return _name;
}
