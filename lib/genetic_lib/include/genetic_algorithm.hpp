#pragma once

#ifdef USE_CONCEPTS
#include <concepts>
#else

#include <type_traits>

#endif

#include <functional>
#include <cstdint>
#include <memory>

#include "include/genetiic_interfaces.hpp"
#include "include/parallel.hpp"
#include "include/genetic_utilits_contaner.hpp"

namespace genetic
{
    using namespace utgenetic;

    struct genetic_param_t
    {
        base::decimal_t chance_agent_mutation = 0.8f;
        size_t rand_seed = 5;
    };

    struct evolution_param_t
    {
        size_t n_agents = 30;
        size_t n_safe_agents = 2;
        size_t n_random_agents = 2;
        bool is_minimize_score = false;
    };

    struct param_t
    {
        evolution_param_t ev_param;
        genetic_param_t gen_param;
    };

    class _threadImplRunAgentFunction
    {
    protected:
        typedef std::function<base::score_t(IGenotype const *const)> iagent_function_t;

        explicit _threadImplRunAgentFunction(iagent_function_t aft, size_t number_threads = 20);

        GenotypeStore simulate_agent_env(const GenotypeStore &current_population);

        virtual ~_threadImplRunAgentFunction() = default;

    private:

        prll::ThreadsPool _threads_pool;
        iagent_function_t _agent_function;
    };

    class _implGeneticAlgorithm : private _threadImplRunAgentFunction
    {
    protected:

        typedef std::function<std::unique_ptr<IGenotype>(
                const base::rand_generator_t &rand_generator)> genotype_generator_t;

        _implGeneticAlgorithm(param_t algorithm_param, iagent_function_t aft, genotype_generator_t gen,
                              size_t number_threads = 20);

        GenotypeStore run_evolution(const GenotypeStore &current_population);

        [[nodiscard]] GenotypeStore select(const GenotypeStore &population) const;

        void crossover(GenotypeStore &new_population, const GenotypeStore &old_population);

        void mutate(GenotypeStore &population);

        [[nodiscard]] std::unique_ptr<IGenotype> generate_genome();

        [[nodiscard]] GenotypeStore init_population();

        ~_implGeneticAlgorithm() override = default;

        param_t _algorithm_param;

    private:
        void _sort_by_results(GenotypeStore::score_store_t &population) const;

        static std::discrete_distribution<size_t>
        _get_ranks_distribution(const GenotypeStore::score_store_t &population);

        genotype_generator_t _genotype_generator;
        base::generator_t _generate;
    };

#ifdef USE_CONCEPTS
    template<typename T>
    concept GenomeType = requires(T v) {
        { &v } -> std::convertible_to<IGenotype *>;
    };

    template<GenomeType genome_t>
#else
    template<typename genome_t>
#endif
    class GeneticAlgorithm : private _implGeneticAlgorithm
    {
    public:
        typedef std::function<base::score_t(genome_t)> agent_function_t;

    public:
        explicit GeneticAlgorithm(param_t algorithm_param,
                                  agent_function_t aft = [](genome_t) -> base::score_t { return 0; },
                                  size_t number_threads = 20)
                : _implGeneticAlgorithm(algorithm_param, [aft](IGenotype const *const ptr) -> base::score_t {
            return aft(*static_cast<genome_t const *>(ptr));
        }, generate_genome, number_threads) {}

        void run_evolution() {
            auto population = _implGeneticAlgorithm::init_population();
            population = _implGeneticAlgorithm::run_evolution(population);
        }


    private:
        static std::unique_ptr<IGenotype> generate_genome(const base::rand_generator_t &rand_generator) {
            auto genome = std::make_unique<genome_t>();
            genome->init(rand_generator);

            return genome;
        }
    };

}