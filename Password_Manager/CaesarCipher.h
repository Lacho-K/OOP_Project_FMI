#pragma once
#include "Cipher.h"

class CaesarCipher : public Cipher
{

public:

    explicit CaesarCipher(int shift);

    std::string encrypt(const std::string& plain) override;
    std::string decrypt(const std::string& coded) override;

private:

    int shift;

};

class CaesarCreator : public CipherCreator
{
public:
    CaesarCreator() : CipherCreator("caesar") {}

    Cipher* create(const std::vector<std::string>& a) const override;
};

//трябва да е в cpp файла
static CaesarCreator __;