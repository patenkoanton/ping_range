#pragma once

#include <string>
#include <vector>
#include <memory>
#include "ip_address.h"
#include "output_stream.h"


class Subnet {
    OutputStream &output_stream;
    OutputStream &error_stream;
    void generate_hosts(std::vector<std::shared_ptr<IPAddress>> &hosts);
    std::unique_ptr<IPAddress> generate_subnet_address(const std::string &input_address, int mask);
    std::unique_ptr<IPAddress> generate_broadcast_address(int mask);
    std::pair<std::string, int> parse_input_address_string(const std::string &input_address_string) const;

    // Constants.
    const uint32_t ipv4_size_bits = 32;
public:
    Subnet(const std::string &input_address_string, OutputStream &output_stream, OutputStream &error_stream);
    std::vector<std::shared_ptr<IPAddress>> hosts;     // vector with generated host addresses
    std::unique_ptr<IPAddress> subnet;
    std::unique_ptr<IPAddress> broadcast;
    uint32_t bitmask;     // in host order (required for Socket configuration)
};
