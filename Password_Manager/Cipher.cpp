#include "Cipher.h"
#include "CipherFactory.h"

CipherCreator::CipherCreator(std::string name):_name(name)
{
    CipherFactory::getInstance().registerCipher(this);
}

const std::string& CipherCreator::getName() const
{
    return _name;
}
