
#include <string>
#include <filesystem>
#include <vector>
#include <mutex>
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace fs = std::filesystem;

class FileStore {

        inline static std::mutex _user_mutex;
        inline static std::mutex _group_mutex;
        inline static std::mutex _chat_mutex;
        
        std::string user_path_prefix = "./server-data/users/";
        std::string group_path_prefix = "./server-data/groups/";
        std::string chat_path_prefix = "./server-data/chats/";

        fs::path group_hash_path = "./server-data/groups/hash_key.txt";

        enum Type {
                USER,
                GROUP,
                CHAT
        };
        
        fs::path make_path(std::string hash, Type type) {

                std::string path_str;

                switch(type) {
                        case Type::USER: path_str = user_path_prefix; break;
                        case Type::GROUP: path_str = group_path_prefix; break;
                        case Type::CHAT: path_str = chat_path_prefix; break;
                }

                path_str += hash + ".txt";

                fs::path file_path = path_str;

                return file_path;
        } 

        inline std::string read_to_eof(fs::path file_path){
                        std::ifstream file_data(file_path); //might optomise this later
                        std::stringstream file_contents;                                  
                        file_contents << file_data.rdbuf();

                        return file_contents.str();
        }



        FileStore() = default;

public:
        static FileStore &instance() {
                static FileStore fs;
                return fs;
        }

        void copy_group_data(std::string hash, std::string &buf);
        
        //getters
        std::mutex& get_user_mutex() const;
        std::mutex& get_chat_mutex() const;
        std::mutex& get_group_mutex() const;

        //for file watcher
        int open_user_fd(const std::string &username);
        void open_chat_fds(const std::string &username, std::vector<int>&, std::unordered_map<int,int>&);
        int open_new_chat_fd(std::string &chat_hash);
        //std::string broadcast_message_to_client(int chat_fd);

        //for protocals
        std::optional<std::string> check_user(std::string &username);
        void make_new_user(std::string &username, std::string &password);
        std::string get_group_data_payload(std::string &username);
        void write_chat(std::string &sender, std::string &group, std::string &message);
        int create_group(std::string &grp_name, std::string &username, std::vector<std::string> &members);

};
