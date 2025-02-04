#pragma once

#include <random>

class Random
{
public:
    Random()
        : m_rng(std::random_device{}()) {}

    int GetInt(int min, int max)
    {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(m_rng);
    }

    float GetFloat(float min, float max)
    {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(m_rng);
    }

private:
    std::mt19937 m_rng;
};
