#pragma once

#include <filesystem>

namespace fs = std::filesystem;

class ChatInfo {

        fs::path _chat_path;
        std::string _chat_hash = "-1";
        std::string _name;

public:
        ChatInfo() = default;
        ChatInfo(fs::path chat_path, std::string chat_hash, std::string name);

        fs::path getPath() const;
        std::string getHash() const;
        std::string getName() const;
};
