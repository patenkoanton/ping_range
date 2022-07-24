#ifndef _SUBNET_H
#define _SUBNET_H

#include <string>
#include <vector>


class AddressRange {
    int generate_address_range(std::string &input_address, int mask);
    uint32_t reverse_byte_order(uint32_t input);
    uint32_t generate_subnet_address(std::string &input_address, int mask);
    std::pair<std::string, int> parse_input_address_string(std::string &input_address_string);
public:
    AddressRange(std::string &input_address_string);
    std::vector<uint32_t> address_range;     // vector with generated host addresses in network order
    uint32_t subnet;
    uint32_t broadcast;
};

#endif  // _SUBNET_H
