#include <vector>
#include <string>
#include <cstring>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>     // getpid()
#include "ping_range.h"
#include "icmp_socket.h"

#define MAX_IP_STRING_LEN       (16)
#define PACKETSIZE              (64)


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
    // TODO: do something with all this.
    // TODO: explain.
	char * noise;
	struct sockaddr_in antelop;
	struct sockaddr_in receiver;
	socklen_t receiverLength = sizeof(receiver);
	struct ip *ipHeader = (struct ip *)(this->receiveBuffer);
    char host_ip[MAX_IP_STRING_LEN];
    std::vector<std::string> hosts = get_address_range();

    // Send ICMP packet for every host in the list and wait for reply.
    for (std::string host : hosts) {
        struct sockaddr_in sender;
        sender.sin_family = PF_INET;
	    sender.sin_port = htons(33490);
	    inet_aton(host.c_str(), (struct in_addr *)&sender.sin_addr.s_addr);

		// if(excludeLocalAddressess(counter->host, this) == 1){
		// 	printf("%s [ONLINE]\n", counter->host);
		// 	continue;
		// }

		this->send_icmp_request(sender);
		
		for (;;)
		{
			if (recvfrom(this->icmp_socket->get_socket(), this->receiveBuffer, sizeof(this->receiveBuffer), 0, (struct sockaddr*)&receiver, &receiverLength) < 0) {
				if (errno == EWOULDBLOCK) {				
					printf(" [OFFLINE]\n");	
					break;
				}
				if (errno == EINTR)
					break;
				perror("send_icmp_request: recvfrom");
				break;
			}
			else {
				noise = inet_ntoa(ipHeader->ip_src);

                char lol[1000];             // TODO: fix this shit.
                strcpy(lol, host.c_str());

				strtok(lol, "\n");
				if (strcmp(host.c_str(), noise) == 0) {
                    this->parse_package();
                    break;
				} else {
					continue;
                }
			}
		}
	}

}

void PingRange::parse_package()
{
	struct icmphdr *icmpHeader;
	icmpHeader = (struct icmphdr *)(this->receiveBuffer + sizeof(struct iphdr));
	if (icmpHeader->type == 0) {
		printf(" [ONLINE]\n");
    } else {
		printf(" [OFFLINE]\n");
    }
}

void PingRange::send_icmp_request(struct sockaddr_in &sender)
{
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
	if (sendto(this->icmp_socket->get_socket(), &packet_to_send, sizeof(packet_to_send), 0, (struct sockaddr*)&sender, sizeof(struct sockaddr)) <= 0) {
		perror("ERROR: failed to send ICMP request\n");
        return;
    } else {
        struct hostent *he = gethostbyaddr(&sender.sin_addr, sizeof(sender.sin_addr), PF_INET);
		if (he != NULL) {
   			printf("%s", he->h_name);
        } else {
			printf("%s", inet_ntoa(sender.sin_addr));
        }
	}
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