#include <string>
#include <vector>
#include <iostream>
#include <tgmath.h>

#include "address_range.h"

#define IP_ADDRESS_NUMBERS  (4)
#define IP_ADDRESS_SIZE_BITS  (32)


// TODO: pass string through a reference
AddressRange::AddressRange(std::string address_and_mask)
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


void AddressRange::generate_address_range(std::string address, int mask)
{
    // IP address represented as octets: (192 << 24) | (168 << 16) | (0 << 8) | (1 << 0) = 192.168.0.1
    uint32_t address_octets = 0;
    this->address_string_to_octets(address, address_octets);
    
    // Go through all possible IP addresses and store them into address_range vector.
    uint32_t mask_number_of_bits = IP_ADDRESS_SIZE_BITS - mask;
    uint32_t nulled_address = address_octets & (0xFFFFFFFF << mask_number_of_bits);
    uint32_t max_mask = pow(2, mask_number_of_bits) - 1;
    for (uint32_t mask_bits = 0; mask_bits <= max_mask; mask_bits++) {
        std::string current_address;
        uint32_t current_address_octets = nulled_address | mask_bits;
        this->address_octets_to_string(current_address, current_address_octets);
        this->address_range.push_back(current_address);
        std::cout << "generated IP address: " << current_address << std::endl;
    }
}

void AddressRange::address_string_to_octets(std::string address, uint32_t & address_number)
{
    size_t dot_pos = 0;
    size_t digit_pos = 0;

    // Go through the dots in IP address.
    for (int i = IP_ADDRESS_NUMBERS - 1; i >= 0; i--) {
        dot_pos = address.find('.', dot_pos);
        if (dot_pos == std::string::npos) {
            dot_pos = address.length();
        }
        
        // Parse the number between the dots.
        std::string address_octet_str = std::string(address, digit_pos, dot_pos - digit_pos);
        uint32_t address_octet = std::stoi(address_octet_str) << i * 8;
        address_number |= address_octet;
        digit_pos = dot_pos = dot_pos + 1;
    }
}

void AddressRange::address_octets_to_string(std::string & address, uint32_t address_number)
{
    address.clear();
    for (int i = IP_ADDRESS_NUMBERS - 1; i >= 0; i--) {
        uint8_t address_octet = address_number >> i * 8;
        address += std::to_string(address_octet) + ".";
    }

    // Remove trailing dot.
    address.pop_back();
}

const std::vector<std::string> & AddressRange::get_address_range()
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

