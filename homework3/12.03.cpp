#include <iostream>
#include <string>
#include <string_view>
#include <vector>

std::string_view longest_palindrome(std::string_view s) {
    const size_t n = s.size();
    if (n == 0) {
        return s;
    }

    std::vector<std::vector<bool>> dp(n, std::vector<bool>(n, false));

    size_t max_len = 1;
    size_t start = 0;

    for (size_t i = 0; i < n; ++i) {
        dp[i][i] = true;
    }

    for (size_t i = 0; i < n - 1; ++i) {
        if (s[i] == s[i + 1]) {
            dp[i][i + 1] = true;
            start = i;
            max_len = 2;
        }
    }

    for (size_t len = 3; len <= n; ++len) {
        for (size_t i = 0; i + len <= n; ++i) {
            const size_t j = i + len - 1;
            if (s[i] == s[j] && dp[i + 1][j - 1]) {
                dp[i][j] = true;
                if (len > max_len) {
                    max_len = len;
                    start = i;
                }
            }
        }
    }

    return s.substr(start, max_len);
}

int main() {
    std::string input;
    std::cout << "Enter a string: ";
    std::getline(std::cin, input);
    std::string_view result = longest_palindrome(input);
    std::cout << "Longest palindrome: " << result << std::endl;
    return 0;
}