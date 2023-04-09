#pragma once
#include <string>
#include <memory>
#include "output_stream_base.h"
#include "ping.h"

class Orchestrator {
    std::string address_and_mask;
    OutputStreamBase &output_stream;
    std::shared_ptr<Ping> ping;
public:
    Orchestrator(std::string address_and_mask, OutputStreamBase &stream) :
                address_and_mask(address_and_mask),
                output_stream(stream) {};
    int start();
    void stop();
};