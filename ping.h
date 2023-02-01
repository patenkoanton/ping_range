#ifndef _PING_H_
#define _PING_H_

#include <string>
#include <vector>
#include <memory>
#include "subnet.h"
#include "socket.h"
#include "ip_address.h"

enum host_status_t {
    online,
    offline,
};


class Ping {
    std::shared_ptr<Subnet> subnet;
    std::shared_ptr<Socket> socket;

    int send_icmp_request(std::shared_ptr<IPAddress> &dest_host) const;
    ssize_t receive_icmp_reply(std::shared_ptr<IPAddress> &host, std::vector<char> &receive_buffer) const;  
    host_status_t parse_host_status(const std::vector<char> &receive_buffer) const;
    void show_host_status(std::shared_ptr<IPAddress> &host, host_status_t status) const;
    uint16_t generate_internet_checksum(const void *packet, int packet_size) const;
public:
    Ping(std::shared_ptr<Subnet> subnet);
    void ping();
};

#endif
