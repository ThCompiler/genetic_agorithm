#pragma once

#include "stream.h"
#include <cstring>
#include <memory>

namespace binary
{

#if USE_CONCEPTS

    template<binary_local::serializable T, typename U = void>
    class reader
#else
        template<typename T, typename U = void>
        class reader
#endif // USE_CONCEPTS

            : public binary_local::stream<T, U>
    {
    public:
        inline static std::size_t read(std::istream &in, T &value) {
            in.read(reinterpret_cast<char *>(&value), sizeof(value));
            return static_cast<std::size_t>(in.gcount());
        }
    };


    template<typename T>
    class reader<T, binary_local::only_if_string<T>>
            : public binary_local::stream<T>
    {
    public:
        inline static std::size_t read(std::istream &in, T &value) {
            const auto start = in.tellg();
            typename reader::size_type size = 0;

            in.read(reinterpret_cast<char *>(&size), sizeof(size));

            if (size > 0) {
                auto tmp = std::make_unique<char[]>(size + 1);

                *(tmp.get() + size) = '\0';

                in.read(tmp.get(), size);

                value = tmp.get();
            }

            return static_cast<std::size_t>(in.tellg() - start);
        }
    };

    template<typename T>
    class reader<T, binary_local::only_if_boolean<T>>
            : public binary_local::stream<T>
    {
    public:
        inline static std::size_t read(std::istream &in, T &value) {
            typename reader::proxy_type val = '\0';

            in.read(reinterpret_cast<char *>(&val), sizeof(val));
            value = (val == reader::t_value);

            return static_cast<std::size_t>(in.gcount());
        }
    };

    template<typename T>
    class reader<T, binary_local::only_if_iserializable<T>>
            : public binary_local::stream<T>
    {
    public:
        inline static std::size_t read(std::istream &in, T &value) {
            return value.deserialize(in);
        }
    };

    template<typename T>
    class reader<T, binary_local::only_if_cstring<T>>
            : public binary_local::stream<T>
    {
    public:
        inline static std::size_t read(std::istream &in, T &value) {
            const auto start = in.tellg();
            typename reader::size_type size = 0;

            in.read(reinterpret_cast<char *>(&size), sizeof(size));

            if (size > 0) {
                char *tmp = new char[size + 1];
                tmp[size] = '\0';

                in.read(tmp, size);

                delete[] value;
                value = tmp;
            }

            return static_cast<std::size_t>(in.tellg() - start);
        }
    };

#if USE_CONCEPTS

    template<binary_local::sequence_container T>
    class reader<T, void>
#else
        template<typename T>
        class reader<T, binary_local::only_if_sequence_container<T>>
#endif // USE_CONCEPTS
            : public binary_local::stream<T>
    {
    public:
        inline static std::size_t read(std::istream &in, T &value) {
            typename reader::size_type size = 0;

            in.read(reinterpret_cast<char *>(&size), sizeof(size));

            auto read = sizeof(size);

            if (size > 0) {
                value.resize(size);

                for (auto &el: value) {
                    read += reader<typename reader::value_type>::read(in, el);
                }
            }

            return read;
        }
    };

    template<typename T>
    class reader<T, binary_local::only_if_pair<T>>
            : public binary_local::stream<T>
    {
    public:
        inline static std::size_t read(std::istream &in, T &value) {
            auto size = 0U;

            size += reader<typename reader::f_type>::read(in, value.first);
            size += reader<typename reader::s_type>::read(in, value.second);

            return size;
        }
    };


#if USE_CONCEPTS

    template<binary_local::associative_container T>
    class reader<T, void>
#else
        template<typename T>
        class reader<T, binary_local::only_if_associative_container<T>>
#endif // USE_CONCEPTS

            : public binary_local::stream<T>
    {
    public:
        static std::size_t read(std::istream &in, T &value) {

            typename reader::size_type size = 0;

            in.read(reinterpret_cast<char *>(&size), sizeof(size));
            auto read = sizeof(size);

            if (size > 0) {
                for (typename reader::size_type i = 0; i < size; ++i) {

                    typename reader::element<typename reader::value_type>::type el{};
                    read += reader<typename element<typename reader::value_type>::type>::read(in, el);

                    value.insert(el);
                }

            }
            return read;
        }

    private:

        template<typename U, typename Z = void>
        struct element
        {
            using type = U;
        };

        template<typename U>
        struct element<U, binary_local::only_if_pair<U>>
        {
            using type = std::pair<std::remove_const_t
                    <typename U::first_type>, typename U::second_type>;
        };
    };


#if USE_CONCEPTS

    template<binary_local::stack_container T>
    class reader<T, void>
#else
        template<typename T>
        class reader<T, binary_local::only_if_stack_container<T>>
#endif // USE_CONCEPTS

            : public binary_local::stream<T>
    {
    public:
        inline static std::size_t read(std::istream &in, T &value) {
            typename reader::size_type size = 0;

            in.read(reinterpret_cast<char *>(&size), sizeof(size));
            auto read = sizeof(size);

            T res;

            for (typename reader::size_type i = 0; i < size; ++i) {
                typename reader::value_type tmp;

                read += reader<typename reader::value_type>::read(in, tmp);
                res.push(tmp);
            }
            value = res;

            return read;
        }
    };

#if USE_CONCEPTS

    template<binary_local::queue_container T>
    class reader<T, void>
#else

        template<typename T>
        class reader<T, binary_local::only_if_queue_container<T>>
#endif // USE_CONCEPTS

            : public binary_local::stream<T>
    {
    public:
        inline static std::size_t read(std::istream &in, T &value) {
            typename reader::size_type size = 0;

            in.read(reinterpret_cast<char *>(&size), sizeof(size));
            auto read = sizeof(size);

            T res;

            for (typename reader::size_type i = 0; i < size; ++i) {
                typename reader::value_type tmp;

                read += reader<typename reader::value_type>::read(in, tmp);
                res.push(tmp);
            }
            value = res;

            return read;
        }
    };

    template<typename T>
    std::size_t read(std::istream &in, T &value) {
        return reader<T>::read(in, value);
    }
}