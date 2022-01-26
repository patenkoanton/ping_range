#include <iostream>
#include <string>
#include <cstring>  // bzero
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>     // getpid()
#include "ping_range.h"
#include "icmp_socket.h"

#define PACKETSIZE              (64)
#define RECEIVE_BUFFER_SIZE     (1024)
#define SOCKET_TIMEOUT_SEC      (5)


PingRange::PingRange(std::string address_and_mask) : address_range(address_and_mask), package_number(0)
{
    try {
        this->icmp_socket = new ICMPSocket(SOCKET_TIMEOUT_SEC);
    } catch(...) {
        throw;
    }
}

void PingRange::ping()
{
    std::vector<char> receive_buffer(RECEIVE_BUFFER_SIZE);      // TODO: make it signed char
    auto hosts = this->get_address_range();

    // Send ICMP request to every host in the list and wait for reply
    for (auto host : hosts) {
        this->send_icmp_request(host);
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


void PingRange::send_icmp_request(std::string &dest_ip)
{
    // Structure includes destination host IP address info
    sockaddr_in dest = {
        .sin_family = PF_INET,
        .sin_port = htons(33490),
    };
    
    inet_aton(dest_ip.c_str(), (in_addr *)&dest.sin_addr);

    struct Packet
    {
        struct icmphdr header;
        char message[PACKETSIZE - sizeof(struct icmphdr )];
    };

	struct Packet packet_to_send;
	bzero(&packet_to_send, sizeof(packet_to_send));     // TODO: do we need this
    
    // Fill the packet
	packet_to_send.header.type = ICMP_ECHO;
	packet_to_send.header.un.echo.id = getpid();        // TODO: do we need this

	for (int i = 0; i < sizeof(packet_to_send.message) - 1; i++) {
		packet_to_send.message[i] = i + '0';
    }
	packet_to_send.message[sizeof(packet_to_send.message)] = 0;
	packet_to_send.header.un.echo.sequence = this->package_number++;
	packet_to_send.header.checksum = this->checksum(&packet_to_send, sizeof(packet_to_send));
	
    // Send
	if (sendto(this->icmp_socket->get_socket(), &packet_to_send, sizeof(packet_to_send), 0, (struct sockaddr*)&dest, sizeof(struct sockaddr)) <= 0) {
		perror("ERROR: failed to send ICMP request\n");
        return;
    } else {
        struct hostent *he = gethostbyaddr(&dest.sin_addr, sizeof(dest.sin_addr), PF_INET);
		if (he != NULL) {
   			printf("%s", he->h_name);
        } else {
			printf("%s", inet_ntoa(dest.sin_addr));
        }
	}
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


unsigned short PingRange::checksum(void *b, int receiverLength)
{	unsigned short *receiveBuffer = (unsigned short *)b;
	unsigned int sum=0;
	unsigned short result;

	for (sum = 0; receiverLength > 1; receiverLength -= 2)
		sum += *receiveBuffer++;
	if (receiverLength == 1)
		sum += *(unsigned char*)receiveBuffer;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return result;
}

const std::vector<std::string> & PingRange::get_address_range()
{
    return this->address_range.get_address_range();
}


PingRange::~PingRange()
{
    delete this->icmp_socket;
}