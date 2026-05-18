#include <algorithm>
#include <backward/strstream>
#include <cmath>
#include <iostream>
#include <numeric>
#include <random>
#include <ranges>
#include <vector>
#include <iterator>

class Fibonacci : public std::ranges::view_interface<Fibonacci> {
public:
    Fibonacci() = default;
    explicit Fibonacci(std::size_t count) : count_(count) {}

    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = int;
        using difference_type = std::ptrdiff_t;
        using pointer = const int*;
        using reference = const int&;

        Iterator() = default;
        Iterator(std::size_t index, int prev, int curr)
            : index_(index), prev_(prev), curr_(curr) {}

        int operator*() const { return curr_; }

        Iterator& operator++() {
            const int next = prev_ + curr_;
            prev_ = curr_;
            curr_ = next;
            ++index_;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++*this;
            return tmp;
        }

        bool operator==(const Iterator& other) const {
            return index_ == other.index_;
        }

    private:
        std::size_t index_ = 0;
        int prev_ = 0;
        int curr_ = 1;
    };

    Iterator begin() const { return Iterator{0, 0, 1}; }
    Iterator end() const { return Iterator{count_, 0, 1}; }

private:
    std::size_t count_ = 0;
};

template <std::ranges::input_range R, std::weakly_incrementable O,
          typename Pred, typename Func>
O transform_if(R&& rng, O out, Pred pred, Func func) {
    for (const auto& elem : rng) {
        if (pred(elem)) {
            *out = func(elem);
            ++out;
        }
    }
    return out;
}

double mae(const std::vector<double>& pred, const std::vector<double>& truth) {
    if (pred.empty() || pred.size() != truth.size()) return 0.0;
    double sum = std::transform_reduce(
        pred.begin(), pred.end(), truth.begin(), 0.0, std::plus<>{},
        [](double p, double t) { return std::abs(p - t); });
    return sum / static_cast<double>(pred.size());
}

double mse(const std::vector<double>& pred, const std::vector<double>& truth) {
    if (pred.empty() || pred.size() != truth.size()) return 0.0;
    double sum = std::transform_reduce(
        pred.begin(), pred.end(), truth.begin(), 0.0, std::plus<>{},
        [](double p, double t) { return (p - t) * (p - t); });
    return sum / static_cast<double>(pred.size());
}

int main() {
    using namespace std;
    using namespace std::views;

    vector<int> nums = {1, 2, 3, 4, 5, 1, 2, 3, 4, 5};

    auto repl = nums;
    ranges::replace(repl, 2, 99);
    cout << "replace 2->99: ";
    for (auto x : repl) cout << x << ' ';
    cout << '\n';

    auto fillv = nums;
    ranges::fill(fillv, 0);
    cout << "fill 0: ";
    for (auto x : fillv) cout << x << ' ';
    cout << '\n';

    auto uniq = nums;
    ranges::sort(uniq);
    auto ret = ranges::unique(uniq);
    uniq.erase(ret.begin(), ret.end());
    cout << "unique: ";
    for (auto x : uniq) cout << x << ' ';
    cout << '\n';

    auto rot = nums;
    ranges::rotate(rot, rot.begin() + 3);
    cout << "rotate by 3: ";
    for (auto x : rot) cout << x << ' ';
    cout << '\n';

    vector<int> sample_out(3);
    mt19937 gen{random_device{}()};
    ranges::sample(nums, sample_out.begin(), 3, gen);
    cout << "sample 3: ";
    for (auto x : sample_out) cout << x << ' ';
    cout << '\n';

    vector<int> src = {1, 2, 3, 4, 5, 6};
    vector<int> dst;
    transform_if(
        src, back_inserter(dst), [](int x) { return x % 2 == 0; },
        [](int x) { return x * 10; });
    cout << "transform_if: ";
    for (auto x : dst) cout << x << ' ';
    cout << '\n';

    vector<double> pred = {2.5, 0.0, 2.1, 1.4};
    vector<double> truth = {3.0, -0.5, 2.0, 1.0};
    cout << "MAE: " << mae(pred, truth) << '\n';
    cout << "MSE: " << mse(pred, truth) << '\n';

    vector<int> vi = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    cout << "filter even: ";
    for (auto x : vi | views::filter([](int i) { return i % 2 == 0; }))
        cout << x << ' ';
    cout << '\n';

    cout << "drop 3: ";
    for (auto x : vi | views::drop(3)) cout << x << ' ';
    cout << '\n';

    vector<vector<int>> nested = {{1, 2}, {3, 4, 5}, {6}};
    cout << "join nested: ";
    for (auto x : nested | views::join) cout << x << ' ';
    cout << '\n';

    vector<char> letters = {'a', 'b', 'c', 'd'};
    cout << "zip with letters: ";
    auto zip_view = views::zip(vi, letters);
    for (const auto& [num, ch] : zip_view)
        cout << '(' << num << ',' << ch << ") ";
    cout << '\n';

    cout << "stride 3: ";
    for (auto x : vi | views::stride(3)) cout << x << ' ';
    cout << '\n';

    cout << "Fibonacci(10): ";
    Fibonacci fib(10);
    for (int f : fib) cout << f << ' ';
    cout << '\n';

    return 0;
}