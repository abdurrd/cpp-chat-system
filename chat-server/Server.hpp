#include <netinet/in.h>
#include <vector>

#define PORT 8888
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

class Group;

class Server {
        int                     server_fd;                      //file descripter for the socket
        int                     new_socket;                     //temperary socket for new connections
        //int                   client_sockets[MAX_CLIENTS] {}; //array to store all client socket descripters
        std::vector<Group*>     groups;
        
        sockaddr_in             address;                        //structure to hold server address information
        int                     opt;                            //option for socket configuration
        
public:
        void start();
        void handle_new_connections();

};
