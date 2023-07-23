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
public:
    Orchestrator(OutputStream &output_stream, OutputStream &error_stream);
    int start(const std::string &address_and_mask) noexcept;
    void stop() noexcept;
    int get_progress() noexcept;     // returns the % of finalized hosts
};