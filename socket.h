#ifndef _SOCKET_H
#define _SOCKET_H

#include "subnet.h"
#include "ip_address.h"

class Socket {
    int open_socket();
    int configure_socket(int socket_timeout_sec, const std::shared_ptr<Subnet> &target_subnet);
    int apply_subnet_bpf_filter(const std::shared_ptr<Subnet> &target_subnet);
    void close_socket();
    int hsocket;
public:
    Socket(int socket_timeout_sec, const std::shared_ptr<Subnet> target_subnet);
    ~Socket();
    int get_socket();
};


#endif  // _SOCKET_H