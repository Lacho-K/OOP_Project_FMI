#include "CipherFactory.h"
#include <istream>

CipherFactory& CipherFactory::getInstance()
{
    static CipherFactory instance;
    return instance;
}

void CipherFactory::registerCipher(const CipherCreator* creator)
{
    creators.push_back(creator);
}

Cipher* CipherFactory::createCipher(const std::string& id, const std::vector<std::string>& args) const
{
    const CipherCreator* creator = getCreator(id);
    if (!creator)
        return nullptr;

    return creator->create(args);
}

const CipherCreator* CipherFactory::getCreator(const std::string& name) const
{
    for (const CipherCreator* creator : creators)
    {
        if (creator->getName() == name)
            return creator;
    }
    return nullptr;
}
