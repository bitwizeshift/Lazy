/**
 * \file index_list.hpp
 *
 * \brief A helpful trait for enumerating \c std::tuples
 *
 * This idea is taken from a Stack Overflow answer for constructin
 * classes from tuples
 *
 * \see http://stackoverflow.com/a/14897685
 */

#ifndef LAZY_DETAIL_INDEX_LIST_HPP_
#define LAZY_DETAIL_INDEX_LIST_HPP_

namespace lazy{

  /// \brief A small class used to unpack tuples at compile-time
  template <size_t... Is>
  struct index_list{};

  namespace detail{

    /// \brief Template helper to build a range for index_lists
    template <size_t Start, size_t N, size_t... Is>
    struct build_range;

    template <size_t Start, size_t... Is>
    struct build_range<Start, Start, Is...>{
        typedef index_list<Is...> type;
    };

    template <size_t Start, size_t N, size_t... Is>
    struct build_range
      : public build_range<Start, N - 1, N - 1, Is...>{};

  }

  /// \brief Creates a range of indices between start and end
  template<unsigned Start, unsigned End>
  using index_range = typename detail::build_range<Start, End>::type;

}

#endif /* LAZY_DETAIL_INDEX_LIST_HPP_ */
