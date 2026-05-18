#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <optional>
#include <print>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

std::pair<std::string, std::vector<std::string>> replace_raw_strings(
    const std::string& input) {
    std::string output = input;
    std::vector<std::string> originals;
    size_t pos = 0;

    while ((pos = output.find("R\"", pos)) != std::string::npos) {
        const size_t start = pos;
        const size_t delim_start = pos + 2;
        const size_t paren_pos = output.find('(', delim_start);
        if (paren_pos == std::string::npos) {
            ++pos;
            continue;
        }
        std::string delimiter =
            output.substr(delim_start, paren_pos - delim_start);
        std::string closing_seq = ")" + delimiter + "\"";
        size_t content_start = paren_pos + 1;
        size_t closing_pos = output.find(closing_seq, content_start);
        if (closing_pos == std::string::npos) {
            ++pos;
            continue;
        }
        size_t end = closing_pos + closing_seq.size();
        std::string raw_str = output.substr(start, end - start);
        std::string placeholder =
            "___RAWSTRPLACEHOLDER_" + std::to_string(originals.size()) + "___";
        output.replace(start, end - start, placeholder);
        originals.push_back(raw_str);
        pos = start + placeholder.size();
    }
    return {output, originals};
}

std::string restore_raw_strings(std::string input,
                                const std::vector<std::string>& originals) {
    for (size_t i = 0; i < originals.size(); ++i) {
        std::string placeholder =
            "___RAWSTRPLACEHOLDER_" + std::to_string(i) + "___";
        size_t pos = 0;
        while ((pos = input.find(placeholder, pos)) != std::string::npos) {
            input.replace(pos, placeholder.size(), originals[i]);
            pos += originals[i].size();
        }
    }
    return input;
}

void transform(std::string const& path_1, std::string const& path_2) {
    auto string =
        (std::stringstream() << std::fstream(path_1, std::ios::in).rdbuf())
            .str();

    auto [processed, raw_strings] = replace_raw_strings(string);

    for (auto iterator = std::begin(processed); iterator != std::end(processed);
         ++iterator) {
        if (*iterator == '\'') {
            do {
                ++iterator;
            } while (!(*iterator == '\'' && *std::prev(iterator) != '\\'));
        }
        if (*iterator == '\"') {
            do {
                ++iterator;
            } while (!(*iterator == '\"' && *std::prev(iterator) != '\\'));
        }
        if (*iterator == '/') {
            if (*std::next(iterator) == '/') {
                auto end = std::next(iterator, 2);
                while (end != std::end(processed) && *end != '\n') {
                    ++end;
                }
                iterator = processed.erase(iterator, end);
            } else if (*std::next(iterator) == '*') {
                auto end = std::next(iterator, 3);
                while (!(*end == '/' && *std::prev(end) == '*')) {
                    ++end;
                }
                iterator = processed.erase(iterator, ++end);
            }
        }
        if (iterator == std::end(processed)) {
            break;
        }
    }

    processed = restore_raw_strings(std::move(processed), raw_strings);

    {
        std::istringstream iss(processed);
        std::string line;
        std::string result;
        while (std::getline(iss, line)) {
            if (std::find_if_not(line.begin(), line.end(), [](unsigned char c) {
                    return std::isspace(c);
                }) != line.end()) {
                result += line + '\n';
            }
        }
        processed = std::move(result);
    }

    std::fstream(path2, std::ios::out) << processed;
}

int main() {
    auto path1 = "source.cpp";
    auto path2 = "output.cpp";

    transform(path1, path2);

    std::print("main : enter char : ");
    std::cin.get();

    std::filesystem::remove(path2);
}