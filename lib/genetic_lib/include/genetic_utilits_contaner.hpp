#pragma once

#include <vector>

#include "genetiic_interfaces.hpp"

namespace utgenetic
{
    struct GenotypeExpiredException : public std::invalid_argument
    {
        explicit GenotypeExpiredException(const std::string &what_arg)
                : std::invalid_argument(what_arg) {}
    };

    using namespace genetic;

    class GenotypeStore
    {
        static const size_t default_store_size = 10;

        typedef std::function<void(size_t, base::score_t)> add_score_t;

    public:

        typedef std::vector<std::pair<base::score_t, size_t>> score_store_t;

//====================================GenotypeStore::with_score========================================================


        struct with_score
        {
            with_score() noexcept;

            with_score(size_t index, add_score_t add_score_function);

            void operator()(base::score_t score);

            void add_score(base::score_t score);

        protected:
            size_t _index;
            add_score_t _add_score_function;
        };


//====================================GenotypeStore::Genotype==========================================================


        class Genotype : public IGenotype, public with_score
        {
        public:
            void init(const base::rand_generator_t &rnd_generator) final;

            [[nodiscard]] size_t size() const final;

            [[nodiscard]] std::unique_ptr<IGenotype> copy() const override;

            [[nodiscard]] std::unique_ptr<IGenotype> cross(const std::weak_ptr<IGenotype> &genotype) const final;

            [[nodiscard]] const std::weak_ptr<IGenotype> &get_genotype() const;

            void mutate(const base::rand_generator_t &rnd_generator) final;

            Genotype(const Genotype &) = default;

            Genotype &operator=(const Genotype &) = default;

            Genotype(Genotype &&) noexcept;

            Genotype &operator=(Genotype &&) noexcept;

            Genotype() noexcept;

            void swap(Genotype &other) noexcept;

            friend void swap(Genotype lhs, Genotype rhs);

            friend void swap(Genotype &lhs, Genotype &rhs);

        private:
            friend GenotypeStore;

            explicit Genotype(std::weak_ptr<IGenotype> &&genotype, size_t index, add_score_t add_score_function);

            std::weak_ptr<IGenotype> _genotype;
        };


//====================================GenotypeStore::const_iterator====================================================


        struct iterator : public with_score
        {
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::vector<std::shared_ptr<IGenotype>>::iterator::difference_type;
            using value_type = GenotypeStore::Genotype;
            using pointer = std::vector<std::shared_ptr<IGenotype>>::iterator;
            using reference = value_type;

            reference operator*() const;

            pointer operator->();

            iterator &operator++();

            const iterator operator++(int);

            iterator &operator--();

            const iterator operator--(int);

            bool operator==(const iterator &iter) const;

            bool operator!=(const iterator &iter) const;

            [[nodiscard]] const pointer &base() const;

            reference operator[](difference_type _n) const noexcept;

            const iterator &operator+=(difference_type _n) noexcept;

            iterator operator+(difference_type _n) const noexcept;

            const iterator &operator-=(difference_type _n) noexcept;

            iterator operator-(difference_type _n) const noexcept;

        private:
            friend GenotypeStore;

            explicit iterator(pointer ptr, size_t index, add_score_t add_function);

            pointer _m_ptr;
        };


//====================================GenotypeStore::const_iterator====================================================


        struct const_iterator
        {
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::vector<std::shared_ptr<IGenotype>>::const_iterator::difference_type;
            using value_type = const GenotypeStore::Genotype;
            using pointer = std::vector<std::shared_ptr<IGenotype>>::const_iterator;
            using reference = value_type;

            reference operator*() const;

            pointer operator->();

            const_iterator &operator++();

            const const_iterator operator++(int);

            const_iterator &operator--();

            const const_iterator operator--(int);

            bool operator==(const const_iterator &iter) const;

            bool operator!=(const const_iterator &iter) const;

            [[nodiscard]] const pointer &base() const;

            reference operator[](difference_type _n) const noexcept;

            const const_iterator &operator+=(difference_type _n) noexcept;

            const_iterator operator+(difference_type _n) const noexcept;

            const const_iterator &operator-=(difference_type _n) noexcept;

            const_iterator operator-(difference_type _n) const noexcept;

        private:
            friend GenotypeStore;

            explicit const_iterator(pointer ptr);

            pointer _m_ptr;
        };


//====================================GenotypeStore::functions========================================================


        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

        GenotypeStore();

        explicit GenotypeStore(size_t size);

        GenotypeStore(const GenotypeStore &store);

        GenotypeStore &operator=(const GenotypeStore &store);

        inline void add_genotype(std::unique_ptr<IGenotype> &&genotype) {
            _genotypes.push_back(std::shared_ptr<IGenotype>(std::move(genotype)));
        }

        void add_score(size_t genotype_index, base::score_t score);

        static void add_score(iterator &iter, base::score_t score);

        [[nodiscard]] iterator begin();

        [[nodiscard]] const_iterator begin() const;

        [[nodiscard]] iterator end();

        [[nodiscard]] const_iterator end() const;

        [[nodiscard]] reverse_iterator rbegin();

        [[nodiscard]] const_reverse_iterator rbegin() const;

        [[nodiscard]] reverse_iterator rend();

        [[nodiscard]] const_reverse_iterator rend() const;

        [[nodiscard]] size_t size() const;

        [[nodiscard]] inline size_t max_size() const noexcept {
            return _genotypes.max_size();
        }

        [[nodiscard]] inline bool empty() const noexcept {
            return _genotypes.empty();
        }

        [[nodiscard]] inline size_t capacity() const noexcept {
            return _genotypes.capacity();
        }

        void reserve(size_t new_cap);

        void resize(size_t count);

        void clear();

        [[nodiscard]] const score_store_t &get_scores() const;

        const Genotype operator[](size_t index) const;

        Genotype operator[](size_t index) noexcept;

        ~GenotypeStore() = default;

    private:
        std::vector<std::shared_ptr<IGenotype>> _genotypes;
        score_store_t _scores;
    };

    inline auto operator-(const GenotypeStore::iterator &iter,
                          const GenotypeStore::iterator &second_iter) -> decltype(iter.base() - iter.base()) {
        return iter.base() - second_iter.base();
    }

    inline auto operator-(const GenotypeStore::const_iterator &iter,
                          const GenotypeStore::const_iterator &second_iter) -> decltype(iter.base() - iter.base()) {
        return iter.base() - second_iter.base();
    }
}
