#pragma once
#include "Cipher.h"
#include "AutoCreator.hpp"

class CaesarCipher : public Cipher
{
public:
    static const std::string ID;

    static Cipher* makeFromArgs(const std::vector<std::string>& args);

    explicit CaesarCipher(int shift);

    std::string encrypt(const std::string& plain) const override;
    std::string decrypt(const std::string& coded) const override;
    std::string name() const override;
    void writeConfig(std::ostream& out) const override;
    void readConfig(std::istream& in) override;

private:
    int _shift;
};


//class CaesarCreator : public CipherCreator
//{
//public:
//    CaesarCreator() : CipherCreator("caesar") {}
//
//    Cipher* create(const std::vector<std::string>& a) const override;
//};