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


/* Simple sequence:
    * 1) Apply mask to calculate the subnet address.
    * 2) Calculate broadcast address.
    * 3) Go through all possible hosts in subnet.
 * Host order is used to simplify calculations, but everything is eventually converted to network order.
 */
int AddressRange::generate_address_range(std::string &input_address_string, int mask)
{
    // Apply mask to calculate the subnet address (network order).
    this->subnet = this->generate_subnet_address(input_address_string, mask);
    if (this->subnet == 0) {
        return -1;
    }

    // Calculate broadcast address.
    uint32_t subnet_host_order = this->reverse_byte_order(this->subnet);
    uint32_t broadcast_host_order = subnet_host_order + std::pow(2, IPv4_SIZE_BITS - mask) - 1;

    // Go through all possible hosts in subnet.
    for (uint32_t host = subnet_host_order + 1; host < broadcast_host_order; host++) {
        uint32_t host_network_order = this->reverse_byte_order(host);
        this->address_range.push_back(host_network_order);
    }

    // Save broadcast address just in case.
    this->broadcast = this->reverse_byte_order(broadcast_host_order);
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

/* Use input address and mask to generate subnet address.
 * Returns subnet address in network order.
 * Returns 0 if error occured.
 */
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

    // Apply a bit mask.
    uint32_t null_mask = 0xFFFFFFFF << (IPv4_SIZE_BITS - mask);
    uint32_t null_mask_in_network_order = this->reverse_byte_order(null_mask);
    return input_address_in_network_order & null_mask_in_network_order;
}


const std::vector<uint32_t> &AddressRange::get_address_range()
{
    return this->address_range;
}


/* Parse IP address and mask provided by user.
 * Always returns a pair of IP address and mask:
 *      * if no mask provided - mask will be set to 32
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
