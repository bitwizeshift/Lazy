/*
 * lazy_exceptions.hpp
 *
 *  Created on: Jun 10, 2016
 *      Author: matthewrodusek
 */

#ifndef LAZY_LAZY_EXCEPTIONS_HPP_
#define LAZY_LAZY_EXCEPTIONS_HPP_

#include <stdexcept>

namespace lazy{

  /// \brief Exception for managing when no initializer is given
  ///        to a constructor
  ///
  class no_lazy_initializer_error : public std::runtime_error
  {
  public:
    no_lazy_initializer_error()
      : std::runtime_error("No lazy initializer provided for Lazy object")
    {

    }
  };
}

#endif /* LAZY_LAZY_EXCEPTIONS_HPP_ */
