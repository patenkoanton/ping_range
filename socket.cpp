#include <string>
#include <cstring>  // strerror
#include <netdb.h>
#include <unistd.h> // close()
#include <iostream>
#include <linux/filter.h>
#include "socket.h"
#include "subnet.h"


// NOTE: we pass target_subnet by value because Socket::Socket shares the ownership of the object.
// It's being passed by reference to other Socket::members because we don't want them to own the object.
Socket::Socket(const std::shared_ptr<Subnet> target_subnet)
{
    // Open socket
    if (this->open_socket() < 0) {
        throw std::string("failed to open socket.");
    }

    // Configure socket
    if (this->configure_socket(target_subnet) < 0) {
        this->close_socket();
        throw std::string("failed to configure socket.");
    }
}


Socket::~Socket()
{
    this->close_socket();
}


// If failed to send - prints warning and exits with error.
int Socket::send_packet(const void *packet, size_t size, std::shared_ptr<IPAddress> dest)
{
    // Structure includes destination host IP address info
    sockaddr_in dest_info = {
        .sin_family = AF_INET,
        .sin_port = htons(0),
        .sin_addr = *dest->to_addr(),
    };

    if (sendto(this->hsocket, packet, size, 0, (sockaddr *)&dest_info, sizeof(sockaddr)) < 0) {
        this->show_general_warning();
        return -1;
    }
    
    return 0;
}


// Returns number of bytes read from socket.
// Prints warning when recv() fails.
ssize_t Socket::receive_packet(std::vector<char> &buffer)
{
    auto bytes_received = recv(this->hsocket, buffer.data(), buffer.capacity(), 0);
    if (bytes_received < 0) {
        // We use a non-blocking socket so we can ignore blocking-related error codes.
        if (errno != EWOULDBLOCK && errno != EAGAIN) {
            this->show_general_warning();
        }
    }

    return bytes_received;
}


int Socket::open_socket()
{
    protoent *protocol = NULL;
    if ((protocol = getprotobyname("icmp")) == NULL) {
        std::cerr << "ERROR: failed to get ICMP protocol info." << std::endl;
        return -1;
    }

    this->hsocket = socket(AF_INET, SOCK_RAW | SOCK_NONBLOCK, protocol->p_proto);
    if (this->hsocket < 0) {
        std::cerr << "ERROR: " << std::strerror(errno) << ". ";
        std::cerr << "Try running 'make install' before executing ping_subnet." << std::endl;
        std::cerr << "See README for more info." << std::endl;
        return -1;
    }

    return 0;
}


int Socket::configure_socket(const std::shared_ptr<Subnet> &target_subnet)
{
    // Apply subnet filter
    if (this-apply_subnet_bpf_filter(target_subnet) < 0) {
        std::cerr << "ERROR: failed to apply BPF filter." << std::endl;     // TODO: change to warning???
        return -1;
    }

    return 0;
}


/* BPF packet filter forces socket to only capture packets from certain subnet.
 * This ensures that ping_subnet wont be capturing irrelevant packets destined for another instance of ping running on the same machine.
 * Detailed bug desription: see commit 15dc408.
 * More info on BPF:
 *      https://www.kernel.org/doc/html/latest/networking/filter.html
 */
int Socket::apply_subnet_bpf_filter(const std::shared_ptr<Subnet> &target_subnet)
{
    auto bitmask = target_subnet->bitmask;
    auto subnet_ip = target_subnet->subnet->to_host();
    struct sock_filter code[] = {
        { 0x30, 0, 0, 0x00000009    },     // ldb      [09]            // load protocol type located at 9-th byte of packet (see struct iphdr)
        { 0x15, 0, 4, 0x00000001    },     // jeq      #0x1            // packet protocol type should be ICMP (0x01) (see /etc/protocols), otherwise drop packet
        { 0x20, 0, 0, 0x0000000c    },     // ld       [12]            // extract source IP
        { 0x54, 0, 0, bitmask       },     // and      #0xbitmask     // apply subnet mask
        { 0x15, 0, 1, subnet_ip     },     // jeq      #0xsubnet      // if different subnet - drop packet
        { 0x6, 0, 0, 0x00040000     },      // ret      #262144         // return packet
        { 0x6, 0, 0, 0x00000000     },      // ret      #0              // drop packet
    };

    struct sock_fprog my_bpf = {
        .len = sizeof(code) / sizeof(sock_filter),
        .filter = code,
    };

    if (setsockopt(this->hsocket, SOL_SOCKET, SO_ATTACH_FILTER, &my_bpf, sizeof(my_bpf)) < 0) {
        std::cerr << "ERROR: could not apply socket filter" << std::endl;
        return -1;
    }

    return 0;
}


void Socket::close_socket()
{
    close(this->hsocket);
}


void Socket::show_general_warning()
{
    std::cout << "WARNING: " << std::strerror(errno) << ". ";
}
