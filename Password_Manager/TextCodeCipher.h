#pragma once
#pragma once
#include "Cipher.h"
#include <vector>

class TextCodeCipher : public Cipher
{
public:
    explicit TextCodeCipher(const std::string& referencePath);
    std::string encrypt(const std::string& plain) override;
    std::string decrypt(const std::string& coded) override;
private:
    std::vector<int> encode;
    std::vector<char> decode;
};

class TextCodeCreator : public CipherCreator
{
public:
    TextCodeCreator() : CipherCreator("textcode") {}

    Cipher* create(const std::vector<std::string>& a) const override;
};

//трябва да е в cpp файла
static TextCodeCreator __;