#pragma once
#include <string>
#include <memory>
#include "output_stream.h"
#include "ping.h"

// Prepares and runs 'ping'.
// Keeps all the exceptions contained (except for constructor which can throw in case of error).
class Orchestrator {
    OutputStream &output_stream;
    OutputStream &error_stream;
    std::unique_ptr<Ping> ping;
    std::atomic<bool> running;
    std::atomic<bool> cancelled;
public:
    Orchestrator(OutputStream &output_stream, OutputStream &error_stream);
    void start(const std::string &address_and_mask) noexcept;
    void stop() noexcept;
    int get_progress() noexcept;     // returns the % of finalized hosts
    bool is_cancelled() noexcept;
    bool is_running() noexcept;
};