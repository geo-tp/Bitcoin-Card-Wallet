#ifndef ENTROPY_CONTEXT_H
#define ENTROPY_CONTEXT_H

#include <vector>
#include <string>
#include <variant>
#include <cstdint>
#include <chrono>
#include <M5Cardputer.h>

namespace contexts {

class EntropyContext {
public:
    // Singleton instance accessor
    static EntropyContext& getInstance();

    // Collect entropy from various source
    void collect();

    // Tick method to record time deltas and add them to the accumulator
    void tick();

    // Get the accumulated entropy
    std::vector<uint8_t> getAccumulatedEntropy();

    // Add different types of entropy using a generic method
    template<typename T>
    void add(const T& data);

private:
    EntropyContext() = default; // Private constructor for singleton
    EntropyContext(const EntropyContext&) = delete;
    EntropyContext& operator=(const EntropyContext&) = delete;

    std::vector<uint8_t> accumulator;
    const size_t maxSize = 256;
    std::chrono::high_resolution_clock::time_point lastTickTime; // Store the last tick time

    // Convert different types to bytes
    template<typename T>
    std::vector<uint8_t> toBytes(const T& data);

    // Overload for std::string
    std::vector<uint8_t> toBytes(const std::string& data);

    // Simple transformation function to randomize bytes
    std::vector<uint8_t> transformBytes(const std::vector<uint8_t>& bytes);
};

} // namespace contexts

#endif // ENTROPY_CONTEXT_H
