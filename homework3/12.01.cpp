#include <algorithm>
#include <iomanip>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>

int main() {
    std::string line;
    std::cout << "Enter amount in RUB (e.g., 1234.56 RUB): ";
    std::getline(std::cin, line);

    std::string number_str;
    for (char c : line) {
        if (std::isdigit(c) || c == '.' || c == ',') {
            number_str += c;
        } else if (c == '-' && number_str.empty()) {
            number_str += c;
        } else if (c == ' ' || c == '\t') {
            break;
        }
    }
    
    std::replace(number_str.begin(), number_str.end(), ',', '.');
    
    long double rub_amount = 0.0;
    try {
        rub_amount = std::stold(number_str);
    } catch (...) {
        std::cerr << "Error parsing number\n";
        return 1;
    }

    constexpr long double rate = 0.011L;
    long double usd_amount = rub_amount * rate;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << rub_amount << " RUB = " << usd_amount << " USD" << std::endl;

    return 0;
}