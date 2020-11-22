#include <vector>
#include <string>

#include "ping_range.h"

const std::vector<std::string> & PingRange::get_address_range()
{
    return this->address_range.get_address_range();
}