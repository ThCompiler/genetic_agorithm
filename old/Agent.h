#pragma once
#include "NeuralNetwork.h"


namespace bs = base_network;

namespace genetic
{

typedef long score_t;

class Agent {
public:
    Agent(std::string path);

    Agent(base::count_t number_inputs, std::vector<base::count_t> topology,
          base::count_t number_outputs, base::seed_t seed);

    Agent operator*(const Agent& agent) const;

    virtual bool operator<(const Agent& agent) const;

    bool operator==(const Agent& agent) const;

    void add_score(score_t score);

    void set_score(score_t score);

    std::vector<bs::neuron_t> get_result(std::vector<bs::neuron_t> inputs);

    long get_score() const;

    void save(std::string path) const;

private:
    bs::NN _neural_network;

protected:
    score_t _score;
};
}