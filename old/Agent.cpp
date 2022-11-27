#include "Agent.h"

namespace genetic
{
Agent::Agent(std::string path)
    : _neural_network(path)
    , _score(0) {}

Agent::Agent(base::count_t number_inputs, std::vector<base::count_t> topology,
             base::count_t number_outputs, base::seed_t seed)
    : _neural_network(number_inputs, topology, number_outputs, seed)
    , _score(0) {}

Agent Agent::operator*(const Agent& agent) const {
    Agent res = Agent(*this);

    res._neural_network = _neural_network * agent._neural_network;
    return res;
}
bool Agent::operator<(const Agent& agent) const {
    return _score > agent._score;
}

bool Agent::operator==(const Agent& agent) const {
    return _neural_network == agent._neural_network && _score == agent._score;
}

void Agent::add_score(score_t score) {
    _score += score;
}

void Agent::set_score(score_t score) {
    _score = score;
}
std::vector<bs::neuron_t> Agent::get_result(std::vector<bs::neuron_t> inputs) {
    return _neural_network.get_outputs(inputs);
}

long Agent::get_score() const {
    return _score;
}

void Agent::save(std::string path) const {
    _neural_network.save(path);
}
}
