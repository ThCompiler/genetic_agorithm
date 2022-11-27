#pragma once
#include <vector>
#include <random>
#include <fstream>
#include "NeuronalLayer.h"

namespace bs = base_network;

namespace genetic
{
class Genotype {
public:
    Genotype();

    Genotype(std::vector<bs::neuron_t> genome);

    void create_random_genotype(long number_gen, float min_val, float max_val, base::seed_t seed);

    const std::vector<bs::neuron_t>& get_genom() const;

    bool is_zero() const;

    Genotype operator*(const Genotype& parent) const;

    bool operator==(const Genotype& genotype) const;

    void save(std::ofstream& file) const;

    void load(std::ifstream& file);
private:
    std::vector <bs::neuron_t> _genome;
};
}