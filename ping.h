#ifndef _PING_H_
#define _PING_H_

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <list>
#include "subnet.h"
#include "socket.h"
#include "ip_address.h"
#include "output_stream_base.h"

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
    OutputStreamBase &output_stream;
    std::shared_ptr<Subnet> subnet;
    std::shared_ptr<Socket> socket;

    int send_icmp_request(std::shared_ptr<IPAddress> &dest_host) const;
    host_status_t parse_host_status(const std::vector<char> &receive_buffer) const;
    void show_host_status(std::shared_ptr<IPAddress> &host, host_status_t status) const;
    uint16_t generate_internet_checksum(const void *packet, int packet_size) const;

    // Parallel execution.
    void sender_thread();
    void receiver_thread();
    void timer_thread();
    void finalizer_thread();
    bool keep_running();

    std::list<pending_host> pending_hosts;
    uint32_t finalized_hosts = 0;
    std::mutex my_mutex;
    bool stop_requested = false;		// TODO: make it thread-safe.
public:
    Ping(std::shared_ptr<Subnet> subnet, OutputStreamBase &stream);
    void ping();
    void stop();
};

#endif
