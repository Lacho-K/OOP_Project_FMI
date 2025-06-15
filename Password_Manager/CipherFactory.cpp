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

Cipher* CipherFactory::createCipher(std::istream& in) const
{
    std::string name;
    in >> name;

    const CipherCreator* creator = getCreator(name);
    if (!creator)
    {
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return nullptr;
    }

    std::vector<std::string> args;
    std::string arg;
    while (in >> arg)
    {
        args.push_back(arg);
    }

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
