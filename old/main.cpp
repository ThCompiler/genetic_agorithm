#include "Evolution.h"

int main() {
    std::random_device rd;

    std::mt19937::result_type seed = rd() ^ (
        (std::mt19937::result_type)
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
            ).count() +
        (std::mt19937::result_type)
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
            ).count());

    genetic::Evolution evolution(seed);// { "nn1.bin", "nn2.bin" };

    evolution.evolution();
    //evolution.load("nn8.bin");
    return 0;
}