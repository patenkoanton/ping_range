#include <string>
#include <cstring>
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include "icmp_socket.h"


ICMPSocket::ICMPSocket(int socket_timeout_sec)
{
    // Open socket
    if (this->open_socket() < 0) {
        throw std::string("failed to open socket.");
    }

    // Configure socket
    if (this->configure_socket(socket_timeout_sec) < 0) {
        this->close_socket();
        throw std::string("failed to configure socket.");
    }
}


int ICMPSocket::open_socket()
{
	struct protoent *protocol = NULL;
	if ((protocol = getprotobyname("icmp")) == NULL) {
		std::cerr << "ERROR: failed to get ICMP protocol info." << std::endl;
		return -1;
	}

    this->hsocket = socket(AF_INET, SOCK_RAW, protocol->p_proto);
	if (this->hsocket < 0) {
        std::cerr << "ERROR: " << strerror(errno);
        std::cerr << ". Try using 'sudo'." << std::endl;
    	return -1;
    }

    return 0;
}


int ICMPSocket::configure_socket(int socket_timeout_sec)
{
    struct timeval timeValue = {
        .tv_sec = socket_timeout_sec,
	    .tv_usec = 0,
    };
	
    // Set socket timeout
    if (setsockopt(this->hsocket, SOL_SOCKET, SO_RCVTIMEO, &timeValue, sizeof(timeValue)) < 0) {
        std::cerr << "ERROR: setsockopt error. " << strerror(errno) << std::endl;
        return -1;
    }

    return 0;
}


void ICMPSocket::close_socket()
{
    close(this->hsocket);
}


ICMPSocket::~ICMPSocket()
{
    this->close_socket();
}


int ICMPSocket::get_socket()
{
    return this->hsocket;
}
