#include <cassert>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

std::string to_hex(const std::vector<std::uint8_t>& data) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (const auto& byte : data) {
        oss << std::setw(2) << static_cast<unsigned>(byte);
    }
    return oss.str();
}

std::vector<std::uint8_t> from_hex(const std::string& hex_str) {
    std::vector<std::uint8_t> data;
    if (hex_str.size() % 2 != 0) {
        throw std::invalid_argument("Hex string must have an even length");
    }

    for (std::size_t i = 0; i < hex_str.length(); i += 2) {
        char c1 = std::tolower(hex_str[i]);
        char c2 = std::tolower(hex_str[i + 1]);

        std::uint8_t val1 = 0;
        if (c1 >= '0' && c1 <= '9') {
            val1 = c1 - '0';
        } else if (c1 >= 'a' && c1 <= 'f') {
            val1 = c1 - 'a' + 10;
        } else {
            throw std::invalid_argument(std::string("Invalid hex character: ") + c1);
        }

        std::uint8_t val2 = 0;
        if (c2 >= '0' && c2 <= '9') {
            val2 = c2 - '0';
        } else if (c2 >= 'a' && c2 <= 'f') {
            val2 = c2 - 'a' + 10;
        } else {
            throw std::invalid_argument(std::string("Invalid hex character: ") + c2);
        }

        std::uint8_t byte = (val1 << 4) | val2;
        data.push_back(byte);
    }
    return data;
}

int main() {
    try {
        std::vector<std::uint8_t> original = {0x00, 0x1a, 0xff, 0x0f,
                                               0xab, 0xcd, 0x03};

        std::string hex_string = to_hex(original);
        std::cout << "Hex string: " << hex_string << std::endl;

        std::vector<std::uint8_t> decoded = from_hex(hex_string);

        assert(original == decoded);

        std::cout << "Original: ";
        for (auto b : original) {
            std::cout << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<unsigned>(b) << ' ';
        }
        std::cout << std::endl;

        std::cout << "Decoded:  ";
        for (auto b : decoded) {
            std::cout << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<unsigned>(b) << ' ';
        }
        std::cout << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}