#include <cmath>
#include <iostream>
#include <optional>
#include <utility>
#include <variant>

struct Visitor {
    void operator()(double root) const { 
        std::cout << root << std::endl; 
    }

    void operator()(const std::pair<double, double>& roots) const {
        std::cout << roots.first << ' ' << roots.second << std::endl;
    }

    void operator()(std::monostate) const { 
        std::cout << "0=0" << std::endl; 
    }
};

std::optional<std::variant<double, std::pair<double, double>, std::monostate>>
compute(double a, double b, double c) {
    const double eps = 1e-9;

    if (std::abs(a) < eps) {
        if (std::abs(b) < eps) {
            if (std::abs(c) < eps) {
                return std::variant<double, std::pair<double, double>,
                                    std::monostate>{std::monostate{}};
            }
            return std::nullopt;
        }
        return std::variant<double, std::pair<double, double>,
                            std::monostate>{-c / b};
    }

    if (std::abs(b) < eps) {
        if (std::abs(c) < eps) {
            return std::variant<double, std::pair<double, double>,
                                std::monostate>{std::pair<double, double>{0.0, 0.0}};
        }
        double ratio = -c / a;
        if (ratio < -eps) {
            return std::nullopt;
        }
        if (std::abs(ratio) < eps) {
            return std::variant<double, std::pair<double, double>,
                                std::monostate>{std::pair<double, double>{0.0, 0.0}};
        }
        double sq = std::sqrt(ratio);
        return std::variant<double, std::pair<double, double>,
                            std::monostate>{std::pair<double, double>{-sq, sq}};
    }


    double D = b * b - 4 * a * c;
    if (D < -eps) {
        return std::nullopt;
    }
    
    if (std::abs(D) < eps) {
        double root = -b / 2 / a;
        return std::variant<double, std::pair<double, double>,
                            std::monostate>{root};
    }
    
    double sD = std::sqrt(D);
    double x1 = (-b - sD) / 2 / a;
    double x2 = (-b + sD) / 2 / a;
    
    if (x1 > x2) std::swap(x1, x2);
    return std::variant<double, std::pair<double, double>,
                        std::monostate>{std::pair<double, double>{x1, x2}};
}

int main() {
    double a, b, c;
    std::cin >> a >> b >> c;

    auto result = compute(a, b, c);
    if (result) {
        std::visit(Visitor{}, *result);
    } else {
        std::cout << "No roots" << std::endl;
    }

    return 0;
}