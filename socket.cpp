#include <string>
#include <cstring>  // strerror
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <linux/filter.h>
#include <memory>
#include "socket.h"
#include "subnet.h"


// NOTE: we pass target_subnet by value because Socket::Socket shares the ownership of the object.
// It's being passed by reference to other Socket::members because we don't want them to own the object.
Socket::Socket(int socket_timeout_sec, const std::shared_ptr<Subnet> target_subnet)
{
    // Open socket
    if (this->open_socket() < 0) {
        throw std::string("failed to open socket.");
    }

    // Configure socket
    if (this->configure_socket(socket_timeout_sec, target_subnet) < 0) {
        this->close_socket();
        throw std::string("failed to configure socket.");
    }
}


int Socket::open_socket()
{
    protoent *protocol = NULL;
    if ((protocol = getprotobyname("icmp")) == NULL) {
        std::cerr << "ERROR: failed to get ICMP protocol info." << std::endl;
        return -1;
    }

    this->hsocket = socket(AF_INET, SOCK_RAW, protocol->p_proto);
    if (this->hsocket < 0) {
        std::cerr << "ERROR: " << std::strerror(errno) << ". ";
        std::cerr << "Try using 'sudo' or applying CAP_NET_RAW capability to ping_subnet executable. ";
        std::cerr << "See 'man capabilities' and 'man setcap' for more info." << std::endl;
        return -1;
    }

    return 0;
}


int Socket::configure_socket(int socket_timeout_sec, const std::shared_ptr<Subnet> &target_subnet)
{
    timeval timeValue = {
        .tv_sec = socket_timeout_sec,
        .tv_usec = 0,
    };

    // Set socket timeout
    if (setsockopt(this->hsocket, SOL_SOCKET, SO_RCVTIMEO, &timeValue, sizeof(timeValue)) < 0) {
        std::cerr << "ERROR: setsockopt error. " << std::strerror(errno) << std::endl;
        return -1;
    }

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
 * Method arguments:
 *      > subnet address
 *      > bitmask (e.g. 0xFFFF0000 for /16 mask) in host order
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
        // std::cout << "WARNING: " << std::strerror(errno) << ". ";
        return -1;
    }
    
    return 0;
}


/* Returns number of bytes received if successful.
 * If failed:
 *      0   - no reply
 *      -1  - general error
 */
ssize_t Socket::receive_packet(char *buffer, size_t size)
{
    auto bytes_received = recvfrom(this->hsocket, buffer, size, 0, NULL, NULL);
    if (bytes_received < 0) {
        // std::cout << "WARNING: " << std::strerror(errno) << ". ";
        if (errno == EWOULDBLOCK) {
            return 0;      // No reply before the socket timeout. Host is down/does not reply.
        }
        this->show_general_warning();
        return -1;
    }

    return bytes_received;
}


void Socket::show_general_warning()
{
    std::cout << "WARNING: " << std::strerror(errno) << ". ";
}
