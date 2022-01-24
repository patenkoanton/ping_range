#ifndef _ICMP_SOCKET_H_
#define _ICMP_SOCKET_H_

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


#endif  // _ICMP_SOCKET_H_