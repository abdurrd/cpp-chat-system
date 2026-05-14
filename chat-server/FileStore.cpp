#include "FileStore.hpp"
#include <filesystem>
#include <mutex>
#include <unordered_set>
#include <unistd.h>
#include <fcntl.h>
#include "../Protocol.hpp"

#include <iostream>


void FileStore::copy_group_data(std::string hash, std::string &buf) {
        buf += hash;                                                                                           
        buf += FIELD_SEP;

        buf += read_to_eof(make_path(hash, Type::GROUP));
        buf += FIELD_SEP;
        buf += read_to_eof(make_path(hash, Type::CHAT));
        buf += FIELD_SEP;
}

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

int FileStore::open_new_chat_fd(std::string &chat_hash){
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

        int group_no = 0;
        std::string group_data;
        std::string hash;
        while(std::getline(user_data, hash)) {
                copy_group_data(hash, group_data);
                group_data += GROUP_SEP;

                ++group_no;
        }
        lock.unlock();

        group_data = std::to_string(group_no) + GROUP_SEP + group_data;
        group_data += '\0';

        //group_no
        //\n\n
        //group_hash
        //~~~
        //group_name
        //group members
        //~~~
        //chat_data
        //\n\n
        
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

int FileStore::create_group(std::string &grp_name, std::string &username, std::vector<std::string> &members) {
        std::fstream gh_file(group_hash_path, std::ios::in | std::ios::out);

        std::unordered_set<std::string> valid_members;
        for(std::string user: members) {
                if(check_user(user) != std::nullopt) valid_members.insert(user);
        }

        if(valid_members.size() < 1) return 0;

        std::unique_lock<std::mutex> lock(_group_mutex);
        int group_hash;
        gh_file >> group_hash;
        ++group_hash;

        fs::path new_grp_path = make_path(std::to_string(group_hash), Type::GROUP);
        fs::path new_cht_path = make_path(std::to_string(group_hash), Type::CHAT);

        std::unique_lock<std::mutex> lock_chat(_chat_mutex);
        std::ofstream new_chat(new_cht_path);
        lock_chat.unlock();

        std::ofstream new_grp_file(new_grp_path);
        
        new_grp_file << grp_name << "\n";
        new_grp_file << username << "\n";

        for(std::string user: valid_members) {
                new_grp_file << user << "\n";
        }

        fs::path cur_path(make_path(username, Type::USER));
        std::ofstream cur_file(cur_path, std::ios::app);
        cur_file << group_hash << "\n";

        for(std::string user: valid_members) {
                fs::path user_path(make_path(user, Type::USER));
                std::ofstream user_file(user_path, std::ios::app);
                user_file << group_hash << "\n";
        }

        gh_file.close();

        std::ofstream rewrite(group_hash_path, std::ios::trunc);
        rewrite << group_hash;

        lock.unlock();
        return valid_members.size()+1;
}
