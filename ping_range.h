#ifndef _PING_RANGE_H_
#define _PING_RANGE_H_

#include <string>
#include <vector>
#include "address_range.h"
#include "icmp_socket.h"

class PingRange {
    AddressRange address_range;
    ICMPSocket *icmp_socket;
    int package_number;             // TODO: do we need this

    int send_icmp_request(std::string &dest_ip);
    int receive_icmp_response(std::vector<char> &receive_buffer);
    void parse_package(std::vector<char> &receive_buffer);    
    unsigned short checksum(void *b, int receiverLength);
public:
    PingRange(std::string address_and_mask);
    ~PingRange();
    const std::vector<std::string> & get_address_range();
    void ping();
};

#endif
