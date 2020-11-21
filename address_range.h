#include <string>
#include <vector>

class AddressRange {
    std::vector<std::string> address_range;     // vector of IP addresses.

    int get_mask(std::string address_and_mask);
    std::string get_address(std::string address_and_mask);
    void generate_address_range(std::string address, int mask);
public:
    AddressRange(std::string address_and_mask);
    const std::vector<std::string> & get_address_range();
};