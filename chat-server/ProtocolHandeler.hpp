#pragma once
#include <sstream>
#include "../Protocol.hpp"

class SessionHandler;

class ProtocolHandler {

public:
        virtual ~ProtocolHandler() = default;
        virtual void handle_payload(std::istringstream &payload, SessionHandler &session) = 0; //need to make this a veriadic template since some accept sessions some dont
        static void sendError(Protocol, int);
};
 
class RegestrationHandler: public ProtocolHandler {

public:
        void handle_payload(std::istringstream &payload, SessionHandler &session) override;
};

class LoginHandler: public ProtocolHandler {

public:
        void handle_payload(std::istringstream &payload, SessionHandler &session) override;
};

class MesssageHandler: public ProtocolHandler {

public:
        void handle_payload(std::istringstream &payload, SessionHandler &session) override;
};

class CreateGroupHandler: public ProtocolHandler {

public:
        void handle_payload(std::istringstream &payload, SessionHandler &session) override;
};
