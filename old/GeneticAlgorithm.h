#pragma once
#include "Agent.h"
#include <functional>

namespace genetic
{

typedef float math_t;
typedef std::function<std::vector<Agent>(const std::vector<Agent>&, base::count_t)> recombinate_func_t;
typedef std::function<std::vector<Agent>(const std::vector<Agent>&)> select_func_t;
typedef std::function<std::vector<Agent>(base::count_t)> initialize_func_t;

class GeneticAlgorithm {
public:
    static math_t chance_gen_mutation();

    static math_t mutation_amount();

    static math_t chance_got_new_gen();

    static math_t chance_agent_mutation();

    static recombinate_func_t recombinate;

    static select_func_t select;

    static base::seed_t seed;

    static initialize_func_t initialize;

    static void update(std::vector<Agent>& agents);

private:
    static math_t _chance_gen_mutation;
    static math_t _mutation_amount;
    static math_t _chance_got_new_gen;
    static math_t _chance_agent_mutation;

    static std::vector<Agent> _defualt_recombinate(const std::vector<Agent>& agents, base::count_t count);

    static std::vector<Agent> _defualt_select(const std::vector<Agent>& agents);

    static std::vector<Agent> _initialize(base::count_t count);
};
}