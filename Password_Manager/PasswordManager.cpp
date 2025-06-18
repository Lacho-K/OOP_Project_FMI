#include "PasswordManager.h"
#include <fstream>
#include "CipherFactory.h"

PasswordManager::PasswordManager() : _cipher(nullptr) {}

void PasswordManager::create(const std::string& file, Cipher* cipher, const std::string& password)
{
    _filename = file;
    _cipher = cipher;
    _password = password;
    _entries.clear();
    saveToFile();
}

void PasswordManager::open(const std::string& file, const std::string& password)
{
    _filename = file;
    _password = password;

    std::ifstream in(_filename.c_str(), std::ios::binary);
    if (!in)
        throw std::runtime_error("Cannot open file");

    int passLen;
    in.read(reinterpret_cast<char*>(&passLen), sizeof(passLen));
    std::string encryptedPass(passLen, ' ');
    in.read(&encryptedPass[0], passLen);

    int cipherNameLen;
    in.read(reinterpret_cast<char*>(&cipherNameLen), sizeof(cipherNameLen));
    std::string cipherName(cipherNameLen, ' ');
    in.read(&cipherName[0], cipherNameLen);

    Cipher* cipher = CipherFactory::getInstance().createCipher(cipherName, {});
    if (!cipher)
        throw std::runtime_error("Unknown cipher");

    cipher->readConfig(in);

    std::string decrypted = cipher->decrypt(encryptedPass);
    if (decrypted != _password)
    {
        delete cipher;
        throw std::runtime_error("Incorrect password");
    }

    _cipher = cipher;

    int count;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));
    _entries.clear();

    for (int i = 0; i < count; i++)
    {
        int wlen, ulen, plen;
        in.read(reinterpret_cast<char*>(&wlen), sizeof(wlen));
        std::string w(wlen, ' ');
        in.read(&w[0], wlen);

        in.read(reinterpret_cast<char*>(&ulen), sizeof(ulen));
        std::string u(ulen, ' ');
        in.read(&u[0], ulen);

        in.read(reinterpret_cast<char*>(&plen), sizeof(plen));
        std::string p(plen, ' ');
        in.read(&p[0], plen);

        PasswordEntry e = { w, u, p };
        _entries.push_back(e);
    }
}

void PasswordManager::savePassword(const std::string& website, const std::string& user, const std::string& password)
{
    PasswordEntry entry;
    entry.website = website;
    entry.user = user;
    entry.encoded = _cipher->encrypt(password);
    _entries.push_back(entry);
    saveToFile();
}

std::string PasswordManager::loadPassword(const std::string& website, const std::string& user) const
{
    for (int i = 0; i < _entries.size(); i++)
    {
        if (_entries[i].website == website && _entries[i].user == user)
            return _cipher->decrypt(_entries[i].encoded);
    }
    return "";
}

std::vector<PasswordEntry> PasswordManager::loadAllForSite(const std::string& website) const
{
    std::vector<PasswordEntry> result;
    for (int i = 0; i < _entries.size(); i++)
    {
        if (_entries[i].website == website)
            result.push_back(_entries[i]);
    }
    return result;
}

bool PasswordManager::updatePassword(const std::string& website, const std::string& user, const std::string& newPassword)
{
    for (int i = 0; i < _entries.size(); i++)
    {
        if (_entries[i].website == website && _entries[i].user == user)
        {
            std::string current = _cipher->decrypt(_entries[i].encoded);
            if (current == newPassword)
                return false;
            _entries[i].encoded = _cipher->encrypt(newPassword);
            saveToFile();
            return true;
        }
    }
    return false;
}

bool PasswordManager::deletePassword(const std::string& website, const std::string& user)
{
    for (int i = 0; i < _entries.size(); i++)
    {
        if (_entries[i].website == website && _entries[i].user == user)
        {
            _entries.erase(_entries.begin() + i);
            saveToFile();
            return true;
        }
    }
    return false;
}

bool PasswordManager::deleteWebsite(const std::string& website)
{
    bool found = false;
    for (int i = 0; i < _entries.size(); )
    {
        if (_entries[i].website == website)
        {
            _entries.erase(_entries.begin() + i);
            found = true;
        }
        else
        {
            i++;
        }
    }
    if (found) saveToFile();
    return found;
}

void PasswordManager::saveToFile() const
{
    std::ofstream out(_filename.c_str(), std::ios::binary);

    std::string encryptedPass = _cipher->encrypt(_password);
    int passLen = encryptedPass.length();
    out.write(reinterpret_cast<const char*>(&passLen), sizeof(passLen));
    out.write(encryptedPass.c_str(), passLen);

    std::string cipherName = _cipher->name();
    int cipherNameLen = cipherName.length();
    out.write(reinterpret_cast<const char*>(&cipherNameLen), sizeof(cipherNameLen));
    out.write(cipherName.c_str(), cipherNameLen);

    _cipher->writeConfig(out);

    int count = _entries.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));
    for (int i = 0; i < count; i++)
    {
        int wlen = _entries[i].website.length();
        int ulen = _entries[i].user.length();
        int plen = _entries[i].encoded.length();

        out.write(reinterpret_cast<const char*>(&wlen), sizeof(wlen));
        out.write(_entries[i].website.c_str(), wlen);

        out.write(reinterpret_cast<const char*>(&ulen), sizeof(ulen));
        out.write(_entries[i].user.c_str(), ulen);

        out.write(reinterpret_cast<const char*>(&plen), sizeof(plen));
        out.write(_entries[i].encoded.c_str(), plen);
    }
}
