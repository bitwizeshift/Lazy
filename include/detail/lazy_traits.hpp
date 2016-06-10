/*
 * lazy_traits.hpp
 *
 *  Created on: Jun 10, 2016
 *      Author: matthewrodusek
 */

#ifndef LAZY_DETAIL_LAZY_TRAITS_HPP_
#define LAZY_DETAIL_LAZY_TRAITS_HPP_

#include <type_traits>
#include <utility>

namespace lazy{
  namespace detail{

    template<typename T>
    struct identity{
      typedef T type;
    };

    template<typename...Args>
    using void_t = void;

    template<typename T, typename = void>
    struct has_functor : public std::false_type{};

    template<typename T>
    struct has_functor<T,void_t<decltype(&T::operator())>>: public std::true_type{};

    template<typename T>
    struct is_callable : public
      std::conditional<
        std::is_class<T>::value,
        has_functor<T>,
        typename std::conditional<
          std::is_function<T>::value,
          std::is_function<T>,
          std::is_member_function_pointer<T>
        >::type
      >::type{};

    template<typename...Args>
    struct first_arg_type{};

    template<typename Arg0, typename...Args>
    struct first_arg_type<Arg0, Args...> : public identity<Arg0>{};

    template<typename...Args>
    using first_arg_type_t = typename first_arg_type<Args...>::type;

    template<typename...Args>
    struct is_ctor_dtor_args : public std::integral_constant<bool,
      sizeof...(Args)<=2 &&
      sizeof...(Args)>=1 &&
      is_callable<first_arg_type_t<Args...>>::value
     >{};

    template<typename...Args>
    struct is_tuple : public std::false_type{};

    template<typename...Args>
    struct is_tuple<std::tuple<Args...> > : public std::true_type{};

    template<typename...Args>
    struct is_tuple<std::pair<Args...> > : public std::true_type{};

    template<typename...Args>
    struct is_tuple_ctor_args : public std::integral_constant<bool,
      sizeof...(Args)==1 &&
      is_tuple<first_arg_type_t<Args...>>::value
    >{};

    template<typename...Args>
    struct is_ctor_or_tuple_args : public std::integral_constant<bool,
      is_tuple_ctor_args<Args...>::value || is_ctor_dtor_args<Args...>::value
    >{};

    template<typename T>
    struct get_functor_signature : public identity<decltype(&T::operator())>{};

    template<typename T>
    struct get_function_signature : public identity<decltype(T())>{};
  }
}

#endif /* LAZY_DETAIL_LAZY_TRAITS_HPP_ */
