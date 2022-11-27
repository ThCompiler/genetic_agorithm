#pragma once
#include "NeuronalLayer.h"
#include "Genotype.h"

namespace base_network
{
class NN {
public:
    NN(std::string path);

    NN(base::count_t number_inputs, std::vector<base::count_t> topology, base::count_t number_outputs,
       base::seed_t seed, genetic::Genotype genotype = genetic::Genotype());

    std::vector<neuron_t> get_outputs(const std::vector<neuron_t>& inputs) const;

    genetic::Genotype get_genotype() const;

    NN operator*(const NN& network) const;

    static base::count_t calculate_number_weigth(base::count_t number_inputs, std::vector<base::count_t> topology,
                                           base::count_t number_outputs);

    void operator=(const NN& from);

    void set_activate_function(activation_func_t activate_function);

    bool operator==(const NN& network) const;

    void save(std::string path) const;

    ~NN() = default;

private:
    base::count_t _number_inputs;
    base::count_t _number_outputs;

    std::vector<base::count_t> _topology;

    genetic::Genotype _genotype;

    std::vector<NeuronalLayer> _layers;

    void _set_genotype(const genetic::Genotype& genotype);
};
}