#include <arpa/inet.h>
#include "ip_address.h"


IPAddress::IPAddress(const std::string &ip_string)
{
    if (inet_aton(ip_string.c_str(), (in_addr *)&this->network_order) == 0) {
        throw std::string("Invalid IP address provided.");
    }

    this->host_order = ntohl(this->network_order);
}


IPAddress::IPAddress(uint32_t ip_host_order)
{
    this->host_order = ip_host_order;
    this->network_order = htonl(this->host_order);
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
    this->network_order = htonl(this->host_order);

    return old;
}


std::ostream& operator<<(std::ostream& stream, const IPAddress& arg)
{
    stream << arg.to_string();
    return stream;
}
