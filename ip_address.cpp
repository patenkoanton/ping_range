#include <arpa/inet.h>
#include "ip_address.h"


IPAddress::IPAddress(const std::string &ip_string)
{
    if (inet_aton(ip_string.c_str(), (in_addr *)&this->network_order) == 0) {
        throw std::string("Invalid IP address provided.");
    }

    this->host_order = this->reverse_byte_order(this->network_order);
}


IPAddress::IPAddress(uint32_t ip_host_order)
{
    this->network_order = this->reverse_byte_order(ip_host_order);
    this->host_order = ip_host_order;
}


uint32_t IPAddress::reverse_byte_order(uint32_t input)
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


uint32_t IPAddress::to_network() const
{
    return this->network_order;
}


uint32_t IPAddress::to_host() const
{
    return this->host_order;
}


std::string IPAddress::to_string() const
{
    char *ip_string = inet_ntoa(*((in_addr*)&this->network_order));
    return std::string(ip_string);
}


IPAddress IPAddress::operator&(uint32_t arg) const
{
    return this->host_order & arg;
}


IPAddress IPAddress::operator+(uint32_t arg) const
{
    return this->host_order + arg;
}


IPAddress IPAddress::operator-(uint32_t arg) const
{
    return this->host_order - arg;
}


bool IPAddress::operator<(const IPAddress &arg) const
{
    return this->host_order < arg.host_order;
}


IPAddress IPAddress::operator++(int)
{
    IPAddress old = *this;
    (this->host_order)++;
    this->network_order = this->reverse_byte_order(this->host_order);

    return old;
}


std::ostream& operator<<(std::ostream& stream, const IPAddress& arg)
{
    stream << arg.to_string();
    return stream;
}
