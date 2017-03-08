/**
 * \file lazy_traits.hpp
 *
 * \brief This file contains a collection of helpful type traits to build the
 *        \c Lazy class.
 *
 * \author Matthew Rodusek (matthew.rodusek@gmail.com)
 * \copyright Matthew Rodusek
 */

/*
 * The MIT License (MIT)
 *
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * Copyright (c) 2016 Matthew Rodusek <http://rodusek.me>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef LAZY_DETAIL_LAZY_TRAITS_HPP_
#define LAZY_DETAIL_LAZY_TRAITS_HPP_

#include <type_traits>

namespace lazy{

  template<typename> class Lazy;

  namespace detail {

    template<typename...>
    struct voidify{ using type = void; };

    template<typename...Ts>
    using void_t = typename voidify<Ts...>::type;

    template<typename Fn, typename R = void, typename = void>
    struct is_callable : std::false_type{};

    template<typename Fn>
    struct is_callable<Fn,void,void_t<std::result_of_t<Fn>>>
      : std::true_type{};

    template<typename Func, typename R>
    struct is_callable<Func,R, void_t<std::result_of_t<Func>>>
      : std::is_convertible<std::result_of_t<Func>,R>{};

    template<bool B>
    using bool_constant = std::integral_constant<bool,B>;

    template<typename...>
    struct conjunction : std::true_type { };

    template<typename B1>
    struct conjunction<B1>
      : B1 { };

    template<typename B1, typename... Bn>
    struct conjunction<B1, Bn...>
      : std::conditional_t<B1::value != false, conjunction<Bn...>, B1>  {};

    template<typename...>
    struct disjunction : std::false_type { };

    template<typename B1>
    struct disjunction<B1>
      : B1 { };

    template<typename B1, typename... Bn>
    struct disjunction<B1, Bn...>
      : std::conditional_t<B1::value != false, B1, conjunction<Bn...>>  {};

    template<typename B>
    struct negation : bool_constant<!B::value>{ };

    template<typename T, typename U>
    struct lazy_is_enabled_ctor_base : negation<
      disjunction<
        std::is_constructible<T, Lazy<U>&>,
        std::is_constructible<T, const Lazy<U>&>,
        std::is_constructible<T, Lazy<U>&&>,
        std::is_constructible<T, const Lazy<U>&&>,

        std::is_convertible<Lazy<U>&, T>,
        std::is_convertible<const Lazy<U>&, T>,
        std::is_convertible<Lazy<U>&&, T>,
        std::is_convertible<const Lazy<U>&&, T>
      >
    >{};

    template<typename T, typename U>
    struct lazy_is_enabled_copy_ctor : conjunction<
      lazy_is_enabled_ctor_base<T,U>,
      std::is_constructible<T,const U&>
    >{};

    template<typename T, typename U>
    struct lazy_is_enabled_move_ctor : conjunction<
    lazy_is_enabled_ctor_base<T,U>,
      std::is_constructible<T, U&&>
    >{};

    template<typename T, typename U>
    struct lazy_is_enabled_assignment_base : negation<
      disjunction<
        std::is_constructible<T, Lazy<U>&>,
        std::is_constructible<T, const Lazy<U>&>,
        std::is_constructible<T, Lazy<U>&&>,
        std::is_constructible<T, const Lazy<U>&&>,

        std::is_convertible<Lazy<U>&, T>,
        std::is_convertible<const Lazy<U>&, T>,
        std::is_convertible<Lazy<U>&&, T>,
        std::is_convertible<const Lazy<U>&&, T>,

        std::is_assignable<T&, Lazy<U>&>,
        std::is_assignable<T&, const Lazy<U>&>,
        std::is_assignable<T&, Lazy<U>&&>,
        std::is_assignable<T&, const Lazy<U>&&>
      >
    >{};

    template<typename T, typename U>
    struct lazy_is_enabled_copy_assignment : conjunction<
      lazy_is_enabled_assignment_base<T,U>,
      std::is_assignable<T&, const U&>,
      std::is_constructible<T, const U&>
    >{};

    template<typename T, typename U>
    struct lazy_is_enabled_move_assignment : conjunction<
      lazy_is_enabled_assignment_base<T,U>,
      std::is_assignable<T&, U&&>,
      std::is_constructible<T, U&&>
    >{};

    template<typename T, typename U>
    struct lazy_is_direct_initializable : conjunction<
      std::is_constructible<T, U&&>,
      negation<std::is_same<std::decay_t<U>,in_place_t>>,
      negation<std::is_same<std::decay_t<U>,Lazy<T>>>
    >{};

    template<typename T, typename U>
    struct lazy_is_direct_init_assignable : conjunction<
      negation<std::is_same<std::decay_t<U>,Lazy<T>>>,
      std::is_constructible<T,U>,
      std::is_assignable<T,U>,
      disjunction<
        negation<std::is_scalar<U>>,
        negation<std::is_same<std::decay_t<U>,T>>
      >
    >{};

  } // namespace detail
} // namespace lazy

#endif /* LAZY_DETAIL_LAZY_TRAITS_HPP_ */
