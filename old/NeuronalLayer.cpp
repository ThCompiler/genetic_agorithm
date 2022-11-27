#include "NeuronalLayer.h"
#include <random>
#include <stdexcept>
#include <chrono>

namespace base_network
{
NeuronalLayer::NeuronalLayer(base::count_t number_outputs, base::count_t number_inputs, base::seed_t seed)
    : _number_outputs(number_outputs)
    , _number_inputs(number_inputs)
    , _seed(seed)
    , _weights(number_outputs, std::vector<neuron_t>(number_inputs, 0.0)) {
    random_put_weights();
}

void NeuronalLayer::random_put_weights() {
    std::uniform_real_distribution<neuron_t> dist(defualt_min_parametr, defualt_max_parametr);
    base::genererator_t gen(_seed);

    for (auto& neuron : _weights) {
        for (auto& weight : neuron) {
            weight = dist(gen);
        }
    }
}

std::vector<float> NeuronalLayer::get_outputs(const std::vector<neuron_t>& inputs) const {
    if (_number_inputs != inputs.size()) {
        throw std::invalid_argument("Number of wait inputs not equal number of got inputs");
    }

    std::vector<neuron_t> outputs(_number_outputs, 0.0);

    size_t k = 0;
    for (const auto& neuron : _weights) {
        for (size_t i = 0; i < _number_inputs; ++i) {
            outputs[k] += inputs[i] * neuron[i];
        }
        outputs[k] += 0.2f;
        outputs[k] = _activate_function(outputs[k]);
        ++k;
    }

    return outputs;
}

void NeuronalLayer::set_activate_function(activation_func_t activate_function) {
    _activate_function = activate_function;
}

void NeuronalLayer::set_weight_from_gen(const std::vector<float>::const_iterator& input_begin
                                        , const std::vector<float>::const_iterator& input_end) {
    std::vector<float>::const_iterator iter = input_begin;

    if (input_end - iter != _number_inputs * _number_outputs) {
        throw std::invalid_argument("Gotted genotype can't be use in this neuronal layer");
    }

    for (auto& neuron : _weights) {
        for (auto& weight : neuron) {
            weight = *(iter++);
        }
    }
}
}
