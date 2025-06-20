#pragma once
#include "Cipher.h"

class CaesarCipher : public Cipher
{
public:
    static const std::string ID;

    static Cipher* makeFromArgs(const std::vector<std::string>& args);

    explicit CaesarCipher(int shift);

    CaesarCipher();

    std::string encrypt(const std::string& plain) const override;
    std::string decrypt(const std::string& coded) const override;
    std::string name() const override;
    void writeConfig(std::ostream& out) const override;
    void readConfig(std::istream& in) override;

private:

    int shift;

    enum class ShiftDirection
    {
        LEFT = -1,
        RIGHT = 1
    };

    char shiftSymbol(char c, ShiftDirection dir) const;
};