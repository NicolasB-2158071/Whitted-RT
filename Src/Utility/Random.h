#pragma once

#include <random>

double RandomDouble() {
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    std::mt19937 generator;
    return distribution(generator);
}