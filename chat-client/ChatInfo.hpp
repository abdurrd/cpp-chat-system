#pragma once

#include <filesystem>

namespace fs = std::filesystem;

class ChatInfo {

        fs::path _chat_path;
        std::string _chat_hash = "-1";
        std::string _name;

public:
        ChatInfo() = default;
        ChatInfo(fs::path chat_path, std::string chat_hash, std::string name) 
                :
                _chat_path(chat_path),
                _chat_hash(chat_hash),
                _name(name)
        {}

        fs::path getPath() const {
                return _chat_path;
        }

        std::string getHash() const {
                return _chat_hash;
        }

        std::string getName() const {
                return _name;
        }

};
