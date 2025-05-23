#pragma once
#include "Cipher.h"
#include <vector>
#include <string>

class HillCipher : public Cipher
{
public:
    explicit HillCipher(const std::vector< std::vector<int> >& key);
    std::string encrypt(const std::string& plain) override;
    std::string decrypt(const std::string& coded) override;
private:
    std::vector< std::vector<int> > key;
    std::vector< std::vector<int> > invKey;
};

class HillCreator : public CipherCreator
{
public:
    HillCreator() : CipherCreator("hill") {}

    Cipher* create(const std::vector<std::string>& a) const override
    {
        // тук се очаква квадратна матрица, подадена като поредица от числа
        // парсването е оставено за имплементация
        std::vector< std::vector<int> > key; // TODO
        return new HillCipher(key);
    }
};

//трябва да е в cpp файла
static HillCreator __;