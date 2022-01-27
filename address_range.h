#include <string>
#include <vector>

class AddressRange {
    std::vector<std::string> address_range;     // vector with generated IP addresses.

    std::string get_address(std::string &address_and_mask);
    int get_mask(std::string &address_and_mask);
    void generate_address_range(std::string address, int mask);
    void address_string_to_octets(std::string address, uint32_t & address_number);
    void address_octets_to_string(std::string & address, uint32_t address_number);
public:
    AddressRange(std::string &address_and_mask);
    const std::vector<std::string> & get_address_range();
};