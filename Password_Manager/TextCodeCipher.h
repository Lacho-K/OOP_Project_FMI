#pragma once
#include "Cipher.h"

class TextCodeCipher : public Cipher
{
public:
    static const std::string ID;

    static Cipher* makeFromArgs(const std::vector<std::string>& args);

    TextCodeCipher();
    explicit TextCodeCipher(const std::string& referencePath);

    std::string encrypt(const std::string& plain) const override;
    std::string decrypt(const std::string& coded) const override;
    std::string name() const override;

    void writeConfig(std::ostream& out) const override;
    void readConfig(std::istream& in) override;

private:
    std::vector<int> _encode;   // ASCII to position
    std::vector<char> _decode;  // position to char

    void buildFromText(std::istream& input);
};



//class TextCodeCreator : public CipherCreator
//{
//public:
//    TextCodeCreator() : CipherCreator("textcode") {}
//
//    Cipher* create(const std::vector<std::string>& a) const override;
//};