#include "Cipher.h"
#include "CipherFactory.h"

// шифърите са саморегистрират чрез този конструктор
CipherCreator::CipherCreator(std::string name):_name(name)
{
    CipherFactory::getInstance().registerCipher(this);
}

const std::string& CipherCreator::getName() const
{
    return _name;
}
