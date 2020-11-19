#ifndef _PING_RANGE_H_
#define _PING_RANGE_H_

#include <string>
#include <vector>

class PingRange {
private:
    std::vector<std::string> addresses_to_ping;
    std::string get_address(std::string address_and_mask);
    int get_mask(std::string address_and_mask);
    void generate_address_range(std::string address, int mask);
public:
    bool initialize(std::string address_and_mask);
};

#endif