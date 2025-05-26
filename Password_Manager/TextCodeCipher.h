#pragma once
#include "Cipher.h"
#include <vector>

class TextCodeCipher : public Cipher
{
public:
    static const std::string ID;

    static Cipher* makeFromArgs(const std::vector<std::string>& args);

    explicit TextCodeCipher(const std::string& refPath);

    std::string encrypt(const std::string& plain) const override;
    std::string decrypt(const std::string& coded) const override;
    std::string name() const override;
    void writeConfig(std::ostream& out) const override;
    void readConfig(std::istream& in) override;

private:
    std::vector<int> _encode;
    std::vector<char> _decode;
};

//class TextCodeCreator : public CipherCreator
//{
//public:
//    TextCodeCreator() : CipherCreator("textcode") {}
//
//    Cipher* create(const std::vector<std::string>& a) const override;
//};