#include <cassert>
#include <cmath>
#include <istream>
#include <numeric>
#include <ostream>
#include <sstream>
#include <vector>
#include <compare>
#include <iostream>
#include <exception>
#include <variant>
#include <optional>
#include <string>


class Exception : public std::exception
{
public:
    explicit Exception(const char* message) : m_message(message) {}
    
    const char* what() const noexcept override
    {
        return m_message.c_str();
    }
    
private:
    std::string m_message;
};


template<typename Derived>
struct addable {
    friend Derived operator+(Derived lhs, const Derived& rhs) {
        return lhs += rhs;
    }
};

template<typename Derived>
struct subtractable {
    friend Derived operator-(Derived lhs, const Derived& rhs) {
        return lhs -= rhs;
    }
};

template<typename Derived>
struct multipliable {
    friend Derived operator*(Derived lhs, const Derived& rhs) {
        return lhs *= rhs;
    }
};

template<typename Derived>
struct dividable {
    friend Derived operator/(Derived lhs, const Derived& rhs) {
        return lhs /= rhs;
    }
};

template<typename Derived>
struct incrementable {
    friend Derived operator++(Derived& obj, int) {
        Derived tmp(obj);
        ++obj;
        return tmp;
    }
};

template<typename Derived>
struct decrementable {
    friend Derived operator--(Derived& obj, int) {
        Derived tmp(obj);
        --obj;
        return tmp;
    }
};


template <typename T>
class Rational : public addable<Rational<T>>,
                  public subtractable<Rational<T>>,
                  public multipliable<Rational<T>>,
                  public dividable<Rational<T>>,
                  public incrementable<Rational<T>>,
                  public decrementable<Rational<T>>
{
public:
    /* explicit */ Rational(T num = 0, T den = 1) : m_num(num), m_den(den)
    {
        if (m_den == 0)
        {
            throw Exception("Rational constructor: denominator cannot be zero");
        }
        reduce();
    }

    // -----------------------------------------------------------------------------------------
    explicit operator double() const
    {
        return 1.0 * m_num / m_den;
    }

    // -----------------------------------------------------------------------------------------
    auto& operator+=(Rational const& other)
    {
        auto lcm = std::lcm(m_den, other.m_den);
        m_num = m_num * (lcm / m_den) + other.m_num * (lcm / other.m_den);
        m_den = lcm;
        reduce();
        return *this;
    }

    // -----------------------------------------------------------------------------------------
    auto& operator-=(Rational const& other)
    {
        return *this += Rational(other.m_num * -1, other.m_den);
    }

    // -----------------------------------------------------------------------------------------
    auto& operator*=(Rational const& other)
    {
        m_num *= other.m_num;
        m_den *= other.m_den;
        reduce();
        return *this;
    }

    // -----------------------------------------------------------------------------------------
    auto& operator/=(Rational const& other)
    {
        if (other.m_num == 0)
        {
            throw Exception("Rational division: division by zero");
        }
        return *this *= Rational(other.m_den, other.m_num);
    }

    // -----------------------------------------------------------------------------------------
    auto& operator++()  
    {
        *this += 1;
        return *this;
    }

    auto& operator--()  
    {
        *this -= 1;
        return *this;
    }

    // -----------------------------------------------------------------------------------------
    friend std::strong_ordering operator<=>(Rational const& lhs, Rational const& rhs)
    {
        auto left  = lhs.m_num * rhs.m_den;
        auto right = rhs.m_num * lhs.m_den;
        if (left < right)  return std::strong_ordering::less;
        if (left > right)  return std::strong_ordering::greater;
        return std::strong_ordering::equal;
    }

    friend bool operator==(Rational const& lhs, Rational const& rhs)
    {
        return lhs.m_num == rhs.m_num && lhs.m_den == rhs.m_den;
    }

    // -----------------------------------------------------------------------------------------
    friend auto& operator>>(std::istream& stream, Rational& rational)
    {
        return (stream >> rational.m_num).ignore() >> rational.m_den;
    }

    friend auto& operator<<(std::ostream& stream, Rational const& rational)
    {
        return stream << rational.m_num << '/' << rational.m_den;
    }

private:
    void reduce()
    {
        if (m_den < 0)
        {
            m_num = -m_num;
            m_den = -m_den;
        }
        auto gcd = std::gcd(m_num, m_den);
        m_num /= gcd;
        m_den /= gcd;
    }

    T m_num = 0, m_den = 1;
};

///////////////////////////////////////////////////////////////////////////////////////////////

auto equal(double x, double y, double epsilon = 1e-6)
{
    return std::abs(x - y) < epsilon;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void demonstrateRationalExceptions()
{
    
    try
    {
        Rational<int> r1(1, 2);
        std::cerr << r1 << std::endl;
        

        std::cerr << "дробь 1/0..." << std::endl;
        Rational<int> r2(1, 0); 
        std::cerr << "ошибка" << std::endl;
    }
    catch (const Exception& e)
    {
        std::cerr << "перехвачено: " << e.what() << std::endl;
    }
    
    try
    {
        Rational<int> r3(3, 4);
        Rational<int> r4(0, 1);
        std::cerr << "деление на ноль:" << r3 << " / " << r4 << std::endl;
        auto r5 = r3 / r4;
        std::cerr << r5 << std::endl;
    }
    catch (const Exception& e)
    {
        std::cerr << "перехвачено: " << e.what() << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////

void demonstrateBadAlloc()
{

    
    try
    {
        std::vector<int> huge_vector;
   
        huge_vector.reserve(100000000000);
        std::cerr << "Память выделена успешно" << std::endl;
    }
    catch (const std::bad_alloc& e)
    {
        std::cerr << "Перехвачено std::bad_alloc: " << e.what() << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////

void demonstrateBadVariantAccess()
{
    
    try
    {
        std::variant<int, double, std::string> var = 42;  
        
        
        double d = std::get<double>(var); 
        std::cerr << "Получено значение: " << d << std::endl;
    }
    catch (const std::bad_variant_access& e)
    {
        std::cerr << "Перехвачено std::bad_variant_access: " << e.what() << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////

void demonstrateBadOptionalAccess()
{

    
    try
    {
        std::optional<int> opt;  
        

        int val = opt.value();  
        std::cerr << "Получено значение: " << val << std::endl;
    }
    catch (const std::bad_optional_access& e)
    {
        std::cerr << "Перехвачено std::bad_optional_access: " << e.what() << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////

void demonstrateVectorExceptions()
{

    
    // std::length_error
    
    try
    {
        std::vector<int> vec;
        // Попытка создать вектор с недопустимо большим размером
        vec.resize(vec.max_size() + 1);  // Здесь будет length_error
        std::cerr << "Вектор изменен успешно" << std::endl;
    }
    catch (const std::length_error& e)
    {
        std::cerr << "Перехвачено std::length_error: " << e.what() << std::endl;
    }
    
    // std::out_of_range
    
    try
    {
        std::vector<int> vec = {1, 2, 3, 4, 5};
        std::cerr << "Вектор содержит " << vec.size() << " элементов" << std::endl;
        
        // Попытка доступа к несуществующему элементу через at()
        int elem = vec.at(10);  // Здесь будет out_of_range
        std::cerr << "Получен элемент: " << elem << std::endl;
    }
    catch (const std::out_of_range& e)
    {
        std::cerr << "Перехвачено std::out_of_range: " << e.what() << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    std::cerr << "=== Демонстрация обработки исключений ===" << std::endl;
    

    try
    {
        demonstrateRationalExceptions();
        demonstrateBadAlloc();
        demonstrateBadVariantAccess();
        demonstrateBadOptionalAccess();
        demonstrateVectorExceptions();
        

    }
    catch (const std::exception& e)
    {

        std::cerr << "стандартное исключение: " << e.what() << std::endl;
    }
    catch (...)
    {
        // Обработчик для всех остальных исключений
        std::cerr << "неизвестное исключение" << std::endl;
    }
    
    std::cerr << " Программа завершена" << std::endl;
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
