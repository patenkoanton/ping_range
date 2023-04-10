#pragma once
#include <string>
#include <memory>
#include "output_stream_base.h"
#include "ping.h"

class Orchestrator {
    OutputStreamBase &output_stream;
    std::shared_ptr<Ping> ping;
public:
    Orchestrator(OutputStreamBase &stream);
    int start(std::string &address_and_mask);
    void stop();
};