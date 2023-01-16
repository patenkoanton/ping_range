#include <string>
#include <cstring>  // strerror
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <linux/filter.h>
#include "socket.h"


Socket::Socket(int socket_timeout_sec, uint32_t subnet, uint32_t bitmask)
{
    // Open socket
    if (this->open_socket() < 0) {
        throw std::string("failed to open socket.");
    }

    // Configure socket
    if (this->configure_socket(socket_timeout_sec, subnet, bitmask) < 0) {
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
        std::cerr << "ERROR: " << std::strerror(errno);
        std::cerr << ". Try using 'sudo'." << std::endl;
        return -1;
    }

    return 0;
}


int Socket::configure_socket(int socket_timeout_sec, uint32_t subnet, uint32_t bitmask)
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
    if (this-apply_subnet_bpf_filter(subnet, bitmask) < 0) {
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
 *      > subnet address in host order
 *      > bitmask (e.g. 0xFFFF0000 for /16 mask) in host order
 */
int Socket::apply_subnet_bpf_filter(uint32_t subnet, uint32_t bitmask)
{
    struct sock_filter code[] = {
        { 0x30, 0, 0, 0x00000009 },     // ldb      [09]            // load protocol type located at 9-th byte of packet (see struct iphdr)
        { 0x15, 0, 4, 0x00000001 },     // jeq      #0x1            // packet protocol type should be ICMP (0x01) (see /etc/protocols), otherwise drop packet
        { 0x20, 0, 0, 0x0000000c },     // ld       [12]            // extract source IP
        { 0x54, 0, 0, bitmask },        // and      #0xbitmask     // apply subnet mask
        { 0x15, 0, 1, subnet },         // jeq      #0xsubnet      // if different subnet - drop packet
        { 0x6, 0, 0, 0x00040000 },      // ret      #262144         // return packet
        { 0x6, 0, 0, 0x00000000 },      // ret      #0              // drop packet
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


int Socket::get_socket()
{
    return this->hsocket;
}
