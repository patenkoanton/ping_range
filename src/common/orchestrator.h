#pragma once
#include <string>
#include <memory>
#include "output_stream.h"
#include "ping.h"

class Orchestrator {
    OutputStream &output_stream;
    std::unique_ptr<Ping> ping;
public:
    Orchestrator(OutputStream &stream) noexcept : output_stream(stream) {};
    int start(const std::string &address_and_mask) noexcept;
    void stop() noexcept;
    int get_progress() noexcept;     // returns the % of finalized hosts
};