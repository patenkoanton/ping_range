#include <iostream>
#include <string>
#include <cstring>  // strerror
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include "ping_subnet.h"
#include "address_range.h"
#include "icmp_socket.h"
#include "factory.h"

#define RECEIVE_BUFFER_SIZE         (1024)
#define SOCKET_TIMEOUT_SEC          (5)
#define ICMP_REPLY_EXPECTED_SIZE    (sizeof(iphdr) + sizeof(icmphdr))


PingSubnet::PingSubnet(std::string address_and_mask)
{
    this->address_range = factory_create_object<AddressRange, std::string&>(address_and_mask);
    if (this->address_range == nullptr) {
        throw std::string("failed to create AddressRange object.");
    }

    this->icmp_socket = factory_create_object<ICMPSocket>(SOCKET_TIMEOUT_SEC);
    if (this->icmp_socket == nullptr) {
        throw std::string("failed to create ICMPSocket object.");
    }
}

void PingSubnet::ping()
{
    std::vector<char> receive_buffer(RECEIVE_BUFFER_SIZE);
    auto hosts = this->get_address_range();

    // Send ICMP request to every host in the list and wait for reply
    for (auto host_address : hosts) {
        if (this->send_icmp_request(host_address) < 0) {
            std::cout << "WARNING: " << std::strerror(errno) << ". ";
            std::cout << "Failed to send ICMP request." << std::endl;
            continue;
        }
        while (1) {
            int bytes_received = this->receive_icmp_reply(receive_buffer);
            if (bytes_received < 0) {
                std::cout << "WARNING: " << std::strerror(errno) << ". ";
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
            auto replier_address = ip_header->saddr;
            if (replier_address == host_address) {
                this->parse_package(receive_buffer);
                break;
            }
        }
    }

}

void PingSubnet::parse_package(std::vector<char> &receive_buffer)
{
    char *receive_buffer_data = receive_buffer.data();
    auto icmpHeader = (icmphdr *)(receive_buffer_data + sizeof(iphdr));
    if (icmpHeader->type == 0) {
        std::cout << " [ONLINE]" << std::endl;
    } else {
        std::cout << " [OFFLINE]" << std::endl;
    }
}


int PingSubnet::send_icmp_request(uint32_t dest_ip)
{
    // Structure includes destination host IP address info
    sockaddr_in dest = {
        .sin_family = AF_INET,
        .sin_port = htons(33490),
        .sin_addr = *(in_addr *)&dest_ip,
    };

    // Fill the packet
    icmphdr icmp_header = {
        .type = ICMP_ECHO,          // Echo request type (used to ping) 
        .code = 0,                  // Code 0 is required
        .checksum = 0,              // Initial checksum has to be zero
    };
    icmp_header.checksum = this->generate_internet_checksum(&icmp_header, sizeof(icmp_header));
    
    // Print host address
    std::cout << inet_ntoa(dest.sin_addr);

    // Send
    int hsocket = this->icmp_socket->get_socket();
    auto dest_sockaddr = (sockaddr *)&dest;
    if (sendto(hsocket, &icmp_header, sizeof(icmp_header), 0, dest_sockaddr, sizeof(sockaddr)) <= 0) {
        return -1;
    }

    // Print host name if applicable
    auto host_data = gethostbyaddr(&dest.sin_addr, sizeof(dest.sin_addr), AF_INET);
    if (host_data != NULL) {
        std::cout << " (" << host_data->h_name << ")";
    }

    return 0;
}


/* Return value:
    * 0  - host is offline / gracefully disconnected
    * -1 - general error / no reply
    * number of bytes received  -   success
 */
int PingSubnet::receive_icmp_reply(std::vector<char> &receive_buffer)
{
    sockaddr_in receiver;
    socklen_t receiverLength = sizeof(receiver);
    char *receive_buffer_data = receive_buffer.data();
    size_t receive_buffer_size = receive_buffer.capacity();

    int rc = recvfrom(this->icmp_socket->get_socket(), receive_buffer_data, receive_buffer_size, 0, (sockaddr*)&receiver, &receiverLength);
    if (rc < 0) {
        if (errno == EWOULDBLOCK) {
            return 0;      // No reply before the socket timeout. Host is down/does not reply.
        }
        return -1;      // General error.
    } else if (rc == 0) {
        return 0;       // Host gracefully disconnected, i.e. offline.
    }

    // Success
    return rc;
}


uint16_t PingSubnet::generate_internet_checksum(const void *packet, int packet_size)
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

const std::vector<uint32_t> &PingSubnet::get_address_range()
{
    return this->address_range->get_address_range();
}
