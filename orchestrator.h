#pragma once
#include <string>
#include "output_stream_base.h"

class Orchestrator {
    std::string address_and_mask;
    OutputStreamBase &output_stream;
public:
    Orchestrator(std::string address_and_mask, OutputStreamBase &stream) :
                address_and_mask(address_and_mask),
                output_stream(stream) {};
    int start();
};