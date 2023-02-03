#include <iostream>
#include <string>
#include <netinet/ip_icmp.h>    // icmphdr
#include "ping.h"
#include "subnet.h"
#include "socket.h"
#include "factory.h"

#define RECEIVE_BUFFER_SIZE         (1024)
#define SOCKET_TIMEOUT_SEC          (2)
#define ICMP_REPLY_EXPECTED_SIZE    (sizeof(iphdr) + sizeof(icmphdr))


Ping::Ping(std::shared_ptr<Subnet> subnet)
{
    this->subnet = subnet;
    this->socket = factory_create_object<Socket>(SOCKET_TIMEOUT_SEC, subnet);
    if (this->socket == nullptr) {
        throw std::string("failed to create Socket object.");
    }
}

void Ping::ping()
{
    std::vector<char> receive_buffer(RECEIVE_BUFFER_SIZE);

    // Ping every host in the list
    for (auto host : this->subnet->hosts) {
        if (this->send_icmp_request(host) < 0) {
            std::cout << "WARNING: failed to send ICMP request." << std::endl;
            continue;
        }

        // Wait for reply...
        host_status_t host_status;
        auto receive_status = this->receive_icmp_reply(host, receive_buffer);
        if (receive_status < 0) {
            std::cout << "WARNING: failed to receive ICMP reply." << std::endl;
            continue;
        } else if (receive_status == 0) {
            host_status = offline;
        // Got valid reply...
        } else {
            host_status = this->parse_host_status(receive_buffer);
        }

        this->show_host_status(host, host_status);
    }
}


// Read host status from ICMP reply payload.
host_status_t Ping::parse_host_status(const std::vector<char> &receive_buffer) const
{
    auto icmp_header = (icmphdr *)(receive_buffer.data() + sizeof(iphdr));
    if (icmp_header->type == 0) {
        return online;
    }

    return offline;
}


// Prints host IP and online status in a readable form.
void Ping::show_host_status(std::shared_ptr<IPAddress> &host, host_status_t status) const
{
    std::cout << host->to_string() << " ";

    auto hostname = host->to_hostname();
    if (!hostname.empty()) {
        std::cout << '(' << hostname << ") ";
    }

    if (status == online) {
        std::cout << "[ONLINE]";
    } else {
        std::cout << "[OFFLINE]";
    }

    std::cout << std::endl;
}


int Ping::send_icmp_request(std::shared_ptr<IPAddress> &dest_host) const
{
    // Fill the packet
    icmphdr icmp_header = {
        .type = ICMP_ECHO,          // Echo request
        .code = 0,                  // Code 0 is required
        .checksum = 0,              // Initial checksum has to be zero
    };
    icmp_header.checksum = this->generate_internet_checksum(&icmp_header, sizeof(icmp_header));

    // Send
    return this->socket->send_packet(&icmp_header, sizeof(icmp_header), dest_host);
}


/* Return value:
    * negative   -   error
    * 0         -   host offline
    * positive  -   sucess: received reply from a host */
ssize_t Ping::receive_icmp_reply(std::shared_ptr<IPAddress> &host, std::vector<char> &receive_buffer) const
{
    // Keep receiving until the replier IP is right.
    ssize_t bytes_received = 0;
    for (auto replier = factory_create_object<IPAddress, uint32_t>(0); *replier != *host; ) {
        bytes_received = this->socket->receive_packet(receive_buffer);
        if (bytes_received <= 0) {
            break;
        } else if (bytes_received == ICMP_REPLY_EXPECTED_SIZE) {
            auto ip_header = (iphdr *)receive_buffer.data();
            *replier = ntohl(ip_header->saddr);
        }
    }

    return bytes_received;
}


uint16_t Ping::generate_internet_checksum(const void *packet, int packet_size) const
{
    uint16_t *buffer = (uint16_t *)packet;
    uint32_t sum = 0;
    uint16_t result = 0;

    for (sum = 0; packet_size > 1; packet_size -= 2) {
        sum += *(buffer++);
    }
    if (packet_size == 1) {
        sum += *(uint8_t *)buffer;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}
