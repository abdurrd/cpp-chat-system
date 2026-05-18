#include "ChatInfo.hpp"

namespace fs = std::filesystem;

ChatInfo::ChatInfo(fs::path chat_path, std::string chat_hash, std::string name) 
        :
        _chat_path(chat_path),
        _chat_hash(chat_hash),
        _name(name)
{}

fs::path ChatInfo::getPath() const {
        return _chat_path;
}

std::string ChatInfo::getHash() const {
        return _chat_hash;
}

std::string ChatInfo::getName() const {
        return _name;
}
