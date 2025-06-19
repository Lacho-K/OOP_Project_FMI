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

    size_t passLen;
    in.read(reinterpret_cast<char*>(&passLen), sizeof(passLen));
    std::string encryptedPass(passLen, ' ');
    in.read(&encryptedPass[0], passLen);

    size_t cipherNameLen;
    in.read(reinterpret_cast<char*>(&cipherNameLen), sizeof(cipherNameLen));
    std::string cipherName(cipherNameLen, ' ');
    in.read(&cipherName[0], cipherNameLen);

    Cipher* cipher = CipherFactory::getInstance().createCipherFromStream(cipherName, in);
    if (!cipher)
        throw std::runtime_error("Unknown cipher");

    std::string decrypted = cipher->decrypt(encryptedPass);
    if (decrypted != _password)
    {
        delete cipher;
        throw std::invalid_argument("Incorrect password");
    }

    if (_cipher)
    {
        delete _cipher;
        _cipher = nullptr;
    }

    _cipher = cipher;

    size_t count;

    _countPos = in.tellg();

    in.read(reinterpret_cast<char*>(&count), sizeof(count));

    _entries.clear();

    for (int i = 0; i < count; i++)
    {
        size_t wlen, ulen, plen;
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

    //ползваме fstream за да не загубим вече записаната информация
    std::fstream out(_filename.c_str(), std::ios::in | std::ios::out | std::ios::binary);
    appendEntryToFile(out, entry, true);
}

std::string PasswordManager::loadPassword(const std::string& website, const std::string& user) const
{
    for (int i = 0; i < _entries.size(); i++)
    {
        if (_entries[i].website == website && _entries[i].user == user)
            return "Password: " + _cipher->decrypt(_entries[i].encoded);
    }
    return "User: " + user + " not found in Website: " + website;
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

void PasswordManager::saveToFile()
{
    std::ofstream out(_filename.c_str(), std::ios::binary);

    std::string encryptedPass = _cipher->encrypt(_password);
    size_t passLen = encryptedPass.length();
    out.write(reinterpret_cast<const char*>(&passLen), sizeof(passLen));
    out.write(encryptedPass.c_str(), passLen);

    std::string cipherName = _cipher->name();
    size_t cipherNameLen = cipherName.length();
    out.write(reinterpret_cast<const char*>(&cipherNameLen), sizeof(cipherNameLen));
    out.write(cipherName.c_str(), cipherNameLen);

    _cipher->writeConfig(out);

    size_t count = _entries.size();

    _countPos = out.tellp();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (int i = 0; i < count; i++)
    {
        appendEntryToFile(out, _entries[i]);
    }
}

void PasswordManager::appendEntryToFile(std::ostream& out, const PasswordEntry& entry, bool updateCount)
{
    if (updateCount)
    {
        std::streampos currentPos = out.tellp();
        out.seekp(_countPos);
        size_t currentCount = _entries.size();
        out.write(reinterpret_cast<const char*>(&currentCount), sizeof(currentCount));
        out.seekp(currentPos);
    }


    size_t wlen = entry.website.length();
    size_t ulen = entry.user.length();
    size_t plen = entry.encoded.length();

    out.write(reinterpret_cast<const char*>(&wlen), sizeof(wlen));
    out.write(entry.website.c_str(), wlen);

    out.write(reinterpret_cast<const char*>(&ulen), sizeof(ulen));
    out.write(entry.user.c_str(), ulen);

    out.write(reinterpret_cast<const char*>(&plen), sizeof(plen));
    out.write(entry.encoded.c_str(), plen);
}

