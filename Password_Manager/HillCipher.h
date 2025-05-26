#pragma once
#include "Cipher.h"
#include <vector>
#include <string>

class HillCipher : public Cipher
{
public:
    static const std::string ID;

    static Cipher* makeFromArgs(const std::vector<std::string>& args);

    explicit HillCipher(const std::vector<std::vector<int>>& key);

    std::string encrypt(const std::string& plain) const override;
    std::string decrypt(const std::string& coded) const override;
    std::string name() const override;
    void writeConfig(std::ostream& out) const override;
    void readConfig(std::istream& in) override;

private:
    std::vector<std::vector<int>> _key;
    std::vector<std::vector<int>> _invKey;
};