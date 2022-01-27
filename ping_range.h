#ifndef _PING_RANGE_H_
#define _PING_RANGE_H_

#include <string>
#include <vector>
#include "address_range.h"
#include "icmp_socket.h"

class PingRange {
    AddressRange *address_range;
    ICMPSocket *icmp_socket;

    int send_icmp_request(std::string &dest_ip);
    int receive_icmp_response(std::vector<char> &receive_buffer);
    void parse_package(std::vector<char> &receive_buffer);    
    u_int16_t generate_internet_checksum(const void *packet, int packet_size);
public:
    PingRange(std::string address_and_mask);
    ~PingRange();
    const std::vector<std::string> & get_address_range();
    void ping();
};

#endif
