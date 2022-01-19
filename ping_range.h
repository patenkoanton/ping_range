#ifndef _PING_RANGE_H_
#define _PING_RANGE_H_

#include <string>
#include <vector>
#include "address_range.h"

#define RECEIVE_BUFFER_SIZE     (1024)

class PingRange {
    AddressRange address_range;
    int open_socket();
    int configure_socket();
    int open_and_configure_socket();
    int hsocket;
    unsigned char receiveBuffer[RECEIVE_BUFFER_SIZE];
    void send_icmp_request(struct sockaddr_in &sender);
    int package_number;             // TODO: do we need this
    void parse_package();
    unsigned short checksum(void *b, int receiverLength);
public:
    PingRange(std::string address_and_mask) : address_range(address_and_mask), hsocket(-1), package_number(0) {};       // TODO: verify declaration vs implementation validity
    const std::vector<std::string> & get_address_range();
    void ping();
};

#endif
