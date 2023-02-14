#include <iostream>
#include <string>
#include <netinet/ip_icmp.h>    // icmphdr
#include <thread>
#include <map>
#include <unistd.h>     // read()
#include "ping.h"
#include "subnet.h"
#include "socket.h"
#include "factory.h"

#define HOST_TIMEOUT_SEC            (2)     // expect host to reply within this time
#define ICMP_REPLY_EXPECTED_SIZE    (sizeof(iphdr) + sizeof(icmphdr))


Ping::Ping(std::shared_ptr<Subnet> subnet)
{
    this->subnet = subnet;
    this->socket = factory_create_object<Socket>(subnet);
    if (this->socket == nullptr) {
        throw std::string("failed to create Socket object.");
    }
}


void Ping::ping()
{
    std::thread sender(&Ping::sender_thread, this);
    std::thread receiver(&Ping::receiver_thread, this);
    std::thread timer(&Ping::timer_thread, this);
    std::thread printer(&Ping::printer_thread, this);

    sender.join();
    printer.join();
    timer.join();
    receiver.~thread();
}


void Ping::sender_thread()
{
    for (auto &host : this->subnet->hosts) {
        if (this->send_icmp_request(host) < 0) {
            std::cout << "WARNING: failed to send ICMP request." << std::endl;
            continue;
        }

        std::lock_guard<std::mutex> guard(this->my_mutex);
        this->pending_hosts.push_back({host, pending, std::chrono::system_clock::now()});
    }
}


void Ping::receiver_thread()
{
    std::vector<char> receive_buffer(ICMP_REPLY_EXPECTED_SIZE);
    while (1) {
        auto bytes_received = read(this->socket->hsocket, receive_buffer.data(), ICMP_REPLY_EXPECTED_SIZE);        // TODO: put read() inside Socket class.
        if (bytes_received != ICMP_REPLY_EXPECTED_SIZE) {
            continue;
        }
        auto ip_header = (iphdr *)receive_buffer.data();
        auto replier = factory_create_object<IPAddress, uint32_t>(ntohl(ip_header->saddr));

        // Only set status if the host is pending.
        // If status is set to online/offline - assume the reply has already been received before.
        std::lock_guard<std::mutex> guard(this->my_mutex);
        for (auto &host_it : this->pending_hosts) {
            if (*replier == *host_it.host) {
                if (host_it.status == pending) {
                    host_it.status = this->parse_host_status(receive_buffer);
                }
                break;
            }
        }
    }
}


void Ping::timer_thread()
{
    while (1) {
        std::lock_guard<std::mutex> guard(this->my_mutex);
        for (auto &pending_it : this->pending_hosts) {
            if (pending_it.status == pending) {
                std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - pending_it.send_time;
                if (elapsed_seconds.count() >= HOST_TIMEOUT_SEC) {
                    pending_it.status = offline;
                }
            }
        }
    }
}


void Ping::printer_thread()
{
    while (1) {
        std::lock_guard<std::mutex> guard(this->my_mutex);
        auto pending_it = this->pending_hosts.begin();
        while (pending_it != this->pending_hosts.end()) {
            if (pending_it->status == pending) {
                break;
            }
            this->show_host_status(pending_it->host, pending_it->status);
            this->pending_hosts.erase(pending_it++);
        }
    }
}


// Read host status from ICMP reply payload.
host_status_t Ping::parse_host_status(const std::vector<char> &receive_buffer) const
{
    auto icmp_header = (icmphdr *)(receive_buffer.data() + sizeof(iphdr));
    if (icmp_header->type == 0) {
        return online;
    }

    return offline;
}


// Prints host IP and online status in a readable form.
void Ping::show_host_status(std::shared_ptr<IPAddress> &host, host_status_t status) const
{
    std::map<host_status_t, std::string> status_string = {
        { pending, "[PENDING]" },
        { online, "[ONLINE]" },
        { offline, "[OFFLINE]" },
    };

    std::cout << host->to_string() << " ";

    auto hostname = host->to_hostname();
    if (!hostname.empty()) {
        std::cout << '(' << hostname << ") ";
    }

    std::cout << status_string[status] << std::endl;
}


int Ping::send_icmp_request(std::shared_ptr<IPAddress> &dest_host) const
{
    // Fill the packet
    icmphdr icmp_header = {
        .type = ICMP_ECHO,          // Echo request
        .code = 0,                  // Code 0 is required
        .checksum = 0,              // Initial checksum has to be zero
    };
    icmp_header.checksum = this->generate_internet_checksum(&icmp_header, sizeof(icmp_header));

    // Send
    return this->socket->send_packet(&icmp_header, sizeof(icmp_header), dest_host);
}


uint16_t Ping::generate_internet_checksum(const void *packet, int packet_size) const
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
