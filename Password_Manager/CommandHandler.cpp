#include "CommandHandler.h"
#include "AsciiUtils.h"
#include <iostream>

CommandHandler::CommandHandler() {}

void CommandHandler::run()
{
    std::string line;
    char buffer[1024];
    while (true)
    {
        std::cout << ">> ";
        if (!std::cin.getline(buffer, 1024))
            break;
        line = buffer;
        executeCommand(line);
    }
}

void CommandHandler::executeCommand(const std::string& line)
{
    std::vector<std::string> args;
    std::string current;
    for (int i = 0; i < line.length(); i++)
    {
        if (line[i] == ' ')
        {
            if (!current.empty())
            {
                args.push_back(current);
                current.clear();
            }
        }
        else
        {
            current += line[i];
        }
    }
    if (!current.empty())
        args.push_back(current);

    if (args.empty())
        return;

    std::string cmd = args[0];
    args.erase(args.begin());

    if (cmd == "create")
        handleCreate(args);
    else if (cmd == "open")
        handleOpen(args);
    else if (cmd == "save")
        handleSave(args);
    else if (cmd == "load")
        handleLoad(args);
    else if (cmd == "update")
        handleUpdate(args);
    else if (cmd == "delete")
        handleDelete(args);
    else
        std::cout << "Unknown command.\n";
}

void CommandHandler::handleCreate(const std::vector<std::string>& args)
{
    if (args.size() < 3)
    {
        std::cout << "Invalid usage.\n";
        return;
    }

    std::string file = args[0];
    std::string cipherName = args[1];

    std::vector<std::string> cipherArgs;
    for (int i = 2; i < args.size(); i++)
        cipherArgs.push_back(args[i]);

    Cipher* cipher = CipherFactory::getInstance().createCipher(cipherName, cipherArgs);
    if (!cipher)
    {
        std::cout << "Unknown cipher.\n";
        return;
    }
    manager.create(file, cipher, "");
    std::cout << "File created.\n";
}

void CommandHandler::handleOpen(const std::vector<std::string>& args)
{
    if (args.size() < 2)
    {
        std::cout << "Invalid usage.\n";
        return;
    }

    try
    {
        manager.open(args[0], args[1]);
        std::cout << "File opened.\n";
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void CommandHandler::handleSave(const std::vector<std::string>& args)
{
    if (args.size() < 3)
    {
        std::cout << "Invalid usage.\n";
        return;
    }
    manager.savePassword(args[0], args[1], args[2]);
    std::cout << "Password saved.\n";
}

void CommandHandler::handleLoad(const std::vector<std::string>& args)
{
    if (args.size() < 1)
    {
        std::cout << "Invalid usage.\n";
        return;
    }

    if (args.size() == 2)
    {
        std::string password = manager.loadPassword(args[0], args[1]);
        std::cout << "Password: " << password << "\n";
    }
    else
    {
        std::vector<PasswordEntry> list = manager.loadAllForSite(args[0]);
        for (int i = 0; i < list.size(); i++)
        {
            std::cout << list[i].user << " -> " << manager.loadPassword(list[i].website, list[i].user) << "\n";
        }
    }
}

void CommandHandler::handleUpdate(const std::vector<std::string>& args)
{
    if (args.size() < 3)
    {
        std::cout << "Invalid usage.\n";
        return;
    }

    if (manager.updatePassword(args[0], args[1], args[2]))
        std::cout << "Password updated.\n";
    else
        std::cout << "Update failed.\n";
}

void CommandHandler::handleDelete(const std::vector<std::string>& args)
{
    if (args.size() < 1)
    {
        std::cout << "Invalid usage.\n";
        return;
    }

    if (args.size() == 1)
    {
        if (manager.deleteWebsite(args[0]))
            std::cout << "Website deleted.\n";
        else
            std::cout << "Website not found.\n";
    }
    else
    {
        if (manager.deletePassword(args[0], args[1]))
            std::cout << "Password deleted.\n";
        else
            std::cout << "Password not found.\n";
    }
}
