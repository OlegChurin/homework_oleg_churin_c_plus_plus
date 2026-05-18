#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <regex>
#include <string>
#include <vector>

using namespace std::literals;

int main() {
    auto text = R"(Please find below 
        the updated contact list for our upcoming global 
        marketing campaign. Ensure all automated notifications 
        are routed correctly.
        General Inquiries: info@global-corp.com
        Customer Support: support.team2026@helpdesk.net
        For direct escalations, you can reach out to our 
        regional managers at their personal work addresses:
        North America: john.doe@global-corp.com)"s;

    std::regex pattern(
        R"(([a-zA-Z0-9._%+\-]+)@([a-zA-Z0-9.\-]+\.[a-zA-Z]{2,}))",
        std::regex_constants::icase);

    std::vector<std::string> emails;
    std::vector<std::string> domains;

    auto begin =
        std::sregex_iterator(std::cbegin(text), std::cend(text), pattern);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it) {
        const std::smatch& match = *it;
        emails.push_back(match[0].str());
        domains.push_back(match[2].str());
    }

    assert(emails == std::vector<std::string>({"info@global-corp.com",
                                               "support.team2026@helpdesk.net",
                                               "john.doe@global-corp.com"}));
    assert(domains ==
           std::vector<std::string>(
               {"global-corp.com", "helpdesk.net", "global-corp.com"}));

    std::cout << "Emails:\n";
    for (const auto& e : emails) {
        std::cout << "  " << e << '\n';
    }
    std::cout << "\nDomains:\n";
    for (const auto& d : domains) {
        std::cout << "  " << d << '\n';
    }

    return 0;
}