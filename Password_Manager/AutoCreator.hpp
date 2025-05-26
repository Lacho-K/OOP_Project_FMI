#pragma once
#include "Cipher.h"
template<typename T>
class AutoCreator : public CipherCreator
{
public:
    AutoCreator() : CipherCreator(T::ID) {}

    Cipher* create(const std::vector<std::string>& args) const override
    {
        return T::makeFromArgs(args);
    }
};