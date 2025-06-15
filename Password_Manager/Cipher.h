#pragma once
#include <string>
#include <vector>

class Cipher
{
public:
    virtual std::string encrypt(const std::string& plain) const = 0;
    virtual std::string decrypt(const std::string& coded) const = 0;
    virtual std::string name() const = 0;
    virtual void writeConfig(std::ostream& out) const = 0;
    virtual void readConfig(std::istream& in) = 0;
    virtual ~Cipher() = default;
};

class CipherCreator
{
public:
    explicit CipherCreator(std::string name);
    virtual Cipher* create(const std::vector<std::string>& args) const = 0;
    const std::string& getName() const;
    virtual ~CipherCreator() = default;


private:
    std::string _name;
};

