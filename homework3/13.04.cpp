#include <chrono>
#include <filesystem>
#include <format>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

auto make_type(std::filesystem::file_status const& status) {
    if (std::filesystem::is_directory(status)) {
        return 'd';
    }
    if (std::filesystem::is_regular_file(status)) {
        return 'f';
    }
    if (std::filesystem::is_symlink(status)) {
        return 'l';
    }
    return '?';
}

auto make_permissions(std::filesystem::perms permissions) -> std::string {
    auto lambda = [permissions](auto x, auto y) {
        return (permissions & x) == std::filesystem::perms::none ? '-' : y;
    };

    return {lambda(std::filesystem::perms::owner_read, 'r'),
            lambda(std::filesystem::perms::owner_write, 'w'),
            lambda(std::filesystem::perms::owner_exec, 'x'),
            lambda(std::filesystem::perms::group_read, 'r'),
            lambda(std::filesystem::perms::group_write, 'w'),
            lambda(std::filesystem::perms::group_exec, 'x'),
            lambda(std::filesystem::perms::others_read, 'r'),
            lambda(std::filesystem::perms::others_write, 'w'),
            lambda(std::filesystem::perms::others_exec, 'x')};
}

std::string format_size(std::uintmax_t size) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit_index = 0;
    double display_size = static_cast<double>(size);
    
    while (display_size >= 1024.0 && unit_index < 4) {
        display_size /= 1024.0;
        unit_index++;
    }
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << display_size << " " << units[unit_index];
    return oss.str();
}

std::uintmax_t calculate_directory_size(const std::filesystem::path& path) {
    std::uintmax_t total_size = 0;
    
    try {
        if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
            for (auto const& entry : std::filesystem::recursive_directory_iterator(
                     path, std::filesystem::directory_options::skip_permission_denied)) {
                if (!std::filesystem::is_directory(entry.status())) {
                    try {
                        total_size += std::filesystem::file_size(entry);
                    } catch (...) {}
                }
            }
        }
    } catch (...) {}
    
    return total_size;
}

void show(std::filesystem::path const& path,
          std::optional<std::regex> name_filter = std::nullopt) {
    try {
        if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
            std::cout << "Listing directory: " << std::filesystem::absolute(path) << "\n\n";
            std::cout << "Type Permissions     Size      Last Modified      Name\n";
            std::cout << "---- ----------- -------- ---------------------- ----\n";
            
            for (auto const& entry : std::filesystem::directory_iterator(path)) {
                try {
                    std::string name = entry.path().filename().string();

                    if (name_filter.has_value() &&
                        !std::regex_match(name, name_filter.value())) {
                        continue;
                    }

                    auto status = entry.status();
                    auto last_write = std::chrono::file_clock::to_sys(entry.last_write_time());
                    auto time_t = std::chrono::system_clock::to_time_t(last_write);
                    
                    std::uintmax_t size = 0;
                    if (std::filesystem::is_regular_file(status)) {
                        size = std::filesystem::file_size(entry);
                    } else if (std::filesystem::is_directory(status)) {
                        size = calculate_directory_size(entry.path());
                    }
                    
                    std::cout << "  " << make_type(status) << "   "
                              << std::left << std::setw(10) << make_permissions(status.permissions())
                              << std::right << std::setw(8) << format_size(size) << "   "
                              << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "   "
                              << name << "\n";
                } catch (const std::exception& e) {
                    std::cerr << "Error processing entry: " << e.what() << "\n";
                }
            }
        } else {
            std::cerr << "Path does not exist or is not a directory: " << path << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error accessing directory: " << e.what() << "\n";
    }
}

int main(int argc, char* argv[]) {
    std::optional<std::regex> filter;
    std::filesystem::path search_path = std::filesystem::current_path();

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg[0] != '-') {
            search_path = arg;
        } else if (arg == "-f" || arg == "--filter") {
            if (i + 1 < argc) {
                try {
                    filter = std::regex(argv[++i]);
                } catch (const std::regex_error& e) {
                    std::cerr << "Invalid regular expression: " << e.what() << "\n";
                    return 1;
                }
            }
        } else if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: " << argv[0] << " [path] [-f|--filter regex]\n";
            std::cout << "  path    - Directory to list (default: current directory)\n";
            std::cout << "  -f regex - Filter files by regular expression\n";
            return 0;
        }
    }

    show(search_path, filter);
    return 0;
}