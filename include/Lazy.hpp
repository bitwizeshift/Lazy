/*
 * Lazy.hpp
 *
 *  Created on: Jun 9, 2016
 *      Author: matthewrodusek
 */

#ifndef LAZY_LAZY_HPP_
#define LAZY_LAZY_HPP_

#if __cplusplus < 201102L
# error This file requires compiler and library support for the ISO C++11 standard.
#endif

#include "detail/lazy_traits.hpp"
#include "detail/index_list.hpp"
#include "lazy_exceptions.hpp"

#include <type_traits>
#include <functional>
#include <stdexcept>
#include <utility>

namespace lazy{

  /// \brief Constructor tag for tag-dispatching VA Arguments
  struct ctor_va_args_tag{};

  template<typename...Args>
  struct get_signature{ typedef void type; };


	/// \brief Lazy class used for lazy-loading any type
	///
	/// The stored lazy-loaded class, \c T, will always be instantiated
	/// before being accessed, and destructed when put out of scope.
	///
	/// \tparam T the type contained within this \c Lazy
	template<
	  typename T,
	  typename CtorSignature = T(void)
	>
	class Lazy
	{
		//---------------------------------------------------------------------
		// Public Member Types
		//---------------------------------------------------------------------
	public:

	  typedef Lazy<T,CtorSignature> this_type;

		typedef T        value_type;
		typedef T*       pointer;
		typedef const T* const_pointer;
		typedef T&       reference;
		typedef const T& const_reference;

		typedef std::function<CtorSignature> ctor_function_type;
		typedef std::function<void(T&)>      dtor_function_type;

		//---------------------------------------------------------------------
		// Construction / Destruction / Assignment
		//---------------------------------------------------------------------
	public:

		/// \brief Default constructor; no initialization takes place
		Lazy( ) noexcept;

		/// \brief Constructs a \c Lazy given the \p constructor and \p destructor
		///        functions
		///
		/// \param constructor function to use for construction
		/// \param destructor  function to use prior to destruction
		Lazy( ctor_function_type constructor,
		      dtor_function_type destructor = default_destructor ) noexcept;

		/// \brief Constructs a \c Lazy by copying another \c Lazy
		///
		/// \param rhs the \c Lazy to copy
		Lazy( const this_type& rhs ) noexcept( std::is_nothrow_copy_constructible<T>::value );

		/// \brief Constructs a \c Lazy by calling \c T's copy constructor
		///
		/// \param rhs the \c T to copy
		Lazy( const value_type& rhs ) noexcept( std::is_nothrow_copy_constructible<T>::value );

		/// \brief Constructs a \c Lazy from a given rvalue \c T
		///
		/// \param rhs the \c T to move
		Lazy( value_type&& rhs ) noexcept( std::is_nothrow_move_constructible<T>::value );

		/// \brief Constructs a \c Lazy by constructing it's \c T with its constructor
		///
		/// \details This only enables if the first argument is not a callable function,
		///          to allow for the overload resolution to find \c Lazy(constructor,destructor)
		///
		/// \param args arguments to \c T's constructor
    template<
      typename...Args,
      typename = typename std::enable_if<!detail::is_ctor_or_tuple_args<Args...>::value>::type
    >
    Lazy( Args&&...args ) noexcept( std::is_nothrow_constructible<T,Args...>::value );

    /// \brief Constructs a \c Lazy by constructing it's \c T with its constructor
    ///
    /// \details This VA constructor never disables, to allow calling \c T's constructor with
    ///          a function, if it is so desired
    ///
    /// \param tag  unused tag for dispatching to VA constructor
    /// \param args arguments to \c T's constructor
    template<typename...Args>
    Lazy( ctor_va_args_tag tag, Args&&...args ) noexcept( std::is_nothrow_constructible<T,Args...>::value );

    template<typename...Args>
    Lazy( const std::tuple<Args...>& x )
      : m_is_initialized(false),
        m_destructor(default_destructor)
    {
      construct(x);
    }

    /// \brief Destructs this \c Lazy and it's \c T
    ~Lazy() noexcept( std::is_nothrow_destructible<T>::value );

		/// \brief Assigns a \c Lazy to this \c Lazy
		///
		/// \param rhs the \c Lazy on the right-side of the assignment
		/// \return reference to (*this)
    this_type& operator=( const this_type& rhs ) noexcept( std::is_nothrow_copy_assignable<T>::value );

		/// \brief Assigns a \c T to this \c Lazy
		///
		/// \param rhs the \c T on the right-side of the assignment
		/// \return reference to (\c ptr())
		value_type& operator=( const value_type& rhs ) noexcept( std::is_nothrow_copy_assignable<T>::value );

    /// \brief Assigns an rvalue \c T to this \c Lazy
    ///
    /// \param rhs the \c T on the right-side of the assignment
    /// \return reference to (\c ptr())
    value_type& operator=( value_type&& rhs ) noexcept( std::is_nothrow_copy_assignable<T>::value );

		//---------------------------------------------------------------------
		// Casting
		//---------------------------------------------------------------------
	public:

    /// \brief Converts this \c Lazy into a reference
    ///
    /// \return the reference to the lazy-loaded object
		operator reference() const;

		//---------------------------------------------------------------------
		// Operators
		//---------------------------------------------------------------------
	public:

    /// \brief Dereferences this \c Lazy object into the lazy-loaded object
    ///
    /// \return a reference to the lazy-loaded object
    reference operator*();

    /// \brief Dereferences this \c Lazy object into the lazy-loaded object
    ///
    /// \return a constant reference to the lazy-loaded object
    const_reference operator*() const;

    /// \brief Dereferences this \c Lazy object into the lazy-loaded object
    ///
    /// \return a pointer to the lazy-loaded object
    pointer operator->();

    /// \brief Dereferences this \c Lazy object into the lazy-loaded object
    ///
    /// \return a constant pointer to the lazy-loaded object
    const_pointer operator->() const;

		//---------------------------------------------------------------------
		// Private Member Types
		//---------------------------------------------------------------------
	private:

		typedef typename std::aligned_storage<sizeof(T),alignof(T)>::type storage_type;

		//---------------------------------------------------------------------
		// Private Members
		//---------------------------------------------------------------------
	private:

		mutable storage_type m_storage;
		mutable bool         m_is_initialized;
		ctor_function_type   m_constructor;
		dtor_function_type   m_destructor;

    //---------------------------------------------------------------------
    // Private Static Member Functions
    //---------------------------------------------------------------------
  private:

		/// \brief A default destructor function for this Lazy object
		///
		/// \param x the \c T type to be destructed
		static void default_destructor(value_type& x);

		//---------------------------------------------------------------------
		// Private Member Functions
		//---------------------------------------------------------------------
	private:

		/// \brief Gets a pointer to the data stored in this \c Laz
    ///
    /// \return the constant pointer to the object
    pointer ptr( ) const noexcept;

		/// \brief Forcibly initializes the \c Lazy
		///
		/// \throws lazy::no_lazy_initializer_error if no initializer has been
		///         provided
		void lazy_construct( ) const;

		/// \brief Constructs a \c Lazy object using \c T's copy constructor
		///
		/// \param x Instance of \c T to copy.
		void construct( const T& x ) const noexcept(std::is_nothrow_copy_constructible<T>::value);

		/// \brief Constructs a \c Lazy object using \c T's move constructor
		///
		/// \param x Instance of rvalue \c T to copy
		void construct( const value_type&& x ) const noexcept(std::is_nothrow_move_constructible<T>::value);

    /// \brief Constructs a \c Lazy object using the arguments for \c T's constructor
		///
		/// \param tag  tag to dispatch to this VA args constructor
		/// \param args the arguments to forward to the constructor
		template<
		  typename...Args
    >
		void construct( ctor_va_args_tag tag, Args&&...args ) const noexcept(std::is_nothrow_constructible<T,Args...>::value);

		/// \brief Constructs a \c Lazy object using the arguments provided in a
		///        \c std::tuple for \c T's constructor
		///
		/// \param tag  the tag for tag-dispatching
		/// \param args the arguments to forward to the constructor
		template<typename...Args>
		void construct( const std::tuple<Args...>& args ) noexcept(std::is_nothrow_constructible<T,Args...>::value);

		/// \brief Constructs a \c lazy object by passing all values stored in a
		///        \c std::tuple to \c T's constructor
		///
		/// \param unused unused parameter for getting index list
		template<typename...Args, size_t...Is>
		void tuple_construct(const std::tuple<Args...>& args,
		                     const index_list<Is...>& unused ) noexcept(std::is_nothrow_constructible<T,Args...>::value);

		/// \brief Destructs the \c Lazy object
		void destruct( ) const noexcept(std::is_nothrow_destructible<T>::value);
	};

  //--------------------------------------------------------------------------
  // Utilities
  //--------------------------------------------------------------------------

  /// \brief Makes a \c Lazy object given the constructor arguments for a type \c T
  ///
  /// \param tag  constructor tag to dispatch to VA args
  /// \return the \c Lazy object
  template<
    typename T,
    typename...Args,
    typename = typename std::enable_if<!detail::is_ctor_dtor_args<Args...>::value>::type
  >
  Lazy<T, std::tuple<Args...>(void)> make_lazy( Args&&... args );

  /// \brief Makes a \c Lazy object given the constructor arguments for a type \c T
  ///
  /// \param tag  constructor tag to dispatch to VA args
  /// \param args arguments to forward to the constructor
  /// \return the \c Lazy object
  template<typename T,typename...Args>
  Lazy<T, std::tuple<Args...>(void)> make_lazy( ctor_va_args_tag tag, Args&&... args );

/*
  /// \brief Makes a \c Lazy object given the constructor and destructor functions
  ///
  /// \param ctor the construction function
  /// \param ctor the destruction function
  /// \return the \c Lazy object
  template<
    typename T,
    typename Ctor,
    typename Dtor,
    typename = typename std::enable_if<detail::is_callable<Ctor>::value>::type,
    typename = typename std::enable_if<detail::is_callable<Dtor>::value>::type
  >
  Lazy<T, typename std::result_of<Ctor>::type> make_lazy( Ctor& ctor, Dtor& dtor );
*/
} // namespace lazy

#include "detail/Lazy.inl"

#endif /* LAZYLAZY_HPP_ */
