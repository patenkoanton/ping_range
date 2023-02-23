#include <arpa/inet.h>
#include <netdb.h>
#include "ip_address.h"


IPAddress::IPAddress(const std::string &ip_string)
{
    auto source = ip_string.c_str();
    auto dest = (in_addr *)&this->network_order;

    // Convert from IP string to a number representation.
    if (inet_aton(source, dest) == 0) {
        throw std::string("Invalid IP address provided.");
    }

    this->host_order = ntohl(this->network_order);
}


IPAddress::IPAddress(uint32_t ip_host_order)
{
    this->host_order = ip_host_order;
    this->network_order = htonl(this->host_order);
}


uint32_t IPAddress::to_host() const
{
    return this->host_order;
}


// Return IP address packed into in_addr in network order.
const in_addr * IPAddress::to_addr() const
{
    return (in_addr *)&this->network_order;
}


// Return host name, or empty string in case of error.
std::string IPAddress::to_hostname() const
{
    std::string hostname;

    auto data = gethostbyaddr(this->to_addr(), sizeof(in_addr), AF_INET);
    if (data != NULL) {
        hostname = data->h_name;
    }

    return hostname;
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


bool IPAddress::operator!=(const IPAddress &arg) const
{
    return this->to_host() != arg.to_host();
}


bool IPAddress::operator==(const IPAddress &arg) const
{
    return this->to_host() == arg.to_host();
}


IPAddress IPAddress::operator++(int)
{
    IPAddress old = *this;
    (this->host_order)++;
    this->network_order = htonl(this->host_order);

    return old;
}


std::string IPAddress::to_string() const
{
    char *ip_string = inet_ntoa(*this->to_addr());
    return std::string(ip_string);
}


std::ostream& operator<<(std::ostream& stream, const IPAddress& arg)
{
    stream << arg.to_string();
    return stream;
}
