#include <algorithm>
#include <cassert>
#include <cstddef>
#include <numeric>
#include <utility>
#include <vector>

int find_pivot(std::vector<int>& vector, std::size_t left_index, std::size_t right_index) {
    std::size_t middle_index = left_index + (right_index - 1 - left_index) / 2;

    int first = vector[left_index];
    int middle = vector[middle_index];
    int last = vector[right_index - 1];

    if ((first <= middle && middle <= last) ||
        (last <= middle && middle <= first)) {
        return middle;
    }
    if ((middle <= first && first <= last) ||
        (last <= first && first <= middle)) {
        return first;
    }
    return last;
}

size_t split(std::vector<int>& vector, std::size_t left, std::size_t right) {
    int pivot = find_pivot(vector, left, right);

    std::size_t left_iter = left;
    std::size_t right_iter = right - 1;

    while (true) {
        while (vector[left_iter] < pivot) {
            ++left_iter;
        }
        while (vector[right_iter] > pivot) {
            if (right_iter == 0) {
                break;
            }
            --right_iter;
        }
        if (left_iter >= right_iter) {
            return right_iter;
        }
        std::swap(vector[left_iter], vector[right_iter]);
        ++left_iter;
        if (right_iter > 0) {
            --right_iter;
        }
    }
}

void quicksort(std::vector<int>& data, std::size_t left, std::size_t right) {
    if (right - left <= 1) {
        return;
    }

    std::size_t pivot_index = split(data, left, right);

    if (pivot_index > left) {
        quicksort(data, left, pivot_index + 1);
    }

    quicksort(data, pivot_index + 1, right);
}

void sort(std::vector<int>& data) {
    quicksort(data, 0, data.size());
}

int main() {
    const std::size_t kVectorSize = 1000;
    std::vector<int> data(kVectorSize, 0);

    for (auto index = 0; index < kVectorSize; ++index) {
        data[index] = static_cast<int>(kVectorSize - index);
    }

    sort(data);

    assert(std::ranges::is_sorted(data));

    return 0;
}
