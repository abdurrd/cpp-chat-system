CXX      := g++-15
CXXFLAGS := -std=c++23 -Wall -Wextra -I.

SERVER_DIR := chat-server
CLIENT_DIR := chat-client

SERVER_BIN := server
CLIENT_BIN := client

SERVER_SRCS := $(SERVER_DIR)/server-main.cpp \
               $(SERVER_DIR)/FileStore.cpp \
               $(SERVER_DIR)/ProtocolHandeler.cpp \
               $(SERVER_DIR)/SessionHandler.cpp \
               $(SERVER_DIR)/ProtocalExpections.cpp

CLIENT_SRCS := $(CLIENT_DIR)/client-main.cpp \
	       $(CLIENT_DIR)/TransportLayer.cpp \
	       $(CLIENT_DIR)/ChatInfo.cpp \
	       $(CLIENT_DIR)/FileManager.cpp \
	       $(CLIENT_DIR)/Reciever.cpp \
	       $(CLIENT_DIR)/Page.cpp \
	       $(CLIENT_DIR)/LoginPage.cpp \
	       $(CLIENT_DIR)/ChatPage.cpp \
	       $(CLIENT_DIR)/AppRunner.cpp 

SERVER_OBJS := $(SERVER_SRCS:.cpp=.o)
CLIENT_OBJS := $(CLIENT_SRCS:.cpp=.o)

.PHONY: all clean server client

all: server client

server: $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) -o $(SERVER_BIN) $^

client: $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) -o $(CLIENT_BIN) $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(SERVER_OBJS) $(CLIENT_OBJS) $(SERVER_BIN) $(CLIENT_BIN)
