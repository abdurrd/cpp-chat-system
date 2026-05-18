#include "FileManager.hpp"

namespace fs = std::filesystem;

FileManager::FileManager(std::shared_ptr<std::queue<ChatInfo>> chats)
        :
        _chats(chats)

{
        fs::create_directories(group_path_prefix);
        fs::create_directories(chat_path_prefix);
}

fs::path FileManager::make_path(std::string hash, Type type) {
        std::string path_str;

        switch(type) {
                case Type::GROUP: path_str = group_path_prefix; break;
                case Type::CHAT: path_str = chat_path_prefix; break;
        }

        path_str += hash + ".txt";

        fs::path file_path = path_str;

        return file_path;
} 
