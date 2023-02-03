#include <string>
#include <iostream>
#include <cmath>
#include <arpa/inet.h>
#include <stdexcept>        // std::invalid_argument
#include "subnet.h"
#include "ip_address.h"
#include "factory.h"

#define IPv4_SIZE_BITS  (32)


/* Simple sequence:
    * 1) Apply mask to calculate subnet address.
    * 2) Calculate broadcast address.
    * 3) Store all possible hosts in subnet into vector.
 */
Subnet::Subnet(std::string &input_address_string)
{
    std::pair<std::string, int> subnet_address_and_mask = this->parse_input_address_string(input_address_string);
    auto input_address = subnet_address_and_mask.first;
    auto input_mask = subnet_address_and_mask.second;

    // Calculate subnet address.
    this->subnet = this->generate_subnet_address(input_address, input_mask);
    if (this->subnet == nullptr) {
        throw std::string("Failed to generate subnet address.");
    }

    // Calculate broadcast address.
    this->broadcast = this->generate_broadcast_address(input_mask);
    if (this->broadcast == nullptr) {
        throw std::string("Failed to generate broadcast address.");
    }

    // Go through all possible hosts in subnet.
    if (this->generate_hosts(this->hosts) < 0) {
        throw std::string("Failed to generate hosts.");
    }
}


// Use input address and mask to generate subnet address.
std::shared_ptr<IPAddress> Subnet::generate_subnet_address(std::string &input_address_string, int mask)
{
    if (mask < 1 || mask > 32) {
        std::cerr << "ERROR: invalid subnet mask provided." << std::endl;
        return nullptr;
    }

    auto input_ip = factory_create_object<IPAddress, std::string&>(input_address_string);
    if (input_ip == nullptr) {
        std::cerr << "ERROR: failed to create IP address object." << std::endl;
        return nullptr;
    }

    // Generate a bitmask.
    this->bitmask = 0xFFFFFFFF << (IPv4_SIZE_BITS - mask);

    // Verify non-network bits not set in the input (similar to what tcmpdump does).
    if ((*input_ip & ~this->bitmask) != 0) {
        std::cerr << "ERROR: non-network bits set in " << input_address_string << "." << std::endl;
        return nullptr;
    }

    // Apply a bitmask.
    return std::make_shared<IPAddress>(*input_ip & this->bitmask);
}


std::shared_ptr<IPAddress> Subnet::generate_broadcast_address(int mask)
{
    uint32_t max_number_of_addresses = std::pow(2, IPv4_SIZE_BITS - mask);
    auto broadcast = *this->subnet + max_number_of_addresses - 1;

    return std::make_shared<IPAddress>(broadcast);
}


int Subnet::generate_hosts(std::vector<std::shared_ptr<IPAddress>> &hosts)
{
    auto host = *this->subnet + 1;
    while (host < *this->broadcast) {
        hosts.push_back(std::make_shared<IPAddress>(host));
        host++;
    }

    return 0;
}


/* Parse IP address and mask provided by user.
 * Always returns a pair of IP address and mask:
 *      * if no mask provided - mask will be set to 32
 *      * slash without mask - mask will be set to -1 (invalid)
 */
std::pair<std::string, int> Subnet::parse_input_address_string(std::string &input_address_string)
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
