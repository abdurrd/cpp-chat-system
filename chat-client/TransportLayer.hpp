#include "../Protocol.hpp"
#include <string>
#include <sys/socket.h>
#include <vector>

class TransportLayer {
        std::string username;
        int server_fd;
        std::string payload;

        inline std::string proto_to_s(Protocol proto) {
                return std::to_string(static_cast<int>(proto));
        }

        inline void send_payload() {
                if(payload.length() == 0) return;

                send(server_fd, payload.c_str(), payload.length(), 0);
                payload.clear();
        }

public:
        TransportLayer(std::string user, int serv_fd) : username(user), server_fd(serv_fd) {}

        void create_user(std::string &password) {
                if(password.length() < 8 || password.length() > 16) 
                        return;
                payload = 
                        proto_to_s(Protocol::REGISTER) + " "
                        + username + " "
                        + password;

                send_payload();
                return;
        }

        void login_user(std::string &password) {
                if(password.length() < 8 || password.length() > 16) 
                        return;
                payload = 
                        proto_to_s(Protocol::LOGIN) + " "
                        + username + " " 
                        + password;

                send_payload();
                return;
        }

        void send_message(int grp_hash, std::string &message) {
                if(message.length() == 0) return;
                // sender + group + message
                payload = 
                        proto_to_s(Protocol::MESSAGE) + " " 
                        + username + " " 
                        + std::to_string(grp_hash) + " " 
                        + message; 

                send_payload();
                return;
        }

        void create_group(std::string grp_name, int mem_num, std::vector<std::string> members) { 
                if(mem_num < 1) return;
                if(grp_name.length() != 0) return;
                
                payload = 
                        proto_to_s(Protocol::CREATE_GROUP) + " " 
                        + grp_name + " "
                        + std::to_string(mem_num); //the user themselves will be added by the server
                                                   //as it is preferable to minimise bytes that
                                                   //need to be transported
                                        
                for(auto &m : members) {
                        payload += " " + m;
                }

                send_payload();
                return;
        }


};
