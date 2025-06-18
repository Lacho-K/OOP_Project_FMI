#pragma once
#include "Cipher.h"
template<typename T>
class AutoCreator : public CipherCreator
{
public:
    AutoCreator() : CipherCreator(T::ID) {}

    Cipher* create(std::istream& is) const override;
};

template<typename T>
inline Cipher* AutoCreator<T>::create(std::istream& is) const
{
    Cipher* cipher = new T();
    cipher->readConfig(in);   
    return cipher;
}
