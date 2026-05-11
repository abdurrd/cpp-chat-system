#pragma once
#include <iostream>

const char FIELD_SEP  = '\x1F';  
const char RECORD_SEP = '\x1E';  
const char GROUP_SEP  = '\x1D'; 

enum class Protocol {
        REGISTER,
        LOGIN,
        MESSAGE,
        CREATE_GROUP,

        USER_NAME_TAKEN = -1,
        USER_NOT_FOUND = -2,
        WRG_PASSWORD = -3,
        INVALID_USERS = -4,
};

inline std::istream &operator>>(std::istream &is, Protocol &proto) {
        int i_proto;
        is >> i_proto;
        proto = static_cast<Protocol>(i_proto);

        return is;
}

inline std::ostream &operator<<(std::ostream &os, Protocol &proto) {

        std::string s_proto;
        switch(proto) {
                case Protocol::REGISTER: s_proto = "REGISTER"; break; 
                case Protocol::LOGIN: s_proto = "LOGIN";break;
                case Protocol::MESSAGE: s_proto = "MESSAGE";break;
                case Protocol::CREATE_GROUP: s_proto = "CREATE_GROUP";break;

                case Protocol::USER_NAME_TAKEN: s_proto = "USER_NAME_TAKEN";break;
                case Protocol::USER_NOT_FOUND: s_proto = "USER_NOT_FOUND";break;
                case Protocol::WRG_PASSWORD: s_proto = "WRG_PASSWORD";break;
                case Protocol::INVALID_USERS: s_proto = "INVALID_USERS";break;
        }

        os << "Protocal::" << s_proto << "(" << static_cast<int>(proto) << ")";

        return os;
}
