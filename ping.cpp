#include <iostream>
#include <string>
#include <cstring>  // strerror
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

    // Send ICMP request to every host in the list
    for (auto host : this->subnet->hosts) {
        if (this->send_icmp_request(host) < 0) {
            std::cout << "Failed to send ICMP request." << std::endl;
            continue;
        }
        // Wait for reply...
        while (1) {
            auto bytes_received = this->socket->receive_packet(receive_buffer.data(), receive_buffer.capacity());
            if (bytes_received < 0) {
                std::cout << "Failed to receive ICMP reply." << std::endl;
                break;
            } else if (bytes_received == 0) {
                std::cout << " [OFFLINE]" << std::endl;
                break;
            } else if (bytes_received != ICMP_REPLY_EXPECTED_SIZE) {
                continue;
            }

            // Got ICMP reply. Verify that it came from the right host.
            auto ip_header = (iphdr *)receive_buffer.data();
            auto replier_address_network_order = ip_header->saddr;
            if (replier_address_network_order == host->to_network()) {
                this->parse_package(receive_buffer);
                break;
            }
        }
    }
}


void Ping::parse_package(std::vector<char> &receive_buffer)
{
    char *receive_buffer_data = receive_buffer.data();
    auto icmpHeader = (icmphdr *)(receive_buffer_data + sizeof(iphdr));
    if (icmpHeader->type == 0) {
        std::cout << " [ONLINE]" << std::endl;
    } else {
        std::cout << " [OFFLINE]" << std::endl;
    }
}


int Ping::send_icmp_request(std::shared_ptr<IPAddress> &dest_host)
{
    // Fill the packet
    icmphdr icmp_header = {
        .type = ICMP_ECHO,          // Echo request
        .code = 0,                  // Code 0 is required
        .checksum = 0,              // Initial checksum has to be zero
    };
    icmp_header.checksum = this->generate_internet_checksum(&icmp_header, sizeof(icmp_header));
    
    // Print host address
    std::cout << dest_host->to_string();

    // Send
    if (this->socket->send_packet(&icmp_header, sizeof(icmp_header), dest_host) < 0) {
        return -1;
    }

    // Print host name if applicable
    auto hostname = dest_host->to_hostname();
    if (hostname.empty() == false) {
        std::cout << " (" << hostname << ")";
    }

    return 0;
}


uint16_t Ping::generate_internet_checksum(const void *packet, int packet_size)
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