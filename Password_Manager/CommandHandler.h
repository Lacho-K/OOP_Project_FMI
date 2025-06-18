#pragma once

#include "PasswordManager.h"
#include "CipherFactory.h"

class CommandHandler
{
public:
    CommandHandler();
    void run();

private:
    PasswordManager manager;

    void executeCommand(const std::string& line);

    void handleCreate(const std::vector<std::string>& args);
    void handleOpen(const std::vector<std::string>& args);
    void handleSave(const std::vector<std::string>& args);
    void handleLoad(const std::vector<std::string>& args);
    void handleUpdate(const std::vector<std::string>& args);
    void handleDelete(const std::vector<std::string>& args);
};