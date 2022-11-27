#include <vector>
#include <stdexcept>

#include "include/genetic_utilits_contaner.hpp"

namespace utgenetic
{

//====================================GenotypeStore::Genotype==========================================================

    void GenotypeStore::Genotype::init(const base::rand_generator_t &rnd_generator) {
        if (!_genotype.expired()) {
            _genotype.lock()->init(rnd_generator);
        }

        throw GenotypeExpiredException("Genotype is expired with index " + std::to_string(_index));
    }

    size_t GenotypeStore::Genotype::size() const {
        if (!_genotype.expired()) {
            return _genotype.lock()->size();
        }
        return 0;
    }

    std::unique_ptr<IGenotype> GenotypeStore::Genotype::copy() const {
        if (!_genotype.expired()) {
            return _genotype.lock()->copy();
        }
        return nullptr;
    }

    std::unique_ptr<IGenotype> GenotypeStore::Genotype::cross(const std::weak_ptr<IGenotype> &genotype) const {
        if (!_genotype.expired()) {
            return _genotype.lock()->cross(genotype);
        }
        return nullptr;
    }

    GenotypeStore::Genotype::Genotype(std::weak_ptr<IGenotype> &&genotype, size_t index, add_score_t add_score_function)
            : with_score(index, std::move(add_score_function))
            , _genotype(std::move(genotype)) {}

    const std::weak_ptr<IGenotype> &GenotypeStore::Genotype::get_genotype() const {
        return _genotype;
    }

    GenotypeStore::Genotype::Genotype() noexcept
            : with_score()
            , _genotype(std::shared_ptr<IGenotype>(nullptr)) {}

    GenotypeStore::Genotype::Genotype(GenotypeStore::Genotype &&gn) noexcept
            : with_score(gn._index, std::move(gn._add_score_function))
            , _genotype(std::move(gn._genotype)) {}

    GenotypeStore::Genotype &GenotypeStore::Genotype::operator=(GenotypeStore::Genotype &&gn) noexcept {
        _genotype = std::move(gn._genotype);
        _index = gn._index;
        _add_score_function = std::move(gn._add_score_function);
        return *this;
    }

    void GenotypeStore::Genotype::swap(GenotypeStore::Genotype &other) noexcept {
        std::swap(_genotype, other._genotype);
        std::swap(_index, other._index);
        std::swap(_add_score_function, other._add_score_function);
    }

    void swap(GenotypeStore::Genotype lhs, GenotypeStore::Genotype rhs) {
        lhs.swap(rhs);
    }

    void swap(GenotypeStore::Genotype &lhs, GenotypeStore::Genotype &rhs) {
        lhs.swap(rhs);
    }

    void GenotypeStore::Genotype::mutate(const base::rand_generator_t &rnd_generator) {
        if (!_genotype.expired()) {
            _genotype.lock()->mutate(rnd_generator);
        }

        throw GenotypeExpiredException("Genotype is expired with index " + std::to_string(_index));
    }


//====================================GenotypeStore::with_score========================================================


    GenotypeStore::with_score::with_score() noexcept
            : _index(0)
            , _add_score_function(nullptr) {}

    GenotypeStore::with_score::with_score(size_t index, GenotypeStore::add_score_t add_score_function)
            : _index(index)
            , _add_score_function(std::move(add_score_function)) {}

    void GenotypeStore::with_score::operator()(base::score_t score) {
        _add_score_function(_index, score);
    }

    void GenotypeStore::with_score::add_score(base::score_t score) {
        _add_score_function(_index, score);
    }


//====================================GenotypeStore::iterator==========================================================


    GenotypeStore::iterator::reference GenotypeStore::iterator::operator*() const {
        return Genotype(std::weak_ptr<IGenotype>(*_m_ptr), _index, _add_score_function);
    }

    GenotypeStore::iterator::pointer GenotypeStore::iterator::operator->() {
        return _m_ptr;
    }

    GenotypeStore::iterator &GenotypeStore::iterator::operator++() {
        _m_ptr++;
        return *this;
    }

    const GenotypeStore::iterator GenotypeStore::iterator::operator++(int) {
        GenotypeStore::iterator tmp = *this;
        ++(*this);
        return tmp;
    }

    GenotypeStore::iterator &GenotypeStore::iterator::operator--() {
        _m_ptr--;
        return *this;
    }

    const GenotypeStore::iterator GenotypeStore::iterator::operator--(int) {
        GenotypeStore::iterator tmp = *this;
        --(*this);
        return tmp;
    }

    bool GenotypeStore::iterator::operator==(const GenotypeStore::iterator &iter) const {
        return _m_ptr == iter._m_ptr;
    };

    bool GenotypeStore::iterator::operator!=(const GenotypeStore::iterator &iter) const {
        return _m_ptr != iter._m_ptr;
    };


    GenotypeStore::iterator::iterator(GenotypeStore::iterator::pointer ptr, size_t index, add_score_t add_function)
            : with_score(index, std::move(add_function))
            , _m_ptr(ptr) {}

    const GenotypeStore::iterator::pointer &GenotypeStore::iterator::base() const {
        return _m_ptr;
    }

    GenotypeStore::iterator::reference
    GenotypeStore::iterator::operator[](GenotypeStore::iterator::difference_type _n) const noexcept {
        return Genotype(_m_ptr[_n], _index, _add_score_function);
    }

    const GenotypeStore::iterator &
    GenotypeStore::iterator::operator+=(GenotypeStore::iterator::difference_type _n) noexcept {
        _m_ptr += _n;
        _index += _n;
        return *this;
    }

    GenotypeStore::iterator
    GenotypeStore::iterator::operator+(GenotypeStore::iterator::difference_type _n) const noexcept {
        return iterator(_m_ptr + _n, _index + _n, _add_score_function);
    }

    const GenotypeStore::iterator &
    GenotypeStore::iterator::operator-=(GenotypeStore::iterator::difference_type _n) noexcept {
        _m_ptr -= _n;
        _index -= _n;
        return *this;
    }

    GenotypeStore::iterator
    GenotypeStore::iterator::operator-(GenotypeStore::iterator::difference_type _n) const noexcept {
        return iterator(_m_ptr - _n, _index - _n, _add_score_function);
    }


//====================================GenotypeStore::const_iterator====================================================

    GenotypeStore::const_iterator::reference GenotypeStore::const_iterator::operator*() const {
        return Genotype(std::weak_ptr<IGenotype>(*_m_ptr), 0, nullptr);
    }

    GenotypeStore::const_iterator::pointer GenotypeStore::const_iterator::operator->() {
        return _m_ptr;
    }

    GenotypeStore::const_iterator &GenotypeStore::const_iterator::operator++() {
        _m_ptr++;
        return *this;
    }

    const GenotypeStore::const_iterator GenotypeStore::const_iterator::operator++(int) {
        GenotypeStore::const_iterator tmp = *this;
        ++(*this);
        return tmp;
    }

    GenotypeStore::const_iterator &GenotypeStore::const_iterator::operator--() {
        _m_ptr--;
        return *this;
    }

    const GenotypeStore::const_iterator GenotypeStore::const_iterator::operator--(int) {
        GenotypeStore::const_iterator tmp = *this;
        --(*this);
        return tmp;
    }

    bool GenotypeStore::const_iterator::operator==(const const_iterator &iter) const {
        return _m_ptr == iter._m_ptr;
    };

    bool GenotypeStore::const_iterator::operator!=(const const_iterator &iter) const {
        return _m_ptr != iter._m_ptr;
    };

    GenotypeStore::const_iterator::const_iterator(GenotypeStore::const_iterator::pointer ptr)
            : _m_ptr(ptr) {}

    const GenotypeStore::const_iterator::pointer &GenotypeStore::const_iterator::base() const {
        return _m_ptr;
    }

    GenotypeStore::const_iterator::reference
    GenotypeStore::const_iterator::operator[](GenotypeStore::const_iterator::difference_type _n) const noexcept {
        return Genotype(_m_ptr[_n], 0, nullptr);
    }

    const GenotypeStore::const_iterator &
    GenotypeStore::const_iterator::operator+=(GenotypeStore::const_iterator::difference_type _n) noexcept {
        _m_ptr += _n;
        return *this;
    }

    GenotypeStore::const_iterator
    GenotypeStore::const_iterator::operator+(GenotypeStore::const_iterator::difference_type _n) const noexcept {
        return const_iterator(_m_ptr + _n);
    }

    const GenotypeStore::const_iterator &
    GenotypeStore::const_iterator::operator-=(GenotypeStore::const_iterator::difference_type _n) noexcept {
        _m_ptr -= _n;
        return *this;
    }

    GenotypeStore::const_iterator
    GenotypeStore::const_iterator::operator-(GenotypeStore::const_iterator::difference_type _n) const noexcept {
        return const_iterator(_m_ptr - _n);
    }

//==================================================GenotypeStore======================================================

    GenotypeStore::GenotypeStore(size_t size)
            : _genotypes(size) {}

    GenotypeStore::iterator GenotypeStore::begin() {
        return iterator(_genotypes.begin(), 0, [this](size_t index, base::score_t score) {
            _scores[index] = {score, index};
        });
    }

    GenotypeStore::iterator GenotypeStore::end() {
        return iterator(_genotypes.end(), _genotypes.size() - 1, [this](size_t index, base::score_t score) {
            _scores[index] = {score, index};
        });
    }

    GenotypeStore::const_iterator GenotypeStore::begin() const {
        return const_iterator(_genotypes.begin());
    }

    GenotypeStore::const_iterator GenotypeStore::end() const {
        return const_iterator(_genotypes.end());
    }

    GenotypeStore::reverse_iterator GenotypeStore::rbegin() {
        return std::reverse_iterator(begin());
    }

    GenotypeStore::const_reverse_iterator GenotypeStore::rbegin() const {
        return std::reverse_iterator(begin());
    }

    GenotypeStore::reverse_iterator GenotypeStore::rend() {
        return std::reverse_iterator(end());
    }

    GenotypeStore::const_reverse_iterator GenotypeStore::rend() const {
        return std::reverse_iterator(end());
    }

    size_t GenotypeStore::size() const {
        return _genotypes.size();
    }

    const GenotypeStore::Genotype GenotypeStore::operator[](size_t index) const {
        return Genotype(_genotypes[index], 0, nullptr);
    }

    GenotypeStore::Genotype GenotypeStore::operator[](size_t index) noexcept {
        return Genotype(_genotypes[index], 0, [this](size_t index, base::score_t score) {
            _scores[index] = {score, index};
        });
    }

    GenotypeStore::GenotypeStore(const GenotypeStore &store) {
        *this = store;
    }

    GenotypeStore &GenotypeStore::operator=(const GenotypeStore &store) {
        this->_genotypes.clear();

        for (const auto& genotype: store._genotypes) {
            _genotypes.push_back(std::shared_ptr<IGenotype>(genotype->copy()));
        }

        _scores = store._scores;

        return *this;
    }

    void GenotypeStore::add_score(size_t genotype_index, base::score_t score) {
        if (genotype_index > size()) {
            throw std::out_of_range("Genotype index is out of range");
        }

        _scores[genotype_index] = {score, genotype_index};
    }

    void GenotypeStore::add_score(GenotypeStore::iterator &iter, base::score_t score) {
        iter(score);
    }

    void GenotypeStore::reserve(size_t new_cap) {
        _genotypes.reserve(new_cap);
        _scores.reserve(new_cap);
    }

    void GenotypeStore::resize(size_t count) {
        _genotypes.resize(count);
        _scores.resize(count);
    }

    void GenotypeStore::clear() {
        _genotypes.clear();
        _scores.clear();
    }

    const GenotypeStore::score_store_t &GenotypeStore::get_scores() const {
        return _scores;
    }

    GenotypeStore::GenotypeStore()
            : _genotypes(0)
            , _scores(0) {}
}