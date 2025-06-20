#pragma once
#include "Cipher.h"

class HillCipher : public Cipher
{
public:
    static const std::string ID;
    static Cipher* makeFromArgs(const std::vector<std::string>& args);

    HillCipher();
    explicit HillCipher(const std::vector<std::vector<int>>& key);

    std::string encrypt(const std::string& plain) const override;
    std::string decrypt(const std::string& coded) const override;
    std::string name() const override;
    void writeConfig(std::ostream& out) const override;
    void readConfig(std::istream& in) override;

private:
    std::vector<std::vector<int>> _key;
    std::vector<std::vector<int>> _invKey;

    std::string process(const std::string& text, const std::vector<std::vector<int>>& matrix) const;

    static int modInverse(int a);
    static std::vector<std::vector<int>> parseKey(std::istream& in);

    std::vector<std::vector<int>> invertMatrix(const std::vector<std::vector<int>>& m) const;
    std::vector<std::vector<int>> adjugateMatrix(const std::vector<std::vector<int>>& m) const;
    std::vector<std::vector<int>> getMinor(const std::vector<std::vector<int>>& m, size_t skipRow, size_t skipCol) const;
    int determinant(const std::vector<std::vector<int>>& m) const;
};