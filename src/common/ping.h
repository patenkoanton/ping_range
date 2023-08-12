#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <list>
#include <atomic>
#include <netinet/ip_icmp.h>    // icmphdr
#include "subnet.h"
#include "socket.h"
#include "ip_address.h"
#include "output_stream.h"
#include "factory.h"

enum host_status_t {
    pending,
    online,
    offline,
};

struct pending_host {
    std::shared_ptr<IPAddress> host;
    host_status_t status;
    std::chrono::time_point<std::chrono::system_clock> send_time;
};

class Ping {
    OutputStream &output_stream;
    OutputStream &error_stream;
    std::shared_ptr<Subnet> subnet;     // subnet is passed from outside so we need shared_ptr to manage the ownership
    std::unique_ptr<Socket> socket;

    // Constants.
    const uint32_t host_timeout_sec = 2;                                            // expect host to reply within this time
    const uint32_t icmp_reply_expected_size = sizeof(iphdr) + sizeof(icmphdr);
    const uint32_t receive_buffer_size = this->icmp_reply_expected_size + 1;        // has to be bigger than ICMP packet

    // Private methods.
    void init(std::shared_ptr<Subnet> subnet);
    int send_icmp_request(const IPAddress &dest_host) const;
    host_status_t parse_host_status(const std::vector<char> &receive_buffer) const;
    void show_host_status(const IPAddress &host, host_status_t status) const;
    uint16_t generate_internet_checksum(const void *packet, int packet_size) const;

    // Parallel execution.
    void sender_thread();
    void receiver_thread();
    void timer_thread();
    void finalizer_thread();
    bool keep_running();
    std::list<pending_host> pending_hosts;
    std::mutex global_ping_mutex;
    std::atomic<uint32_t> finalized_hosts;
    std::atomic<bool> stop_requested;

public:
    Ping(OutputStream &output_stream, OutputStream &error_stream) :
        output_stream(output_stream),
        error_stream(error_stream),
        socket(Factory::make_unique<Socket, OutputStream&, OutputStream&>(output_stream, error_stream)),
        finalized_hosts(0) {};
    void ping(std::shared_ptr<Subnet> subnet);
    void stop();
    int get_progress();     // returns % of hosts finalized
};
