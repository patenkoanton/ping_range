#ifndef _SOCKET_H
#define _SOCKET_H

class ICMPSocket {
    int open_socket();
    int configure_socket(int socket_timeout_sec);
    void close_socket();
    int hsocket;
public:
    ICMPSocket(int socket_timeout_sec);
    ~ICMPSocket();
    int get_socket();
};


#endif  // _SOCKET_H