#include <random>
#include <chrono>
#include "GeneticAlgorithm.h"
#include "Genotype.h"
#include "Serialize/stream_writer.h"
#include "Serialize/stream_read.h"

namespace genetic
{

Genotype::Genotype()
    : _genome() {}

Genotype::Genotype(std::vector<bs::neuron_t> genome) {
    _genome = genome;
}

void Genotype::create_random_genotype(long number_gen, bs::neuron_t min_val, bs::neuron_t max_val, base::seed_t seed) {
    _genome = std::vector<bs::neuron_t>(number_gen, 0.0);

    base::genererator_t generate(seed);

    std::uniform_real_distribution<bs::neuron_t> dist(min_val, max_val);

    for (auto& gen : _genome) {
        gen = dist(generate);
    }
}

const std::vector<bs::neuron_t>& Genotype::get_genom() const {
    return _genome;
}

bool Genotype::is_zero() const {
    return _genome.size() == 0;
}

Genotype Genotype::operator*(const Genotype& parent) const {
    Genotype child(_genome);

    std::uniform_real_distribution<bs::neuron_t> dist(0.0f, 1.0f);

    base::genererator_t generator(GeneticAlgorithm::seed);

    for (size_t i = 0; i < child._genome.size(); ++i) {
        if (dist(generator) < GeneticAlgorithm::chance_got_new_gen()) {
            child._genome[i] = parent._genome[i];
        }
    }

    if (dist(generator) < GeneticAlgorithm::chance_agent_mutation()) {
        for (auto& gen : child._genome) {
            if (dist(generator) < GeneticAlgorithm::chance_gen_mutation()) {
                gen = dist(generator) * (GeneticAlgorithm::mutation_amount() * 2) - GeneticAlgorithm::mutation_amount();
            }
        }
    }
    return child;
}

bool Genotype::operator==(const Genotype& genotype) const {
    return _genome == genotype._genome;
}

void Genotype::save(std::ofstream& file) const {
    binary::write(file, _genome);
}

void Genotype::load(std::ifstream& file) {
    binary::read(file, _genome);
}
}