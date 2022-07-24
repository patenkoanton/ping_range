#ifndef _SOCKET_H
#define _SOCKET_H

class Socket {
    int open_socket();
    int configure_socket(int socket_timeout_sec);
    void close_socket();
    int hsocket;
public:
    Socket(int socket_timeout_sec);
    ~Socket();
    int get_socket();
};


#endif  // _SOCKET_H