#pragma once
#include <sstream>
#include <string>
#include "ProtocalHandeler.hpp"
#include "SessionHandler.hpp"
#include <string>

void ProtocalHandler::sendError(Protocal error_code, int clientfd) {
        std::string err = std::to_string((int)error_code);
        send(clientfd, err.c_str(), err.length(), 0);
}
 
void RegestrationHandler::handle_payload(std::istringstream &payload, SessionHandler &session) {
        auto client = session.getClient();

        std::string username, password;
        payload >> username >> password;

        auto result = FileStore::instance().check_user(username);

        if(result) {
                sendError(Protocal::USER_NAME_TAKEN, client._fd);
                return;
        }

        FileStore::instance().make_new_user(username, password);

        char success_message[] = "1 User_created!\0";
        send(client._fd, success_message, strlen(success_message), 0);

}

void LoginHandler::handle_payload(std::istringstream &payload, SessionHandler &session) {
        auto client = session.getClient();

        std::string u_username, u_password;
        payload >> u_username >> u_password;

        auto result = FileStore::instance().check_user(u_username);
        if(!result) {
                sendError(Protocal::USER_NOT_FOUND, client._fd);
                return;
        }
        
        std::string password = result.value();

        if(u_password != password) {
                sendError(Protocal::WRG_PASSWORD, client._fd);
                return;
        }

        session.setClientUsername(u_username);
        std::string group_data = FileStore::instance().get_group_data_payload(u_username);
        
        send(client._fd, group_data.c_str(), strlen(group_data.c_str()), 0);
        session.init_watcher();
        return;
}


void MesssageHandler::handle_payload(std::istringstream &payload, SessionHandler &session) {
        std::string sender, group, message;
        payload >> sender >> group;
        std::getline(payload, message);

        FileStore::instance().write_chat(sender, group, message);

        //no need to handle braodcast as this will update the chat file
        //which will be watched by all other live memebers on their
        //own thread
}


void CreateGroupHandler::handle_payload(std::istringstream &payload, SessionHandler &session) {
        std::string grp_name;
        int num_members;
        std::vector<std::string> users;

        payload >> grp_name;
        payload >> num_members;
        users = std::vector<std::string>(num_members);
        for(int i = 0; i < num_members; ++i) {
                payload >> users[i];
        }

        FileStore::instance().create_group(grp_name, users);

        return;
}
