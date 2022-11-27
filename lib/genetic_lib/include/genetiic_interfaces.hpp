#pragma once

#include "base.hpp"

#include <memory>
#include <functional>

namespace genetic
{

    class IMutable
    {
    public:
        virtual void mutate(const base::rand_generator_t &) = 0;
    };

    class IGenotype : public IMutable
    {
    public:
        virtual void init(const base::rand_generator_t &) = 0;

        [[nodiscard]] virtual size_t size() const = 0;

        [[nodiscard]] virtual std::unique_ptr<IGenotype> copy() const = 0;

        [[nodiscard]] virtual std::unique_ptr<IGenotype> cross(const std::weak_ptr<IGenotype> &genotype) const = 0;

        virtual ~IGenotype() = default;
    };


}