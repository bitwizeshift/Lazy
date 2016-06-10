/*
 * index_list.hpp
 *
 *  Created on: Jun 9, 2016
 *      Author: matthewrodusek
 */

#ifndef LAZY_DETAIL_INDEX_LIST_HPP_
#define LAZY_DETAIL_INDEX_LIST_HPP_

namespace lazy{

  template <size_t... Is>
  struct index_list{};

  namespace detail{

    template <size_t Start, size_t N, size_t... Is>
    struct range_builder;

    template <size_t Start, size_t... Is>
    struct range_builder<Start, Start, Is...>{
        typedef index_list<Is...> type;
    };

    template <size_t Start, size_t N, size_t... Is>
    struct range_builder
      : public range_builder<Start, N - 1, N - 1, Is...>{};

  }

  template<unsigned Start, unsigned End>
  using index_range = typename detail::range_builder<Start, End>::type;

}

#endif /* LAZY_DETAIL_INDEX_LIST_HPP_ */
