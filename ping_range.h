#ifndef _PING_RANGE_H_
#define _PING_RANGE_H_

#include <string>
#include <vector>
#include "address_range.h"
#include "icmp_socket.h"

#define RECEIVE_BUFFER_SIZE     (1024)
#define SOCKET_TIMEOUT_SEC      (5)

class PingRange {
    AddressRange address_range;
    ICMPSocket *icmp_socket;

    unsigned char receiveBuffer[RECEIVE_BUFFER_SIZE];
    void send_icmp_request(struct sockaddr_in &sender);
    int package_number;             // TODO: do we need this
    void parse_package();
    unsigned short checksum(void *b, int receiverLength);
public:
    PingRange(std::string address_and_mask);
    ~PingRange();
    const std::vector<std::string> & get_address_range();
    void ping();
};

#endif
