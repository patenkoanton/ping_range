#ifndef _SUBNET_H
#define _SUBNET_H

#include <string>
#include <vector>
#include <memory>
#include "ip_address.h"


class Subnet {
    int generate_hosts(std::string &input_address, int mask);
    uint32_t reverse_byte_order(uint32_t input);
    std::shared_ptr<IPAddress> generate_subnet_address(std::string &input_address, int mask);
    std::pair<std::string, int> parse_input_address_string(std::string &input_address_string);
public:
    Subnet(std::string &input_address_string);
    std::vector<uint32_t> hosts;     // vector with generated host addresses in network order
    uint32_t subnet;      // in host order (required for Socket configuration)
    uint32_t bitmask;     // in host order (required for Socket configuration)
    uint32_t broadcast;
};

#endif  // _SUBNET_H
