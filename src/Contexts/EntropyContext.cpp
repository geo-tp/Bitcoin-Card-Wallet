#include "EntropyContext.h"
#include <random>
#include <algorithm>
#include <Arduino.h> // Include Arduino for GPIO functions

namespace contexts {

// Singleton instance accessor
EntropyContext& EntropyContext::getInstance() {
    static EntropyContext instance;
    return instance;
}

void EntropyContext::collect() {
    // Current time
    auto now = std::chrono::high_resolution_clock::now();

    // Collect GPIO states from microphone (DATA and CLK)
    uint32_t mic_gpio_state = 0;
    uint8_t data_state = digitalRead(46); // Read DATA (G46)
    uint8_t clk_state = digitalRead(43); // Read CLK (G43)

    // Rotate the bits of mic_gpio_state by a random number of positions
    uint8_t random_rotation = static_cast<uint8_t>(esp_random() & 0x1F); // Random value between 0 and 31
    mic_gpio_state = (mic_gpio_state << random_rotation) | (mic_gpio_state >> (32 - random_rotation));

    // Func duration delta
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();

    // Add the combined state + duration to the accumulator
    add(mic_gpio_state + duration);
}


void EntropyContext::tick() {
    auto now = std::chrono::high_resolution_clock::now();

    // Calculate the delta between the last tick and now
    if (lastTickTime.time_since_epoch().count() != 0) {
        auto delta = std::chrono::duration_cast<std::chrono::nanoseconds>(now - lastTickTime).count();
        add(delta);
    }

    lastTickTime = now; // Update the last tick time
}

std::vector<uint8_t> EntropyContext::getAccumulatedEntropy() {
    return accumulator;
}

std::vector<uint8_t> EntropyContext::transformBytes(const std::vector<uint8_t>& bytes) {
    std::vector<uint8_t> transformedBytes = bytes;
    
    // Add some randomness in case sequences are the same
    for (auto& byte : transformedBytes) {
        byte ^= static_cast<uint8_t>(esp_random() & 0xFF); // Use only the lower 8 bits
    }
    return transformedBytes;
}

// Convert different types to bytes
template<typename T>
std::vector<uint8_t> EntropyContext::toBytes(const T& data) {
    const uint8_t* begin = reinterpret_cast<const uint8_t*>(&data);
    return std::vector<uint8_t>(begin, begin + sizeof(T));
}

std::vector<uint8_t> EntropyContext::toBytes(const std::string& data) {
    return std::vector<uint8_t>(data.begin(), data.end());
}

template<typename T>
void EntropyContext::add(const T& data) {
    auto bytes = toBytes(data);
    auto transformedBytes = transformBytes(bytes);
    accumulator.insert(accumulator.end(), transformedBytes.begin(), transformedBytes.end());

    // Limit accumulator size
    if (accumulator.size() > maxSize) {
        accumulator.erase(accumulator.begin(), accumulator.begin() + (accumulator.size() - maxSize));
    }
}

// Template specialization implementation
template void contexts::EntropyContext::add<int>(const int& data);
template void contexts::EntropyContext::add<uint32_t>(const uint32_t& data);
template void contexts::EntropyContext::add<std::vector<char>>(const std::vector<char>& data);
template void contexts::EntropyContext::add<char>(const char& data);

} // namespace contexts
