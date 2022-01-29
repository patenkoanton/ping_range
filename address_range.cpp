#include <string>
#include <vector>
#include <iostream>
#include <tgmath.h>     // TODO: replace with <math> or <math.h>?
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "address_range.h"

#define IP_ADDRESS_SIZE_BITS  (32)


AddressRange::AddressRange(std::string &address_and_mask)
{
    std::string address = this->get_address(address_and_mask);
    if (address.empty()) {
        throw std::string("Error: failed to parse IP address.");
    }
    std::cout << address << std::endl;

    int mask = this->get_mask(address_and_mask);
    if (mask < 0) {
        throw std::string("Error: failed to parse IP mask.");
    }
    std::cout << mask << std::endl;

    this->generate_address_range(address, mask);
}


void AddressRange::generate_address_range(std::string &input_address_string, int mask)
{
    // Get subnet address (network order).
    uint32_t subnet_address = this->generate_subnet_address(input_address_string, mask);

    // Go through all possible hosts in subnet.
    uint32_t max_mask = pow(2, IP_ADDRESS_SIZE_BITS - mask) - 1;
    for (uint32_t current_mask = 0; current_mask <= max_mask; current_mask++) {
        uint32_t current_mask_in_network_order = this->reverse_byte_order(current_mask);
        uint32_t host_address = subnet_address | current_mask_in_network_order;
        this->address_range.push_back(host_address);
    }
}


uint32_t AddressRange::reverse_byte_order(uint32_t mask)
{
    uint8_t *first_byte = (uint8_t *)&mask;
    uint8_t *last_byte = first_byte + sizeof(mask) - 1;
    while (first_byte < last_byte) {
        uint8_t buffer = *last_byte;
        *last_byte = *first_byte;
        *first_byte = buffer;
        first_byte++;
        last_byte--;
    }

    return mask;
}

// Use input address and mask to generate subnet address.
// Returns subnet address in network order.
uint32_t AddressRange::generate_subnet_address(std::string &input_address_string, int mask)
{
    // Convert from numbers-and-dots notation into a number.
    // TODO: inet_aton might return an error if address is invalid
    uint32_t input_address_in_network_order = 0;
    inet_aton(input_address_string.c_str(), (in_addr *)&input_address_in_network_order);
    
    // Magic part: now we have to reverse the address (to a host byte order) in order to apply the mask and get a subnet address.
    // Applying the mask to the network order address is too much of a brainfuck.
    uint32_t input_address_in_host_order = this->reverse_byte_order(input_address_in_network_order);

    // Get subnet address by applying the mask.
    uint32_t subnet_address_in_host_order = input_address_in_host_order & (0xFFFFFFFF << (IP_ADDRESS_SIZE_BITS - mask));
    return this->reverse_byte_order(subnet_address_in_host_order);
}


const std::vector<uint32_t> &AddressRange::get_address_range()
{
    return this->address_range;
}


std::string AddressRange::get_address(std::string &address_and_mask)
{
    size_t slash_pos = address_and_mask.find('/');
    if (slash_pos == std::string::npos) {
        return std::string();     // Return empty string if no slash found.
    }

    return std::string(address_and_mask, 0, slash_pos);
}


int AddressRange::get_mask(std::string &address_and_mask)
{
    size_t slash_pos = address_and_mask.find('/');
    if (slash_pos == std::string::npos) {
        return -1;      // Error - no slash found. 
    }

    return std::stoi(std::string(address_and_mask, slash_pos + 1));
}

