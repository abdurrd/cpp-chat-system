#include <sstream>
#include <string>
#include "ProtocolHandeler.hpp"
#include "SessionHandler.hpp"
#include "FileStore.hpp"
#include "ProtocalExpections.hpp"
#include <string>

void ProtocolHandler::sendError(Protocol error_code, int clientfd) {
        std::string err = std::to_string((int)error_code);
        send(clientfd, err.c_str(), err.length(), 0);
}
 
void RegestrationHandler::handle_payload(std::istringstream &payload, SessionHandler &session) {
        std::cout << "Signup proto: " << payload.str() << "\n";
        auto client = session.getClient();

        std::string username, password;
        payload >> username >> password;

        auto result = FileStore::instance().check_user(username);

        if(result) {
                sendError(Protocol::USER_NAME_TAKEN, client._fd);
                throw ProtocalExpection(ProtocalExpection(Protocol::USER_NAME_TAKEN));
                return;
        }

        FileStore::instance().make_new_user(username, password);
        session.setClientUsername(username);

        std::string success_message = "0";
        send(client._fd, success_message.c_str(), strlen(success_message.c_str()), 0);

}

void LoginHandler::handle_payload(std::istringstream &payload, SessionHandler &session) {
        std::cout << "Login proto: " << payload.str() << "\n";
        auto client = session.getClient();


        std::string u_username, u_password;
        payload >> u_username >> u_password;

        auto result = FileStore::instance().check_user(u_username);
        if(!result) {
                std::cout << "Not found" << "\n";
                sendError(Protocol::USER_NOT_FOUND, client._fd);
                throw ProtocalExpection(ProtocalExpection(Protocol::USER_NOT_FOUND));

                return;
        }
        
        std::string password = result.value();

        if(u_password != password) {
                sendError(Protocol::WRG_PASSWORD, client._fd);
                throw ProtocalExpection(ProtocalExpection(Protocol::WRG_PASSWORD));
                return;
        }

        session.setClientUsername(u_username);
        std::string group_data = "1\n";
        group_data += FileStore::instance().get_group_data_payload(u_username);
        
        send(client._fd, group_data.c_str(), strlen(group_data.c_str()), 0);
        session.init_watcher();
        return;
}


void MesssageHandler::handle_payload(std::istringstream &payload, SessionHandler &session) {
        std::cout << "MesssageHandler: " << payload.str() << "\n";
        std::string sender, group, message;
        payload >> sender >> group;
        std::getline(payload, message);

        FileStore::instance().write_chat(sender, group, message);

        //no need to handle braodcast as this will update the chat file
        //which will be watched by all other live memebers on their
        //own thread
}


void CreateGroupHandler::handle_payload(std::istringstream &payload, SessionHandler &session) {
        std::cout << "Create group handler: " << payload.str() << "\n";
        auto client = session.getClient();

        std::string grp_name;
        int num_members;
        std::vector<std::string> members;

        payload >> grp_name;
        payload >> num_members;
        members = std::vector<std::string>(num_members);
        for(int i = 0; i < num_members; ++i) {
                payload >> members[i];
        }

        int added = FileStore::instance().create_group(grp_name, client._username, members);

        if(added < num_members) sendError(Protocol::INVALID_USERS, client._fd);
        throw ProtocalExpection(ProtocalExpection(Protocol::INVALID_USERS));

        return;
}
