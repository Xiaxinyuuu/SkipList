#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include "skip_list.h"

using namespace std;
using namespace std::chrono;

void run_performance_tests(int node_num, int repetitions) {
    SkipList skiplist;

    // Generate test data
    vector<string> keys;
    for (int i = 0; i < node_num; ++i) {
        keys.push_back("key" + to_string(i));
    }

    // Insert initial nodes
    for (const auto& key : keys) {
        skiplist.insert(key, "raw_value");
    }

    // Insert operation
    auto insert_start = high_resolution_clock::now();
    for (int i = 0; i < repetitions; ++i) {
        skiplist.insert("key" + to_string(node_num + i), "raw_value");
    }
    auto insert_end = high_resolution_clock::now();
    auto insert_duration = duration_cast<microseconds>(insert_end - insert_start).count();

    // Search operation
    auto search_start = high_resolution_clock::now();
    for (int i = 0; i < repetitions; ++i) {
        skiplist.find_by_key("key" + to_string((node_num / 2) + i));
    }
    auto search_end = high_resolution_clock::now();
    auto search_duration = duration_cast<microseconds>(search_end - search_start).count();

    // Update operation
    auto update_start = high_resolution_clock::now();
    for (int i = 0; i < repetitions; ++i) {
        skiplist.update_by_key("key" + to_string((node_num / 2) + i), "new_value");
    }
    auto update_end = high_resolution_clock::now();
    auto update_duration = duration_cast<microseconds>(update_end - update_start).count();

    // Delete operation
    auto delete_start = high_resolution_clock::now();
    for (int i = 0; i < repetitions; ++i) {
        skiplist.delete_by_key("key" + to_string((node_num / 2) + i));
    }
    auto delete_end = high_resolution_clock::now();
    auto delete_duration = duration_cast<microseconds>(delete_end - delete_start).count();

    // Output results
    cout << "Performance test results with " << node_num << " nodes and " << repetitions << " repetitions:\n";
    cout << "Insert: " << insert_duration / repetitions << " µs (average)\n";
    cout << "Search: " << search_duration / repetitions << " µs (average)\n";
    cout << "Update: " << update_duration / repetitions << " µs (average)\n";
    cout << "Delete: " << delete_duration / repetitions << " µs (average)\n";
}

int main() {
    int node_num = 10000;
    int repetitions = 10000;
    run_performance_tests(node_num, repetitions);
    return 0;
}