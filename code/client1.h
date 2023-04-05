#ifndef CLIENT_H
#define CLIENT_H

#include "global1.h"

class client{
    private:
        int server_port;
        string server_ip;
        int sock;
    public:
        client(int port,string ip);
        ~client();
        void run();
        void HandleClient(int conn);
        void userinfo(struct user *user);
};
#endif
