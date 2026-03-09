#include <iostream>
#include <cmath>
#include <optional>
#include <variant>
#include <utility>
std::optional<std::variant<std::monostate, double, std::pair<double, double>>>
solve(double a, double b, double c, double eps = 1e-4)
{

    if (std::abs(a) < eps)
    {
        if (std::abs(b) < eps)
        {
            if (std::abs(c) < eps)
                return std::variant<std::monostate, double, std::pair<double, double>>(std::monostate{}); 
            else
                return std::nullopt; 
        }
        else
        {
            double root = -c / b;
            return std::variant<std::monostate, double, std::pair<double, double>>(root);
        }
    }
    else
    {
        
        double D = b * b - 4 * a * c;
        if (D < -eps) 
        {
            return std::nullopt;
        }
        else if (std::abs(D) < eps) 
        {
            double root = -b / (2 * a);
            return std::variant<std::monostate, double, std::pair<double, double>>(root);
        }
        else 
        {
            double sqrtD = std::sqrt(D);
            double x1 = (-b + sqrtD) / (2 * a);
            double x2 = (-b - sqrtD) / (2 * a);
            return std::variant<std::monostate, double, std::pair<double, double>>(std::make_pair(x1, x2));
        }
    }
}

int main()
{
    double a, b, c;
    std::cout << "Введите a: ";
    std::cin >> a;
    std::cout << "Введите b: ";
    std::cin >> b;
    std::cout << "Введите c: ";
    std::cin >> c;

    auto result = solve(a, b, c);

    if (!result.has_value())
    {
        std::cout << "Нет решений" << std::endl;
    }
    else
    {
        const auto& var = result.value();
        // Определяем тип по индексу variant
        switch (var.index())
        {
        case 0:
            std::cout << "Бесконечное количество решений" << std::endl;
            break;
        case 1:
            std::cout << "Один корень: " << std::get<double>(var) << std::endl;
            break;
        case 2:
            auto [x1, x2] = std::get<std::pair<double, double>>(var);
            std::cout << "Два корня: " << x1 << " и " << x2 << std::endl;
            break;
        }
    }

    return 0;
}
