#pragma once
#include "Cipher.h"

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

    int shift;

    static constexpr int MIN_SYMBOL = 32;
    static constexpr int MAX_SYMBOL = 126;
    static constexpr int RANGE = MAX_SYMBOL - MIN_SYMBOL + 1;

    enum class ShiftDirection
    {
        LEFT = -1,
        RIGHT = 1
    };

    char shiftSymbol(char c, ShiftDirection) const;
};


//class CaesarCreator : public CipherCreator
//{
//public:
//    CaesarCreator() : CipherCreator("caesar") {}
//
//    Cipher* create(const std::vector<std::string>& a) const override;
//};