#include "include/genetic_algorithm.hpp"
#include "base.hpp"

#include <stdexcept>
#include <random>
#include <chrono>
#include <utility>
#include <cmath>

namespace genetic
{

    _threadImplRunAgentFunction::_threadImplRunAgentFunction(iagent_function_t aft, size_t number_threads)
            : _threads_pool(number_threads)
            , _agent_function(std::move(aft)) {}

    GenotypeStore _threadImplRunAgentFunction::simulate_agent_env(const GenotypeStore &current_population) {
        auto work_population = current_population;
        for (auto agent: work_population) {
            _threads_pool.add([this, &agent]() {
                agent(this->_agent_function(&agent));
            });
        }
        return work_population;
    }

    _implGeneticAlgorithm::_implGeneticAlgorithm(param_t algorithm_param, iagent_function_t aft,
                                                 genotype_generator_t gen, size_t number_threads)
            : _threadImplRunAgentFunction(std::move(aft), number_threads)
            , _algorithm_param(algorithm_param)
            , _genotype_generator(std::move(gen))
            , _generate(algorithm_param.gen_param.rand_seed) {}

    GenotypeStore _implGeneticAlgorithm::run_evolution(const GenotypeStore &current_population) {
        auto runned_population = simulate_agent_env(current_population);

        auto selected_population = select(runned_population);

        crossover(selected_population, runned_population);

        mutate(selected_population);
        return runned_population;
    }

    GenotypeStore _implGeneticAlgorithm::select(const GenotypeStore &population) const {
        auto scores = population.get_scores();
        _sort_by_results(scores);

        auto new_population = GenotypeStore(_algorithm_param.ev_param.n_safe_agents);
        for (size_t i = 0; i < _algorithm_param.ev_param.n_safe_agents; ++i) {
            new_population[i] = population[i];
        }

        return new_population;
    }

    void _implGeneticAlgorithm::crossover(GenotypeStore &new_population, const GenotypeStore &old_population) {
        base::generator_t generate(_algorithm_param.gen_param.rand_seed);

        auto ranks_distribution = _get_ranks_distribution(old_population.get_scores());

        while (new_population.size() < _algorithm_param.ev_param.n_agents - _algorithm_param.ev_param.n_random_agents) {
            auto first_parent_i = ranks_distribution(generate);
            auto second_parent_i = ranks_distribution(generate);
            while (second_parent_i == first_parent_i) {
                second_parent_i = ranks_distribution(generate);
            }

            new_population.add_genotype(
                    old_population[first_parent_i].cross(old_population[second_parent_i].get_genotype()));
        }

        std::uniform_real_distribution<base::decimal_t> dist(0.0f, 1.0f);

        for (size_t i = 0; i < _algorithm_param.ev_param.n_random_agents; ++i) {
            if (dist(generate) < 0.5f) {
                new_population.add_genotype(generate_genome());
            } else {
                auto new_genome = new_population[0].copy();
                new_genome->mutate(_generate);
                new_population.add_genotype(std::move(new_genome));
            }
        }
    }

    void _implGeneticAlgorithm::mutate(GenotypeStore &population) {
        std::uniform_real_distribution<base::decimal_t> dist(0.0f, 1.0f);

        for (size_t i = _algorithm_param.ev_param.n_safe_agents; i < population.size(); ++i) {
            if (dist(_generate) < _algorithm_param.gen_param.chance_agent_mutation) {
                population[i].mutate([this, &dist]() -> base::decimal_t {
                    return dist(_generate);
                });
            }
        }
    }


    void _implGeneticAlgorithm::_sort_by_results(GenotypeStore::score_store_t &population) const {
        std::sort(population.begin(), population.end(), [this](const auto &lhs, const auto &rhs) {
            return _algorithm_param.ev_param.is_minimize_score ? lhs.first > rhs.first : lhs.first < rhs.first;
        });
    }

    std::discrete_distribution<size_t>
    _implGeneticAlgorithm::_get_ranks_distribution(const GenotypeStore::score_store_t &population) {
        std::vector<base::decimal_t> scores_ranks;
        scores_ranks.reserve(population.size());

        for (const auto &score: population) {
            scores_ranks.push_back((base::decimal_t) score.first);
        }

        auto len = scores_ranks.size();
        base::decimal_t sum = 0.;
        std::transform(scores_ranks.begin(), scores_ranks.end(), scores_ranks.begin(), [len, &sum](auto score) {
            auto res = std::exp(-(base::decimal_t) score / (base::decimal_t) len * 4);
            sum += res;
            return res;
        });

        std::transform(scores_ranks.begin(), scores_ranks.end(), scores_ranks.begin(), [sum](auto score) {
            return score / sum;
        });


        return {scores_ranks.begin(), scores_ranks.end()};
    }

    std::unique_ptr<IGenotype> _implGeneticAlgorithm::generate_genome()  {
        std::uniform_real_distribution<base::decimal_t> dist(0.0f, 1.0f);
        return _genotype_generator([this, &dist]() -> base::decimal_t {
            return dist(_generate);
        });
    }

    GenotypeStore _implGeneticAlgorithm::init_population() {
        GenotypeStore population;
        population.reserve(_algorithm_param.ev_param.n_agents);

        for (size_t i = 0; i < _algorithm_param.ev_param.n_agents; ++i) {
            population.add_genotype(generate_genome());
        }

        return population;
    }

/*
    math_t GeneticAlgorithm::_chance_gen_mutation = 0.2f;
    math_t GeneticAlgorithm::_mutation_amount = 1.0f;
    math_t GeneticAlgorithm::_chance_got_new_gen = 0.4f;
    math_t GeneticAlgorithm::_chance_agent_mutation = 0.8f;


    math_t GeneticAlgorithm::chance_gen_mutation() {
        return _chance_gen_mutation;
    }

    math_t GeneticAlgorithm::mutation_amount() {
        return _mutation_amount;
    }

    math_t GeneticAlgorithm::chance_got_new_gen() {
        return _chance_got_new_gen;
    }

    math_t GeneticAlgorithm::chance_agent_mutation() {
        return _chance_agent_mutation;
    }

    void GeneticAlgorithm::update(std::vector<Agent> &agents) {
        std::sort(agents.begin(), agents.end());
    }

    std::vector<Agent> GeneticAlgorithm::_defualt_recombinate(const std::vector<Agent> &agents, base::count_t count) {
        if (agents.size() < 2) {
            throw std::invalid_argument("For recombination needed two or more agents");
        }
        base::genererator_t generate(GeneticAlgorithm::seed);

        std::vector<Agent> new_population = agents;
        while (new_population.size() < count) {
            std::shuffle(new_population.begin(), new_population.end(), generate);
            new_population.push_back(new_population[0] * new_population[1]);
        }

        return new_population;
    }

    std::vector<Agent> GeneticAlgorithm::_defualt_select(const std::vector<Agent> &agents) {
        return {agents[0], agents[1]};
    }

    std::vector<Agent> GeneticAlgorithm::_initialize(base::count_t count) {
        return std::vector<Agent>();
    }*/
}