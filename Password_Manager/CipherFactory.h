#pragma once
#include "Cipher.h"

class CipherFactory
{
public:
    static CipherFactory& getInstance();

    void registerCipher(const CipherCreator* creator);

    Cipher* createCipher(const std::string& id, const std::vector<std::string>& args) const;

private:
    CipherFactory() = default;
    ~CipherFactory() = default;

    CipherFactory(const CipherFactory&) = delete;
    CipherFactory& operator=(const CipherFactory&) = delete;

    const CipherCreator* getCreator(const std::string& name) const;

    std::vector<const CipherCreator*> creators;
};
