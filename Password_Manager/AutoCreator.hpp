#pragma once
#include "Cipher.h"
template<typename T>
class AutoCreator : public CipherCreator
{
public:
    AutoCreator() : CipherCreator(T::ID) {}

    virtual Cipher* createFromStream(std::istream& is) const override;
    virtual Cipher* createFromArgs(const std::vector<std::string>& args) const override;

};

template<typename T>
inline Cipher* AutoCreator<T>::createFromStream(std::istream& is) const
{
    Cipher* cipher = new T();
    cipher->readConfig(is);   
    return cipher;
}

template<typename T>
inline Cipher* AutoCreator<T>::createFromArgs(const std::vector<std::string>& args) const
{
    return T::makeFromArgs(args);
}
