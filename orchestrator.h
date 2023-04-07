#pragma once
#include <string>
#include "output_stream_base.h"

class Orchestrator {
    std::string address_and_mask;
public:
    Orchestrator(std::string &address_and_mask) : address_and_mask(address_and_mask) {};
    int start();
};