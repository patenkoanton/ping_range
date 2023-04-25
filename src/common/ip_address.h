#pragma once

#include <stdint.h>
#include <string>
#include <netinet/in.h>     // in_addr


class IPAddress {
    uint32_t host_order = 0;
    uint32_t network_order = 0;
public:
    IPAddress(uint32_t ip_host_order);
    IPAddress(const std::string &ip_string);
    uint32_t to_host() const;
    const in_addr *to_addr() const;
    std::string to_hostname() const;

    // Make calculations easier.
    IPAddress operator&(uint32_t arg) const;
    IPAddress operator+(uint32_t arg) const;
    IPAddress operator-(uint32_t arg) const;
    bool operator<(const IPAddress &arg) const;
    bool operator!=(const IPAddress &arg) const;
    bool operator==(const IPAddress &arg) const;
    IPAddress operator++(int);
    std::string to_string() const;
};
