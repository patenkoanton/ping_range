#include <string>
#include <vector>


class AddressRange {
    std::vector<uint32_t> address_range;     // vector with generated host addresses in network order

    int generate_address_range(std::string &input_address, int mask);
    uint32_t reverse_byte_order(uint32_t input);
    uint32_t generate_subnet_address(std::string &input_address, int mask);
    std::pair<std::string, int> parse_input_address_string(std::string &input_address_string);
public:
    AddressRange(std::string &input_address_string);
    const std::vector<uint32_t> & get_address_range();
};