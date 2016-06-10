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

    /// \brief Type-trait for identities (always defines \c T as \c type)
    template<typename T>
    struct identity{
      typedef T type;
    };

    /// \brief Implementation helper for the \c void_t idiom
    template<typename...Args>
    struct voidify : public identity<void>{};

    /// \brief Implementation of the \c void_t idiom
    template<typename...Args>
    using void_t = typename voidify<Args...>::type;

    /// \brief type-trait to check if \c T has a functor
    template<typename T, typename = void>
    struct has_functor : public std::false_type{};

    template<typename T>
    struct has_functor<T,void_t<decltype(&T::operator())>>: public std::true_type{};

    /// \brief type-trait to check if \c T is callable
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

    /// \brief type-trait to determine if a type provided is a \c std::tuple
    ///
    /// \note this also detectes \c std::pair
    template<typename T>
    struct is_tuple : public std::false_type{};

    template<typename...Args>
    struct is_tuple<std::tuple<Args...> > : public std::true_type{};

    template<typename...Args>
    struct is_tuple<std::pair<Args...> > : public std::true_type{};

    //
    // Arguments
    //

    /// \brief type-trait to retrieve the first argument of a varidic template
    template<typename...Args>
    struct first_arg_type{};

    template<typename Arg0, typename...Args>
    struct first_arg_type<Arg0, Args...> : public identity<Arg0>{};

    /// \brief helper alias for the first argument
    template<typename...Args>
    using first_arg_type_t = typename first_arg_type<Args...>::type;

    /// \brief type-trait to determine if variadic arguments are the first arg to
    ///        a lazy construction
    template<typename...Args>
    struct is_ctor_dtor_args : public std::integral_constant<bool,
      sizeof...(Args)<=2 &&
      sizeof...(Args)>=1 &&
      is_callable<first_arg_type_t<Args...>>::value
     >{};
    /// \brief type-trait to determine if variadic arguments are a tuple argument
    ///        for the Lazy class
    template<typename...Args>
    struct is_tuple_ctor_args : public std::integral_constant<bool,
      sizeof...(Args)==1 &&
      is_tuple<first_arg_type_t<Args...>>::value
    >{};

    template<typename...Args>
    struct is_ctor_or_tuple_args : public std::integral_constant<bool,
      is_tuple_ctor_args<Args...>::value || is_ctor_dtor_args<Args...>::value
    >{};
  }
}

#endif /* LAZY_DETAIL_LAZY_TRAITS_HPP_ */
