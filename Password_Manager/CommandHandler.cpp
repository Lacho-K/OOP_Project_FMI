#include "CommandHandler.h"
#include <iostream>

static std::vector<std::string> split(const std::string& line)
{
    std::vector<std::string> parts;
    std::string current;

    for (size_t i = 0; i < line.size(); i++)
    {
        char c = line[i];

        if (c == ' ' || c == '\t')
        {
            if (!current.empty())
            {
                parts.push_back(current);
                current.clear();
            }
        }
        else
        {
            current += c;
        }
    }

    if (!current.empty())
    {
        parts.push_back(current);
    }

    return parts;
}

void CommandHandler::run()
{
    std::string line;

    while (true)
    {
        std::cout << ">> ";

        char c;
        line.clear();
        while (std::cin.get(c) && c != '\n')
        {
            line += c;
        }

        if (line.empty())
        {
            break;
        }

        executeCommand(line);
    }
}

void CommandHandler::executeCommand(const std::string& line)
{
    std::vector<std::string> args = split(line);

    if (args.empty())
    {
        return;
    }

    std::string cmd = args[0];
    args.erase(args.begin());

    try
    {
        if (cmd == "create")
        {
            handleCreate(args);
        }
        else if (cmd == "open")
        {
            handleOpen(args);
        }
        else if (cmd == "save")
        {
            handleSave(args);
        }
        else if (cmd == "load")
        {
            handleLoad(args);
        }
        else if (cmd == "update")
        {
            handleUpdate(args);
        }
        else if (cmd == "delete")
        {
            handleDelete(args);
        }
        else
        {
            std::cout << "Unknown command\n";
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << '\n';
    }
}


void CommandHandler::handleCreate(const std::vector<std::string>& a)
{
    if (a.size() < 3)
    {
        std::cout << "Usage: create <file> <cipher> <cipher-args...> <file-pass>\n";
        return;
    }

    std::string file = a[0];
    std::string cipherId = a[1];
    std::string master = a.back();

    std::vector<std::string> cipherArgs;

    for (size_t i = 2; i < a.size() - 1; ++i)
    {
        cipherArgs.push_back(a[i]);
    }

    Cipher* cipher = CipherFactory::getInstance().createCipherFromArgs(cipherId, cipherArgs);

    if (cipher == nullptr)
    {
        std::cout << "Unknown cipher\n";
        return;
    }

    manager.create(file, cipher, master);
    std::cout << "Created " << file << '\n';
}

void CommandHandler::handleOpen(const std::vector<std::string>& a)
{
    if (a.size() != 2)
    {
        std::cout << "Usage: open <file> <file-pass>\n";
        return;
    }

    manager.open(a[0], a[1]);
    std::cout << "Opened " << a[0] << '\n';
}

void CommandHandler::handleSave(const std::vector<std::string>& a)
{
    if (a.size() != 3)
    {
        std::cout << "Usage: save <site> <user> <pass>\n";
        return;
    }

    manager.savePassword(a[0], a[1], a[2]);
    std::cout << "Saved.\n";
}

void CommandHandler::handleLoad(const std::vector<std::string>& a)
{
    if (a.empty())
    {
        std::cout << "Usage: load <site> [user]\n";
        return;
    }

    if (a.size() == 2)
    {
        std::string pass = manager.loadPassword(a[0], a[1]);
        std::cout << pass << '\n';
    }
    else
    {
        std::vector<PasswordEntry> list = manager.loadAllForSite(a[0]);

        if (list.empty())
        {
            std::cout << "No entries for " << a[0] << '\n';
            return;
        }

        for (size_t i = 0; i < list.size(); ++i)
        {
            std::string original = manager.loadPassword(list[i].website, list[i].user);
            std::cout << list[i].user << " -> " << original << '\n';
        }
    }
}

void CommandHandler::handleUpdate(const std::vector<std::string>& a)
{
    if (a.size() != 3)
    {
        std::cout << "Usage: update <site> <user> <new-pass>\n";
        return;
    }

    manager.updatePassword(a[0], a[1], a[2]);
    std::cout << "Updated.\n";
}

void CommandHandler::handleDelete(const std::vector<std::string>& a)
{
    if (a.empty())
    {
        std::cout << "Usage: delete <site> [user]\n";
        return;
    }

    if (a.size() == 1)
    {
        manager.deleteWebsite(a[0]);
        std::cout << "Deleted all entries for " << a[0] << '\n';
    }
    else
    {
        manager.deletePassword(a[0], a[1]);
        std::cout << "Deleted.\n";
    }
}
