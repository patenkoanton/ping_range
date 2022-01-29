#include <string>
#include <vector>

class AddressRange {
    std::vector<std::string> address_range;     // vector with generated IP addresses.

    std::string get_address(std::string &address_and_mask);
    int get_mask(std::string &address_and_mask);
    void generate_address_range(std::string &input_address, int mask);
    uint32_t reverse_byte_order(uint32_t mask);
    uint32_t generate_subnet_address(std::string &input_address, int mask);
public:
    AddressRange(std::string &address_and_mask);
    const std::vector<std::string> & get_address_range();
};