#ifndef _PING_SUBNET_H_
#define _PING_SUBNET_H_

#include <string>
#include <vector>
#include "address_range.h"
#include "icmp_socket.h"

class PingSubnet {
    AddressRange *address_range;
    ICMPSocket *icmp_socket;

    int send_icmp_request(uint32_t dest_ip);
    int receive_icmp_response(std::vector<char> &receive_buffer);
    void parse_package(std::vector<char> &receive_buffer);    
    u_int16_t generate_internet_checksum(const void *packet, int packet_size);
public:
    PingSubnet(std::string address_and_mask);
    ~PingSubnet();
    const std::vector<uint32_t> &get_address_range();
    void ping();
};

#endif
