#ifndef _PING_RANGE_H_
#define _PING_RANGE_H_

#include <string>
#include <vector>

#include "address_range.h"

class PingRange {
    AddressRange address_range;
public:
    PingRange(std::string address_and_mask) : address_range(address_and_mask) {};
};

#endif