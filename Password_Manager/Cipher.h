#pragma once
#include <string>
#include <vector>

class Cipher
{

public:

    virtual std::string encrypt(const std::string& plain) = 0;
    virtual std::string decrypt(const std::string& coded) = 0;
    virtual ~Cipher() = default;

};

class CipherCreator
{
public:
    explicit CipherCreator(std::string name);
    virtual ~CipherCreator() = default;

    virtual Cipher* create(const std::vector<std::string>& args) const = 0;
    const char* getName() const { return _name; }

private:
    const char* _name;
};

