#include "PasswordManager.h"
#include "CipherFactory.h"
#include <fstream>

PasswordManager::PasswordManager() : _cipher(nullptr)
{
}

void PasswordManager::create(const std::string& file, Cipher* cipher, const std::string& password)
{
    if (cipher == nullptr)
    {
        throw std::invalid_argument("Null cipher pointer passed to create()");
    }

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
    {
        throw std::runtime_error("Cannot open file: " + _filename);
    }

    size_t passLen = 0;
    in.read(reinterpret_cast<char*>(&passLen), sizeof(passLen));


    std::string encryptedPass(passLen, ' ');
    in.read(&encryptedPass[0], passLen);

    size_t cipherNameLen = 0;
    in.read(reinterpret_cast<char*>(&cipherNameLen), sizeof(cipherNameLen));

    std::string cipherName(cipherNameLen, ' ');
    in.read(&cipherName[0], cipherNameLen);

    Cipher* cipher = CipherFactory::getInstance().createCipherFromStream(cipherName, in);

    if (cipher == nullptr)
    {
        throw std::runtime_error("Unknown cipher id: " + cipherName);
    }

    std::string decrypted = cipher->decrypt(encryptedPass);

    if (decrypted != _password)
    {
        delete cipher;
        throw std::invalid_argument("Incorrect file password");
    }

    if (_cipher != nullptr)
    {
        delete _cipher;
    }

    _cipher = cipher;

    _countPos = in.tellg();

    size_t count = 0;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));

    _entries.clear();

    for (size_t i = 0; i < count; ++i)
    {
        size_t wlen = 0;
        size_t ulen = 0;
        size_t plen = 0;

        in.read(reinterpret_cast<char*>(&wlen), sizeof(wlen));
        std::string w(wlen, ' ');
        in.read(&w[0], wlen);

        in.read(reinterpret_cast<char*>(&ulen), sizeof(ulen));
        std::string u(ulen, ' ');
        in.read(&u[0], ulen);

        in.read(reinterpret_cast<char*>(&plen), sizeof(plen));
        std::string p(plen, ' ');
        in.read(&p[0], plen);

        PasswordEntry e;
        e.website = w;
        e.user = u;
        e.encoded = p;

        _entries.push_back(e);
    }
}

// актуализира count и добавя само в края
void PasswordManager::savePassword(const std::string& website, const std::string& user, const std::string& password)
{
    validateFile();

    for (size_t i = 0; i < _entries.size(); ++i)
    {
        if (_entries[i].website == website && _entries[i].user == user)
        {
            throw std::runtime_error("Entry for website \"" + website + "\" and user \"" + user + "\" already exists. Use update instead.");
        }
    }

    PasswordEntry entry;
    entry.website = website;
    entry.user = user;
    entry.encoded = _cipher->encrypt(password);
    _entries.push_back(entry);

    // отваряме с този режим за да не изтрием вече записаната информация
    std::fstream out(_filename.c_str(),
        std::ios::in |
        std::ios::out |
        std::ios::binary |
        std::ios::ate);

    // true параметър за да акруализираме count
    appendEntryToFile(out, entry, true);
}

std::string PasswordManager::loadPassword(const std::string& website, const std::string& user) const
{
    validateFile();

    for (size_t i = 0; i < _entries.size(); ++i)
    {
        if (_entries[i].website == website && _entries[i].user == user)
        {
            return _cipher->decrypt(_entries[i].encoded);
        }
    }

    throw std::runtime_error("No entry for site \"" + website + "\" and user \"" + user + "\"");
}

std::vector<PasswordEntry> PasswordManager::loadAllForSite(const std::string& website) const
{
    validateFile();

    std::vector<PasswordEntry> result;

    for (size_t i = 0; i < _entries.size(); ++i)
    {
        if (_entries[i].website == website)
        {
            result.push_back(_entries[i]);
        }
    }

    if (result.empty())
    {
        throw std::runtime_error("No entries for site \"" + website + "\"");
    }

    return result;
}

bool PasswordManager::updatePassword(const std::string& website, const std::string& user, const std::string& newPassword)
{
    validateFile();

    for (size_t i = 0; i < _entries.size(); ++i)
    {
        if (_entries[i].website == website && _entries[i].user == user)
        {
            std::string current = _cipher->decrypt(_entries[i].encoded);

            if (current == newPassword)
            {
                throw std::invalid_argument("New password is identical to the existing one");
            }

            _entries[i].encoded = _cipher->encrypt(newPassword);
            saveToFile();
            return true;
        }
    }

    throw std::runtime_error("No entry to update for site \"" + website + "\" and user \"" + user + "\"");
}

bool PasswordManager::deletePassword(const std::string& website, const std::string& user)
{
    validateFile();

    for (size_t i = 0; i < _entries.size(); ++i)
    {
        if (_entries[i].website == website && _entries[i].user == user)
        {
            _entries.erase(_entries.begin() + i);
            saveToFile();
            return true;
        }
    }

    throw std::runtime_error("No entry to delete for site \"" + website + "\" and user \"" + user + "\"");
}

bool PasswordManager::deleteWebsite(const std::string& website)
{
    validateFile();

    bool removed = false;

    for (size_t i = 0; i < _entries.size();)
    {
        if (_entries[i].website == website)
        {
            _entries.erase(_entries.begin() + i);
            removed = true;
        }
        else
        {
            ++i;
        }
    }

    if (!removed)
    {
        throw std::runtime_error("No entries found for site \"" + website + "\"");
    }

    saveToFile();
    return true;
}

void PasswordManager::validateFile() const
{
    if (_cipher == nullptr)
    {
        throw std::runtime_error("No file opened or created");
    }
}

void PasswordManager::saveToFile()
{
    if (_cipher == nullptr)
    {
        throw std::runtime_error("Cannot save: no cipher loaded");
    }

    std::ofstream out(_filename.c_str(), std::ios::binary);

    if (!out)
    {
        throw std::runtime_error("Cannot write to file");
    }

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

    for (size_t i = 0; i < count; ++i)
    {
        appendEntryToFile(out, _entries[i]);
    }
}

void PasswordManager::appendEntryToFile(std::ostream& out, const PasswordEntry& entry, bool updateCount)
{
    if (updateCount)
    {
        std::streampos cur = out.tellp();
        out.seekp(_countPos);

        size_t cnt = _entries.size();
        out.write(reinterpret_cast<const char*>(&cnt), sizeof(cnt));

        out.seekp(cur);
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
