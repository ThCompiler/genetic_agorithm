#pragma once
#include "base.hpp"
#include <vector>
#include <functional>

namespace base_network
{
typedef float neuron_t;
typedef std::function<neuron_t(neuron_t a)> activation_func_t;

const neuron_t defualt_min_parametr = -1.0f;
const neuron_t defualt_max_parametr = 1.0f;

class NeuronalLayer {
public:
    NeuronalLayer() = delete;

    NeuronalLayer(base::count_t number_outputs, base::count_t number_inputs, base::seed_t seed = base::default_seed);

    void random_put_weights();

    std::vector<neuron_t> get_outputs(const std::vector<neuron_t>& inputs) const;

    void set_activate_function(activation_func_t activate_function);

    void set_weight_from_gen(const std::vector<neuron_t>::const_iterator& input_begin
                             , const std::vector<neuron_t>::const_iterator& input_end);
private:
    base::count_t _number_outputs;
    base::count_t _number_inputs;

    base::seed_t _seed = base::default_seed;

    std::vector<std::vector<neuron_t>> _weights;

    activation_func_t _activate_function = [](neuron_t a) { return (exp(2.0f * a) - 1.0f) / (1.0f + exp(2.0f * a)); };
};
}