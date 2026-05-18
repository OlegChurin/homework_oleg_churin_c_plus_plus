#include <algorithm>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/graph_traits.hpp>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

using namespace boost;

int main() {
    const int N = 10;
    typedef adjacency_matrix<undirectedS, no_property,
                             property<edge_weight_t, int>>
        Graph;
    Graph g(N);

    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_int_distribution<int> dist(1, 10);

    property_map<Graph, edge_weight_t>::type weight_map = get(edge_weight, g);

    // Добавляем ребра с весами
    for (int i = 0; i < N; ++i) {
        for (int j = i + 1; j < N; ++j) {
            int w = dist(gen);
            auto [ed, inserted] = boost::add_edge(i, j, g);
            weight_map[ed] = w;
        }
    }

    std::cout << "Adjacency matrix:\n";
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (i == j) {
                std::cout << " 0 ";
            } else {
                auto [ed, exists] = boost::edge(i, j, g);
                if (exists) {
                    std::cout << weight_map[ed] << ' ';
                } else {
                    std::cout << " - ";
                }
            }
        }
        std::cout << '\n';
    }

    std::vector<int> vertices(N - 1);
    std::iota(vertices.begin(), vertices.end(), 1);
    std::vector<int> best_path;
    int best_cost = std::numeric_limits<int>::max();
    int iteration = 0;
    int total_permutations = 0;
    
    // Вычисляем общее количество перестановок для информации
    long long fact = 1;
    for (int i = 2; i <= N-1; ++i) fact *= i;
    std::cout << "Total permutations to check: " << fact << "\n";

    do {
        int cost = 0;
        int prev = 0;
        bool valid_path = true;
        
        for (int v : vertices) {
            auto [ed, exists] = boost::edge(prev, v, g);
            if (!exists) {
                valid_path = false;
                break;
            }
            cost += weight_map[ed];
            prev = v;
        }
        
        if (valid_path) {
            auto [ed, exists] = boost::edge(prev, 0, g);
            if (exists) {
                cost += weight_map[ed];
                if (cost < best_cost) {
                    best_cost = cost;
                    best_path = vertices;
                }
            }
        }
        
        iteration++;
        if (iteration % 1000000 == 0) {
            std::cout << "Processed " << iteration << " permutations...\n";
        }
    } while (std::next_permutation(vertices.begin(), vertices.end()));

    if (best_cost != std::numeric_limits<int>::max()) {
        std::cout << "\nBest path:\n0 ";
        for (int v : best_path) {
            std::cout << v << ' ';
        }
        std::cout << "0\n";
        std::cout << "Total cost: " << best_cost << '\n';
    } else {
        std::cout << "No valid Hamiltonian cycle found!\n";
    }

    return 0;
}