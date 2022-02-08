#include <string>
#include <iostream>
#include <cmath>
#include <arpa/inet.h>
#include <stdexcept>        // std::invalid_argument
#include "address_range.h"

#define IPv4_SIZE_BITS  (32)


AddressRange::AddressRange(std::string &input_address_string)
{
    std::pair<std::string, int> subnet_address_and_mask = this->parse_input_address_string(input_address_string);
    std::cout << "address: " << subnet_address_and_mask.first << std::endl;
    std::cout << "mask: " << subnet_address_and_mask.second << std::endl;
    if (this->generate_address_range(subnet_address_and_mask.first, subnet_address_and_mask.second) < 0) {
        throw std::string("Failed to generate the range of host IP addresses.");
    }
}


int AddressRange::generate_address_range(std::string &input_address_string, int mask)
{
    // Get subnet address (network order).
    uint32_t subnet_address = this->generate_subnet_address(input_address_string, mask);
    if (subnet_address == 0) {
        return -1;
    }

    // Go through all possible hosts in subnet.
    uint32_t max_mask = std::pow(2, IPv4_SIZE_BITS - mask) - 1;
    for (uint32_t current_mask = 0; current_mask <= max_mask; current_mask++) {
        uint32_t current_mask_in_network_order = this->reverse_byte_order(current_mask);
        uint32_t host_address = subnet_address | current_mask_in_network_order;
        this->address_range.push_back(host_address);
    }

    return 0;
}


uint32_t AddressRange::reverse_byte_order(uint32_t input)
{
    uint8_t *first_byte = (uint8_t *)&input;
    uint8_t *last_byte = first_byte + sizeof(input) - 1;
    while (first_byte < last_byte) {
        uint8_t buffer = *last_byte;
        *last_byte = *first_byte;
        *first_byte = buffer;
        first_byte++;
        last_byte--;
    }

    return input;
}

// Use input address and mask to generate subnet address.
// Returns subnet address in network order.
// Returns 0 if error occured.
uint32_t AddressRange::generate_subnet_address(std::string &input_address_string, int mask)
{
    if (mask < 1 || mask > 32) {
        std::cerr << "ERROR: invalid subnet mask provided." << std::endl;
        return 0;
    }

    // Convert from numbers-and-dots notation into a number.
    uint32_t input_address_in_network_order = 0;
    if (inet_aton(input_address_string.c_str(), (in_addr *)&input_address_in_network_order) == 0) {
        std::cerr << "ERROR: invalid IP address provided." << std::endl;
        return 0;
    }
    
    // Magic part: now we have to reverse the address (to a host byte order) in order to apply the mask and get a subnet address.
    // Applying the mask to the network order address is too much of a brainfuck.
    uint32_t input_address_in_host_order = this->reverse_byte_order(input_address_in_network_order);

    // Get subnet address by applying the mask.
    uint32_t subnet_address_in_host_order = input_address_in_host_order & (0xFFFFFFFF << (IPv4_SIZE_BITS - mask));
    return this->reverse_byte_order(subnet_address_in_host_order);
}


const std::vector<uint32_t> &AddressRange::get_address_range()
{
    return this->address_range;
}


/* Parse IP address and mask provided by user.
 * Always returns a pair of IP address and mask:
 *		* if no mask provided - mask will be set to 32
 *      * slash without mask - mask will be set to -1 (invalid)
 */
std::pair<std::string, int> AddressRange::parse_input_address_string(std::string &input_address_string)
{
    std::string address;
    int mask = 0;

    size_t slash_pos = input_address_string.find('/');
    if (slash_pos == std::string::npos) {       // no slash '/'
        address = input_address_string;
        mask = IPv4_SIZE_BITS;
    } else {
        address = std::string(input_address_string, 0, slash_pos);
        try {
            mask = std::stoi(std::string(input_address_string, slash_pos + 1));
        } catch (std::invalid_argument &) {
            std::cout << "WARNING: empty subnet mask." << std::endl;
            mask = -1;
        }
    }

    return std::pair<std::string, int> (address, mask);
}
