#include <string>
#include <cstring>  // strerror
#include <netdb.h>
#include <unistd.h> // close()
#include <iostream>
#include <linux/filter.h>
#include "socket.h"
#include "subnet.h"
#include "custom_exception.h"


Socket::Socket(OutputStream &stream) : output_stream(stream)
{
    protoent *protocol = NULL;
    if ((protocol = getprotobyname("icmp")) == NULL) {
        throw CustomException("Failed to get ICMP protocol info.");
    }

    this->hsocket = socket(AF_INET, SOCK_RAW | SOCK_NONBLOCK, protocol->p_proto);
    if (this->hsocket < 0) {
        std::cerr << std::endl;
        std::cerr << "Try running 'make install' before running the app." << std::endl;
        std::cerr << "See README for more info." << std::endl;

        throw CustomException(std::strerror(errno));
    }
}


Socket::~Socket()
{
    this->close_socket();
}


// If failed to send - prints warning and exits with error.
int Socket::send_packet(const void *packet, size_t size, const IPAddress &dest) const
{
    // Structure includes destination host IP address info
    const sockaddr_in dest_info = {
        .sin_family = AF_INET,
        .sin_port = htons(0),
        .sin_addr = *dest.to_addr(),
    };

    if (sendto(this->hsocket, packet, size, 0, (const sockaddr *)&dest_info, sizeof(sockaddr)) < 0) {
        this->show_errno();
        return -1;
    }
    
    return 0;
}


// Returns number of bytes read from socket.
// Prints warning when recv() fails.
ssize_t Socket::receive_packet(std::vector<char> &buffer) const
{
    const auto bytes_received = recv(this->hsocket, buffer.data(), buffer.capacity(), 0);
    if (bytes_received < 0) {
        // We use a non-blocking socket so we can ignore blocking-related error codes.
        if (errno != EWOULDBLOCK && errno != EAGAIN) {
            this->show_errno();
        }
    }

    return bytes_received;
}


int Socket::configure(const Subnet &target_subnet) const
{
    // Apply subnet filter
    if (this-apply_subnet_bpf_filter(target_subnet) < 0) {
        this->output_stream << "ERROR: failed to apply BPF filter." << std::endl;     // TODO: change to warning???
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
int Socket::apply_subnet_bpf_filter(const Subnet &target_subnet) const
{
    const auto bitmask = target_subnet.bitmask;
    const auto subnet_ip = target_subnet.subnet->to_host();
    sock_filter code[] = {
        { 0x30, 0, 0, 0x00000009    },     // ldb      [09]            // load protocol type located at 9-th byte of packet (see struct iphdr)
        { 0x15, 0, 4, 0x00000001    },     // jeq      #0x1            // packet protocol type should be ICMP (0x01) (see /etc/protocols), otherwise drop packet
        { 0x20, 0, 0, 0x0000000c    },     // ld       [12]            // extract source IP
        { 0x54, 0, 0, bitmask       },     // and      #0xbitmask     // apply subnet mask
        { 0x15, 0, 1, subnet_ip     },     // jeq      #0xsubnet      // if different subnet - drop packet
        { 0x6, 0, 0, 0x00040000     },      // ret      #262144         // return packet
        { 0x6, 0, 0, 0x00000000     },      // ret      #0              // drop packet
    };

    const sock_fprog bpf_filter = {
        .len = sizeof(code) / sizeof(sock_filter),
        .filter = code,
    };

    if (setsockopt(this->hsocket, SOL_SOCKET, SO_ATTACH_FILTER, &bpf_filter, sizeof(bpf_filter)) < 0) {
        return -1;
    }

    return 0;
}


void Socket::close_socket() const
{
    close(this->hsocket);
}


void Socket::show_errno() const
{
    this->output_stream << "WARNING: " << std::strerror(errno) << ". ";
}
