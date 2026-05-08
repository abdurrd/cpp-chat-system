#include "FileStore.hpp"
#include <filesystem>
#include <unistd.h>
#include <fcntl.h>

#include <iostream>

//file watching
int FileStore::open_user_fd(const std::string &username){
        fs::path user_path = make_path(username, Type::USER);
        int user_fd = open(user_path.c_str(), O_RDONLY);
        lseek(user_fd, 0, SEEK_END);
        return user_fd;
}

void FileStore::open_chat_fds(const std::string &username, std::vector<int> &chat_fd, std::unordered_map<int,int> &cfd_to_hash) {
        fs::path user_path = make_path(username, Type::USER);

        std::vector<int> chat_hashs;
        {//just to discard
                std::ifstream user_file(user_path);

                std::string discard;
                std::getline(user_file, discard);

                std::string hash;
                while(std::getline(user_file, hash)) {
                        chat_hashs.push_back(std::stoi(hash));
                }
        }


        for(int hash: chat_hashs) {
                std::string chat_path = make_path(std::to_string(hash), Type::CHAT);
                chat_fd.push_back(open(chat_path.c_str(), O_RDONLY));
                cfd_to_hash[*--chat_fd.end()] = hash;
        }
        
        for(int cfd: chat_fd) {
                lseek(cfd, 0, SEEK_END);
        }
}

int FileStore::open_new_chat_fd(std::string &&chat_hash){
        fs::path chat_path = make_path(chat_hash, Type::CHAT);
        return open(chat_path.c_str(), O_RDONLY);
}

//std::string broadcast_message_to_client(int chat_fd);

//protocals
std::optional<std::string> FileStore::check_user(std::string &username) {
        fs::path user_path = make_path(username, Type::USER);
        if(!fs::exists(user_path)) return std::nullopt;

        std::ifstream user_file(user_path);
        std::string password;

        std::unique_lock<std::mutex> lock(_user_mutex);
        user_file >> password;

        lock.unlock();
        return password;
}

void FileStore::make_new_user(std::string &username, std::string &password) {
        fs::path user_path = make_path(username, Type::USER);

        std::unique_lock<std::mutex> lock(_user_mutex);
        std::ofstream new_user(user_path);
        new_user << password << "\n";

        lock.unlock();
        return;
}


std::string FileStore::get_group_data_payload(std::string &username){
        fs::path user_path = make_path(username, Type::USER);
        std::ifstream user_data(user_path);

        std::unique_lock<std::mutex> lock(_user_mutex);
        {
                std::string discard;
                std::getline(user_data, discard);
        }

        int group_no = -1;
        std::string group_data;
        std::string line;
        while(std::getline(user_data, line)) {
                group_data += line;
                group_data += '\n';

                group_data += read_to_eof(make_path(line, Type::GROUP));
                group_data += "%\n";
                group_data += read_to_eof(make_path(line, Type::CHAT));
                group_data += "%\n";

                ++group_no;
        }
        lock.unlock();

        group_data = std::to_string(group_no) + '\n' + group_data;
        group_data += '\0';
        
        return group_data;
}


void FileStore::write_chat(std::string &sender, std::string &group, std::string &message) {
        fs::path chat_path = make_path(group, Type::CHAT);

        std::ofstream chat_file(chat_path, std::ios::app);
        std::unique_lock<std::mutex> lock(_chat_mutex);

        chat_file << sender << " ";
        chat_file << message << "\n";

        lock.unlock();
        return;
}

void FileStore::create_group(std::string &grp_name, std::string &username, std::vector<std::string> &members) {
        std::fstream gh_file(group_hash_path, std::ios::in | std::ios::out);

        int group_hash;
        gh_file >> group_hash;
        ++group_hash;

        fs::path new_grp_path = make_path(std::to_string(group_hash), Type::GROUP);

        std::unique_lock<std::mutex> lock(_group_mutex);
        std::ofstream new_grp_file(new_grp_path);
        
        new_grp_file << grp_name << "\n";
        new_grp_file << username << "\n";

        for(std::string user: members) {
                if(check_user(user)) {
                        new_grp_file << user << "\n";
                }
        }

        gh_file.seekp(0);
        gh_file << group_hash;

        lock.unlock();
        return;
}
