#include <iostream>
#include <string>
#include <cstring>  // strerror
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include "ping_range.h"
#include "icmp_socket.h"

#define RECEIVE_BUFFER_SIZE     (1024)
#define SOCKET_TIMEOUT_SEC      (5)


// TODO: pass string through a reference
PingRange::PingRange(std::string address_and_mask)
{
    try {
        this->address_range = new AddressRange(address_and_mask);
        this->icmp_socket = new ICMPSocket(SOCKET_TIMEOUT_SEC);
    } catch(...) {
        throw;
    }
}

void PingRange::ping()
{
    std::vector<char> receive_buffer(RECEIVE_BUFFER_SIZE);
    auto hosts = this->get_address_range();

    // Send ICMP request to every host in the list and wait for reply
    for (auto host : hosts) {
        if (this->send_icmp_request(host) < 0) {
            std::cout << "WARNING: " << strerror(errno) << ". ";
            std::cout << "Failed to send ICMP request." << std::endl;
            continue;
        }
		while (1) {
            int rc = this->receive_icmp_response(receive_buffer);
            if (rc < 0) {
                std::cout << "ERROR: " << strerror(errno) << ". ";
                std::cout << "Failed to receive ICMP response." << std::endl;
                break;
            } else if (rc == 0) {
                std::cout << " [OFFLINE]" << std::endl;
                break;
            }

            // Got ICMP response
            struct ip *ip_header = (struct ip *)(receive_buffer.data());
            std::string responder_ip = inet_ntoa(ip_header->ip_src);

            // Remove potential trailing newline (TODO: check if it's possible to get trailing newline here)
            if (responder_ip.back() == '\n') {
                responder_ip.erase(responder_ip.back());
            }

            // Check response if it came from the right host.
            if (host == responder_ip) {
                this->parse_package(receive_buffer);
                break;
            }
		}
	}

}

void PingRange::parse_package(std::vector<char> &receive_buffer)
{
    char *receive_buffer_data = receive_buffer.data();
	struct icmphdr *icmpHeader = (struct icmphdr *)(receive_buffer_data + sizeof(struct iphdr));
	if (icmpHeader->type == 0) {
		std::cout << " [ONLINE]" << std::endl;
    } else {
		std::cout << " [OFFLINE]" << std::endl;
    }
}


int PingRange::send_icmp_request(std::string &dest_ip)
{
    // Structure includes destination host IP address info
    sockaddr_in dest = {
        .sin_family = AF_INET,
        .sin_port = htons(33490),
    };
    
    inet_aton(dest_ip.c_str(), (in_addr *)&dest.sin_addr);

    // Fill the packet
    struct icmphdr icmp_header = {
        .type = ICMP_ECHO,          // Echo request type (used to ping) 
        .code = 0,                  // Code 0 is required
        .checksum = 0,              // Initial checksum has to be zero
    };
	icmp_header.checksum = this->generate_internet_checksum(&icmp_header, sizeof(icmp_header));
	
    // Print host address
    std::cout << inet_ntoa(dest.sin_addr);

    // Send
    int hsocket = this->icmp_socket->get_socket();
    sockaddr *dest_sockaddr = (sockaddr *)&dest;
	if (sendto(hsocket, &icmp_header, sizeof(icmp_header), 0, dest_sockaddr, sizeof(sockaddr)) <= 0) {
        return -1;
    }

    // Print host name if applicable
    hostent *host_data = gethostbyaddr(&dest.sin_addr, sizeof(dest.sin_addr), AF_INET);
    if (host_data != NULL) {
        std::cout << " (" << host_data->h_name << ")";
    }

    return 0;
}


/* Return value:
    * 0  - host is offline / gracefully disconnected
    * -1 - general error / no response
    * number of bytes received  -   success
 */
int PingRange::receive_icmp_response(std::vector<char> &receive_buffer)
{
    struct sockaddr_in receiver;
    socklen_t receiverLength = sizeof(receiver);
    char *receive_buffer_data = receive_buffer.data();
    size_t receive_buffer_size = receive_buffer.capacity();

    int rc = recvfrom(this->icmp_socket->get_socket(), receive_buffer_data, receive_buffer_size, 0, (struct sockaddr*)&receiver, &receiverLength);
    if (rc < 0) {
        if (errno == EWOULDBLOCK) {
            return 0;      // No response before the socket timeout. Host is down/does not reply.
        }
        return -1;      // General error.
    } else if (rc == 0) {
        return 0;       // Host gracefully disconnected, i.e. offline.
    }

    // Success
    return rc;
}


u_int16_t PingRange::generate_internet_checksum(const void *packet, int packet_size)
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

const std::vector<std::string> & PingRange::get_address_range()
{
    return this->address_range->get_address_range();
}


PingRange::~PingRange()
{
    delete this->address_range;
    delete this->icmp_socket;
}