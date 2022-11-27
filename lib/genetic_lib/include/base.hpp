#pragma once

#include <random>
#include <functional>

#ifndef USE_CONCEPTS
    #ifndef _HAS_CXX20
        #define USE_CONCEPTS __cplusplus > 201703L && __cpp_concepts >= 201907L
    #else
        #define USE_CONCEPTS _HAS_CXX20
    #endif
#endif

namespace base
{
    typedef uint32_t count_t;

    typedef int64_t score_t;

    typedef std::mt19937::result_type seed_t;

    typedef std::mt19937 generator_t;

    typedef float decimal_t;

    typedef std::function<decimal_t()> rand_generator_t;

    const seed_t default_seed = 0;
}