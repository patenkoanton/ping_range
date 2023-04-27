#include <iostream>
#include <string>
#include <thread>
#include <map>
#include "ping.h"
#include "subnet.h"
#include "socket.h"
#include "factory.h"


Ping::Ping(OutputStream &stream) : output_stream(stream), socket(factory_create_object<Socket, OutputStream&>(this->output_stream))
{
    if (this->socket == nullptr) {
        throw std::string("failed to open socket.");
    }
}


void Ping::ping(const Subnet &subnet)
{
    this->init(subnet);

    std::thread sender(&Ping::sender_thread, this);
    std::thread receiver(&Ping::receiver_thread, this);
    std::thread timer(&Ping::timer_thread, this);
    std::thread finalizer(&Ping::finalizer_thread, this);

    sender.join();
    receiver.join();
    timer.join();
    finalizer.join();

    this->running = false;
}


// Return % of finalized hosts if running. Otherwise return -1.
int Ping::get_progress()
{
    if (this->running == false) {
        return -1;
    }
    const auto ratio = (float)this->finalized_hosts / (float)this->subnet->hosts.size();
    return (int)(ratio * 100);
}


void Ping::init(const Subnet &subnet)
{
    this->subnet = std::make_shared<Subnet>(subnet);
    this->pending_hosts.clear();
    this->finalized_hosts = 0;
    this->running = true;
    this->stop_requested = false;
    if (this->socket->configure(*this->subnet) < 0) {
        // Non-critical. Ignore for now.
    }
}


// Sends ICMP request to every host in the subnet.
void Ping::sender_thread()
{
    for (auto host : this->subnet->hosts) {
        if (this->send_icmp_request(*host) < 0) {
            this->output_stream << "WARNING: failed to send ICMP request." << std::endl;
            continue;
        }

        std::lock_guard<std::mutex> guard(this->global_ping_mutex);
        this->pending_hosts.push_back({host, pending, std::chrono::system_clock::now()});
    }
}


// Wait for ICMP reply from every 'pending' host.
void Ping::receiver_thread()
{
    std::vector<char> receive_buffer(this->receive_buffer_size);
    while (this->keep_running()) {
        auto bytes_received = this->socket->receive_packet(receive_buffer);
        if (bytes_received != this->icmp_reply_expected_size) {
            continue;
        }
        const auto ip_header = (iphdr *)receive_buffer.data();
        const auto replier = factory_create_object<IPAddress, uint32_t>(ntohl(ip_header->saddr));

        // Only set status if the host is pending.
        // If status is set to online/offline - assume the reply has already been received before.
        std::lock_guard<std::mutex> guard(this->global_ping_mutex);
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


// Sets host to 'offline' if no reply received within timeout.
void Ping::timer_thread()
{
    while (this->keep_running()) {
        std::lock_guard<std::mutex> guard(this->global_ping_mutex);
        for (auto &pending_it : this->pending_hosts) {
            if (pending_it.status == pending) {
                std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - pending_it.send_time;
                if (elapsed_seconds.count() >= this->host_timeout_sec) {
                    pending_it.status = offline;
                }
            }
        }
    }
}


// Prints host status and removes it from 'pending' list.
void Ping::finalizer_thread()
{
    while (this->keep_running()) {
        std::lock_guard<std::mutex> guard(this->global_ping_mutex);
        auto pending_it = this->pending_hosts.begin();
        while (pending_it != this->pending_hosts.end() && this->keep_running()) {
            if (pending_it->status == pending) {
                break;
            }
            this->show_host_status(*pending_it->host, pending_it->status);
            this->pending_hosts.erase(pending_it++);
            this->finalized_hosts++;
        }
    }
}


// Returns 'true' until we ping (and finalize) all hosts, unless 'stop' requested.
bool Ping::keep_running()
{
    return (this->finalized_hosts < this->subnet->hosts.size()) && !this->stop_requested;
}

void Ping::stop()
{
    this->stop_requested = true;
}


// Read host status from ICMP reply payload.
host_status_t Ping::parse_host_status(const std::vector<char> &receive_buffer) const
{
    auto icmp_header = (const icmphdr *)(receive_buffer.data() + sizeof(iphdr));
    if (icmp_header->type == 0) {
        return online;
    }

    return offline;
}


// Prints host IP and online status in a readable form.
void Ping::show_host_status(const IPAddress &host, host_status_t status) const
{
    const std::map<host_status_t, std::string> status_string = {
        { pending, "[PENDING]" },
        { online, "[ONLINE]" },
        { offline, "[OFFLINE]" },
    };

    this->output_stream << host.to_string() << " " << status_string.at(status) << " ";

    const auto hostname = host.to_hostname();
    if (!hostname.empty()) {
        this->output_stream << "(" << hostname << ") ";
    }
    this->output_stream << std::endl;
}


int Ping::send_icmp_request(const IPAddress &dest_host) const
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
