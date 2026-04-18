#pragma once
enum class Protocal {
        REGISTER,
        LOGIN,
        MESSAGE,
        CREATE_GROUP,

        USER_NAME_TAKEN = -1,
        USER_NOT_FOUND = -2,
        WRG_PASSWORD = -3,
};
