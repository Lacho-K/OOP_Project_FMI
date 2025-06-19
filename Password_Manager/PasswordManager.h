    #pragma once

#include "Cipher.h"
#include "PasswordEntry.h"

class PasswordManager
{
public:
    PasswordManager();

    void create(const std::string& file, Cipher* cipher, const std::string& password);
    void open(const std::string& file, const std::string& password);

    void savePassword(const std::string& website, const std::string& user, const std::string& password);
    std::string loadPassword(const std::string& website, const std::string& user) const;
    std::vector<PasswordEntry> loadAllForSite(const std::string& website) const;

    bool updatePassword(const std::string& website, const std::string& user, const std::string& newPassword);
    bool deletePassword(const std::string& website, const std::string& user);
    bool deleteWebsite(const std::string& website);

private:
    std::string _filename;
    std::string _password;
    Cipher* _cipher;
    std::vector<PasswordEntry> _entries;
    std::streampos _countPos;

    void loadFromFile();
    void saveToFile();
    void appendEntryToFile(std::ostream& out, const PasswordEntry& entry, bool append = false);
};