#include "ProtocalExpections.hpp"

ProtocalExpection::ProtocalExpection(Protocol proto) : _proto(proto) {}

const char* ProtocalExpection::what() const noexcept{
        switch(_proto) {
                case Protocol::REGISTER: _err_msg = "REGISTER"; break; 
                case Protocol::LOGIN: _err_msg = "LOGIN";break;
                case Protocol::MESSAGE: _err_msg = "MESSAGE";break;
                case Protocol::CREATE_GROUP: _err_msg = "CREATE_GROUP";break;
                case Protocol::DELETE_GROUP: _err_msg = "DELETE_GROUP";break;

                case Protocol::USER_NAME_TAKEN: _err_msg = "USER_NAME_TAKEN";break;
                case Protocol::USER_NOT_FOUND: _err_msg = "USER_NOT_FOUND";break;
                case Protocol::WRG_PASSWORD: _err_msg = "WRG_PASSWORD";break;
                case Protocol::INVALID_USERS: _err_msg = "INVALID_USERS";break;
        }

        _err_msg = "Error Occured in Protocol::" + _err_msg;

        return _err_msg.c_str();
}

