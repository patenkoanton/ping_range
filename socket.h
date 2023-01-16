#ifndef _SOCKET_H
#define _SOCKET_H

class Socket {
    int open_socket();
    int configure_socket(int socket_timeout_sec, uint32_t subnet, uint32_t bitmask);
    int apply_subnet_bpf_filter(uint32_t subnet, uint32_t bitmask);
    void close_socket();
    int hsocket;
public:
    Socket(int socket_timeout_sec, uint32_t subnet, uint32_t bitmask);
    ~Socket();
    int get_socket();
};


#endif  // _SOCKET_H