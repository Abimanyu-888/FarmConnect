#include "helper.hpp" // Include the header to ensure consistency

#include <random>
#include <array>
#include <sstream>
#include <iomanip>
#include <functional>
#include <algorithm>

/**
 * @brief Generates a random alphanumeric string to be used as a unique ID.
 *
 * This function creates an 8-character string using a thread-safe
 * (thread_local) random number generator for better performance
 * in a multi-threaded environment like a web server.
 *
 * @return A std::string of 8 random characters.
 */
std::string generate_product_id() {
    /**
     * @brief A thread-local Mersenne Twister engine.
     * 'thread_local' ensures that each thread gets its own instance
     * of the generator, avoiding the need for locks.
     * It's initialized using a lambda function the first time
     * it's accessed by a thread.
     */
    thread_local std::mt19937 generator = []{
        std::random_device rd;
        std::array<int, std::mt19937::state_size> seed_data;
        std::generate(seed_data.begin(), seed_data.end(), std::ref(rd));
        std::seed_seq seq(seed_data.begin(), seed_data.end());

        return std::mt19937(seq);
    }();

    const std::string charset = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::uniform_int_distribution<int> dist(0, charset.length() - 1);

    std::string id;
    id.reserve(8);
    for (int i = 0; i < 8; ++i) {
        id += charset[dist(generator)];
    }

    return id;
}