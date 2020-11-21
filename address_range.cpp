#include <string>
#include <vector>
#include <iostream>

#include "address_range.h"

#define INVALID_ADDRESS     ("")
#define INVALID_MASK        (-1)

// TODO: replace string arg with const string &.
AddressRange::AddressRange(std::string address_and_mask)
{
    std::string address = this->get_address(address_and_mask);
    if (address == INVALID_ADDRESS) {
        std::cout << "Error: failed to parse IP address." << std::endl;
        // TODO: throw exception
    }

    std::cout << address << std::endl;

    int mask = this->get_mask(address_and_mask);
    if (mask == INVALID_MASK) {
        std::cout << "Error: failed to parse IP mask." << std::endl;
        // TODO: throw exception
    }

    std::cout << mask << std::endl;

    this->generate_address_range(address, mask);
}

void AddressRange::generate_address_range(std::string address, int mask)
{

}

const std::vector<std::string> & AddressRange::get_address_range()
{

}


std::string AddressRange::get_address(std::string address_and_mask)
{
    size_t slash_pos = address_and_mask.find('/');
    if (slash_pos == std::string::npos) {
        return INVALID_ADDRESS;     // TODO: fix
    }

    return std::string(address_and_mask, 0, slash_pos);
}


int AddressRange::get_mask(std::string address_and_mask)
{
    size_t slash_pos = address_and_mask.find('/');
    if (slash_pos == std::string::npos) {
        return INVALID_MASK;     // TODO: fix
    }

    return std::stoi(std::string(address_and_mask, slash_pos + 1));
}

