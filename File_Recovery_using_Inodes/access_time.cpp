#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <string>
#include <vector>

using namespace std;
using namespace std::chrono;

double measure_access_time(const string& filename) {
    ifstream file(filename, ios::binary | ios::ate);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return -1;
    }

    streamsize size = file.tellg();
    file.seekg(0, ios::beg);

    random_device rd; // Using a hardware-based random number generator
    mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    uniform_int_distribution<> dis(0, size - 1);

    const int num_reads = 100;
    vector<double> times;

    // 100 random read operations
    for (int i = 0; i < num_reads; ++i) {
        auto pos = dis(gen);
        file.seekg(pos);

        char buffer;
        auto start = high_resolution_clock::now();
        file.read(&buffer, 1);
        auto end = high_resolution_clock::now();

        duration<double, micro> access_time = end - start;
        times.push_back(access_time.count());
    }

    file.close();

    double total_time = 0;
    for (double t : times) {
        total_time += t;
    }

    return total_time / num_reads;
}

int main() {
    vector<string> filenames = {"file_kb.txt", "file_mb.txt", "file_gb.txt"};

    for (const string& filename : filenames) {
        double avg_time = measure_access_time(filename);
        if (avg_time >= 0) {
            cout << "Average access time for " << filename << ": " << avg_time << " microseconds" << endl;
        }
    }

    return 0;
}
