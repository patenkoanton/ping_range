#ifndef _SOCKET_H
#define _SOCKET_H

#include "subnet.h"
#include "ip_address.h"

class Socket {
    OutputStreamBase& output_stream;
    int hsocket;
    int apply_subnet_bpf_filter(const std::shared_ptr<Subnet> &target_subnet);
    void close_socket();
    void show_errno();
public:
    Socket(OutputStreamBase& stream);
    ~Socket();
    int send_packet(const void *packet, size_t size, std::shared_ptr<IPAddress> dest);
    ssize_t receive_packet(std::vector<char> &buffer);
    int configure(const std::shared_ptr<Subnet> &target_subnet);
};


#endif  // _SOCKET_H