#include "./mutator.h"
#include "./knobs.h"
#include "./defs.h"
#include <chrono>
#include <array>
#include <iostream>

namespace trooper {
  void Test() {
    // using Unix timestamp as rng seed
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();

    Mutator mutator(seed);
    Knobs& knobs = mutator.knobs();
    std::array<uint8_t, 7> knob_values = { 0, 0, 0, 0, 0, 0, 0 };
    ByteArray data = { 1, 2, 3, 4, 5, 6, 7, 8 };
    std::cout << "original data: ";
    for (auto byte : data) {
        std::cout << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;

    // test EraseBytes
    knob_values = { 1, 0, 0, 0, 0, 0, 0 };
    knobs.Set(knob_values);
    mutator.Mutate(data);
    std::cout << "erase bytes: ";
    for (auto byte : data) {
        std::cout << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;

    // test FlipBit
    knob_values = { 0, 1, 0, 0, 0, 0, 0 };
    knobs.Set(knob_values);
    mutator.Mutate(data);
    std::cout << "flip bits: ";
    for (auto byte : data) {
        std::cout << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;

    // test Swap Bytes
    knob_values = { 0, 0, 1, 0, 0, 0, 0 };
    knobs.Set(knob_values);
    mutator.Mutate(data);
    std::cout << "swap bytes: ";
    for (auto byte : data) {
        std::cout << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;

    // test ChangeByte
    knob_values = { 0, 0, 0, 1, 0, 0, 0 };
    knobs.Set(knob_values);
    mutator.Mutate(data);
    std::cout << "change bytes: ";
    for (auto byte : data) {
        std::cout << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;

    // test InsertBytes
    knob_values = { 0, 0, 0, 0, 0, 1, 0 };
    knobs.Set(knob_values);
    mutator.Mutate(data);
    std::cout << "insert bytes: ";
    for (auto byte : data) {
        std::cout << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;

    // test for probabilistic selection
    knob_values = { 122, 133, 8, 23, 123, 23, 123 };
    knobs.Set(knob_values);
    const size_t N = 10000;
    std::vector<size_t> counts(knob_values.size(), 0);

    for (size_t i = 0; i < N; ++i) {
        Rng rng(seed+i); 
        size_t chosen_knob = knobs.Choose2({ 0, 1, 2, 3, 4, 5, 6 }, rng());
        counts[chosen_knob]++;
    }
    std::cout << "test probabilistic choose: "<<std::endl;
    for (size_t i = 0; i < counts.size(); ++i) {
        std::cout << "  " << "Knob " << i << " chosen ratio: " << static_cast<double>(counts[i]) / N << std::endl;
    }
  }
} // namespace trooper

int main() {
  trooper::Test();
  return 0;
}
