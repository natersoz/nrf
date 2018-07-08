/**
 * @file make_array.h
 *
 * Copied and modified from:
 * https://en.cppreference.com/w/cpp/experimental/to_array
 */

#pragma once

#include <array>
#include <type_traits>
#include <utility>

namespace utility
{
namespace detail
{

template <class data_type, std::size_t data_size, std::size_t... indexer>
constexpr std::array<std::remove_cv_t<data_type>, data_size>
    make_array_impl(std::remove_cv_t<data_type> const *data,
                    std::index_sequence<indexer...>)
{
    return { {data[indexer]...} };
}

} // namespace detail

/**
 * Create a std::array from a data pointer of the same type.
 * @details By using the helper detail::make_array_impl() an appropriate array
 * initializer {a[0], a[1], ... a[N]} is created and returned as a std::array
 * of the correct size and type.
 *
 * @note The passed in data pointer must be at least data_size in length.
 *       Bounds checking on the input data pointer is not performed.
 *
 * @tparam data_type The data type carried within both the created array and
 *                   the data pointer being passed in.
 * @tparam data_size The number of elements created within the returned
 *                   std::array.
 *                   The returned std::array::size() will be equal to this value.
 *
 * @param data_ptr The data pointer containing the initialization values for
 *                 the returned std::array.
 *
 * @return std::array<data_type, data_size> The std::array object initialized
 *         with data from the input data pointer.
 */
template <typename data_type, std::size_t data_size>
constexpr std::array<std::remove_cv_t<data_type>, data_size>
    make_array(std::remove_cv_t<data_type> const *data_ptr)
{
    return detail::make_array_impl<data_type, data_size>(
        data_ptr,
        std::make_index_sequence<data_size>{});
}

/**
 * Create a std::array from a data array of the same type.
 * This is preferred over make_array, since it includes bounds checking and
 * the sizing and types can be inferred at compile time.
 * @note This is the functionality as std::experimental::to_array()
 *       implemented here since clang does not yet support std::experimental.
 *
 * @tparam data_type The data type carried within both the created array and
 *                   the data pointer being passed in.
 *                   This type should be inferred by the compiler.
 * @tparam data_size The number of elements created within the returned
 *                   std::array.
 *                   The returned std::array::size() will be equal to this value.
 *                   This value should be inferred by the compiler.
 *
 * @param data[data_size] The initializing data array.
 *
 * @return std::array<data_type, data_size> The std::array object initialized
 *         with data from the input data pointer.
 */
template <typename data_type, std::size_t data_size>
constexpr std::array<std::remove_cv_t<data_type>, data_size>
    to_array(data_type const (&data)[data_size])
{
    return detail::make_array_impl<data_type, data_size>(
        data,
        std::make_index_sequence<data_size>{});
}

} // namespace utility

