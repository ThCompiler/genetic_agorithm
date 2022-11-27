#include "include/genetic_algorithm.hpp"

int main() {
    genetic::GeneticAlgorithm<typename utgenetic::GenotypeStore::Genotype> alg(genetic::param_t{},
                                                                               [](const typename utgenetic::GenotypeStore::Genotype&) -> base::score_t { return 0; },
                                                                               20);// { "nn1.bin", "nn2.bin" };

    // evolution.evolution();
    //evolution.load("nn8.bin");
    return 0;
}