#pragma once
#include "Cipher.h";

class CipherFactory
{
public:
    static CipherFactory& getFactory();

    Cipher* createCipher(const std::string& name,
        const std::vector<std::string>& args) const;

    void registerCipher(const CipherCreator* c);

private:
    CipherFactory();
    CipherFactory(const CipherFactory&) = delete;
    CipherFactory& operator=(const CipherFactory&) = delete;

    std::vector<const CipherCreator*> _creators;
};