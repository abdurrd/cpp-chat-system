#pragma once
#include <sstream>
#include "Protocal.hpp"

class SessionHandler;

class ProtocalHandler {

public:
        virtual ~ProtocalHandler() = default;
        virtual void handle_payload(std::istringstream &payload, SessionHandler &session) = 0;
        static void sendError(Protocal, int);
};
 
class RegestrationHandler: public ProtocalHandler {

public:
        void handle_payload(std::istringstream &payload, SessionHandler &session) override;
};

class LoginHandler: public ProtocalHandler {

public:
        void handle_payload(std::istringstream &payload, SessionHandler &session) override;
};

class MesssageHandler: public ProtocalHandler {

public:
        void handle_payload(std::istringstream &payload, SessionHandler &session) override;
};

class CreateGroupHandler: public ProtocalHandler {

public:
        void handle_payload(std::istringstream &payload, SessionHandler &session) override;
};
