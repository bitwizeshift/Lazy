namespace lazy{

  //--------------------------------------------------------------------------
  // Constructors / Destructor / Assignment
  //--------------------------------------------------------------------------

  template<typename T>
  inline Lazy<T>::Lazy()
    : m_is_initialized(false),
      m_constructor([this](){this->construct(ctor_va_args_tag());}),
      m_destructor(default_destructor)
  {
    static_assert(std::is_default_constructible<T>::value,"No matching default constructor for type T");
  }

  template<typename T>
  template<typename CtorFunc,typename DtorFunc,typename,typename>
  inline Lazy<T>::Lazy( const CtorFunc& constructor,
                        const DtorFunc& destructor )
    : m_is_initialized(false),
      m_constructor([this,constructor](){this->construct(constructor());}),
      m_destructor(destructor)
  {
    using return_type = typename detail::function_traits<CtorFunc>::result_type;

    static_assert(detail::is_tuple<return_type>::value,"Lazy-construction functions must return tuples containing constructor arguments");
    static_assert(detail::is_tuple_constructible<T,return_type>::value, "No matching constructor for type T with given arguments");
  }

  template<typename T>
  inline Lazy<T>::Lazy( const this_type& rhs )
    : m_is_initialized(false),
      m_constructor(rhs.m_constructor),
      m_destructor(rhs.m_destructor)
  {
    static_assert(std::is_copy_constructible<T>::value,"No matching copy constructor for type T");

    if(rhs.m_is_initialized)
    {
      construct(*rhs);
    }
  }

  template<typename T>
  inline Lazy<T>::Lazy( this_type&& rhs )
    : m_is_initialized(false),
      m_constructor(std::move(rhs.m_constructor)),
      m_destructor(std::move(rhs.m_destructor))
  {
    static_assert(std::is_move_constructible<T>::value,"No matching move constructor for type T");

    if(rhs.m_is_initialized)
    {
      construct(std::move(*rhs));
    }
  }

  template<typename T>
  inline Lazy<T>::Lazy( const value_type& rhs )
    : m_is_initialized(false),
      m_constructor([this,rhs](){this->construct(std::move(rhs));}),
      m_destructor(default_destructor)
  {
    static_assert(std::is_copy_constructible<T>::value,"No matching copy constructor for type T");
  }

  template<typename T>
  inline Lazy<T>::Lazy( value_type&& rhs )
    : m_is_initialized(false),
      m_constructor([this,rhs](){this->construct(std::move(rhs));}),
      m_destructor(default_destructor)
  {
    static_assert(std::is_move_constructible<T>::value,"No matching move constructor for type T");
  }

  //--------------------------------------------------------------------------

  template<typename T>
  inline Lazy<T>::~Lazy()
  {
    destruct();
  }

  //--------------------------------------------------------------------------

  template<typename T>
  inline Lazy<T>& Lazy<T>::operator=( const this_type& rhs )
  {
    static_assert(std::is_copy_assignable<T>::value,"No matching copy assignment operator for type T");
    static_assert(std::is_copy_constructible<T>::value,"No matching copy constructor for type T");

    if(rhs.m_is_initialized) {
      lazy_construct();
      assign(*rhs);
    } else {
      m_constructor = rhs.m_constructor;
    }
    m_destructor  = rhs.m_destructor;

    return (*this);
  }

  template<typename T>
  inline typename Lazy<T>::this_type& Lazy<T>::operator=( this_type&& rhs )
  {
    static_assert(std::is_move_assignable<T>::value,"No matching move assignment operator for type T");
    static_assert(std::is_move_constructible<T>::value,"No matching move constructor for type T");

    if(rhs.m_is_initialized) {
      lazy_construct();
      assign(std::move(*rhs));
    } else {
      m_constructor = std::move(rhs.m_constructor);
      rhs.m_constructor = nullptr;
    }
    m_destructor = std::move(rhs.m_destructor);
    rhs.m_destructor = nullptr;

    return (*this);
  }

  template<typename T>
  inline typename Lazy<T>::value_type& Lazy<T>::operator=( const value_type& rhs )
  {
    static_assert(std::is_copy_assignable<T>::value,"No matching copy assignment operator for type T");

    lazy_construct();
    assign(rhs);

    return *ptr();
  }

  template<typename T>
  inline typename Lazy<T>::value_type& Lazy<T>::operator=( value_type&& rhs )
 {
    static_assert(std::is_move_assignable<T>::value,"No matching move assignment operator for type T");

    lazy_construct();
    assign(std::forward<value_type>(rhs));

    return *ptr();
  }

  //--------------------------------------------------------------------------
  // Casting
  //--------------------------------------------------------------------------

  template<typename T>
  inline Lazy<T>::operator Lazy<T>::reference()
    const
  {
    lazy_construct();
    return *ptr();
  }

  template<typename T>
  inline Lazy<T>::operator bool()
    const noexcept
  {
    return m_is_initialized;
  }

  //--------------------------------------------------------------------------
  // Operators
  //--------------------------------------------------------------------------

  template<typename T>
  inline void Lazy<T>::swap(Lazy<T>& rhs)
    noexcept
  {
    using std::swap; // for ADL

    swap(m_constructor,rhs.m_constructor);
    swap(m_destructor,rhs.m_destructor);
    swap(m_is_initialized,rhs.m_is_initialized);
    swap((*ptr()),(*rhs.ptr())); // Swap the values of the T types
  }

  template<typename T>
  inline bool Lazy<T>::is_initialized()
    const noexcept
  {
    return m_is_initialized;
  }

  template<typename T>
  inline typename Lazy<T>::pointer Lazy<T>::get()
    const
  {
    lazy_construct();
    return ptr();
  }

  template<typename T>
  inline typename Lazy<T>::reference Lazy<T>::operator*()
    const
  {
    lazy_construct();
    return *ptr();
  }

  template<typename T>
  inline typename Lazy<T>::pointer Lazy<T>::operator->()
    const
  {
    lazy_construct();
    return ptr();
  }

  //--------------------------------------------------------------------------
  // Private Static Member Functions
  //--------------------------------------------------------------------------

  template<typename T>
  inline void Lazy<T>::default_destructor(value_type& x) noexcept{}

  //--------------------------------------------------------------------------
  // Private Constructors
  //--------------------------------------------------------------------------

  template<typename T>
  template<typename...Args>
  inline Lazy<T>::Lazy( ctor_va_args_tag tag, Args&&...args )
    noexcept( detail::are_nothrow_copy_constructible<Args...>::value )
    : m_is_initialized(false),
      m_constructor([this,args...](){this->construct(ctor_va_args_tag(), std::move(args)...);}),
      m_destructor(default_destructor)
  {
    static_assert(std::is_constructible<T,Args...>::value, "No matching constructor for type T with given arguments");
  }

  //--------------------------------------------------------------------------
  // Private Member Functions
  //--------------------------------------------------------------------------

  template<typename T>
  inline typename Lazy<T>::unqualified_pointer Lazy<T>::ptr()
    const noexcept
  {
    // address-of idiom (https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Address_Of)
    return reinterpret_cast<unqualified_pointer>(& const_cast<char&>(reinterpret_cast<const volatile char &>(m_storage)));
  }

  template<typename T>
  inline void Lazy<T>::lazy_construct( )
    const
  {
    if( !m_is_initialized )
    {
      m_constructor();
      m_is_initialized = true;
    }
  }

  template<typename T>
  inline void Lazy<T>::construct( const value_type& x )
    const
  {
    destruct();
    new (ptr()) value_type( x );
    m_is_initialized = true;
  }

  template<typename T>
  inline void Lazy<T>::construct( value_type&& x )
    const
  {
    destruct();
    new (ptr()) value_type( std::forward<value_type>(x) );
    m_is_initialized = true;
  }

  template<typename T>
  template<typename...Args>
  inline void Lazy<T>::construct( ctor_va_args_tag tag, Args&&...args )
    const
  {
    destruct();
    new (ptr()) value_type( std::forward<Args>(args)... );
    m_is_initialized = true;
  }

  template<typename T>
  template<typename...Args>
  inline void Lazy<T>::construct( const std::tuple<Args...>& args )
    const
  {
    destruct();
    tuple_construct(args,detail::index_sequence_for<Args...>());
    m_is_initialized = true;
  }

  template<typename T>
  template<typename...Args, std::size_t...Ints>
  inline void Lazy<T>::tuple_construct(const std::tuple<Args...>& args,
                                       const detail::index_sequence<Ints...>& )
    const noexcept( std::is_nothrow_constructible<T,Args...>::value )
  {
    static_assert(std::is_constructible<T,Args...>::value,"No matching constructor for type T with given arguments");

    new (ptr()) T( std::get<Ints>(args)... );
  }

  template<typename T>
  inline void Lazy<T>::destruct( ) const
  {
    if( m_is_initialized )
    {
      if( m_destructor )
      {
        m_destructor(*ptr());
      }
      ptr()->~T();
      m_is_initialized = false;
    }
  }

  template<typename T>
  inline void Lazy<T>::assign( const value_type& rhs )
    const noexcept( std::is_nothrow_copy_assignable<T>::value )
  {
    (*ptr()) = rhs;
  }

  template<typename T>
  inline void Lazy<T>::assign( value_type&& rhs )
    const noexcept( std::is_nothrow_move_assignable<T>::value )
  {
    (*ptr()) = std::forward<value_type>(rhs);
  }

  //--------------------------------------------------------------------------
  // Utilities
  //--------------------------------------------------------------------------

  template<typename T, typename...Args>
  Lazy<T> make_lazy(Args&&...args)
  {
    return Lazy<T>(typename Lazy<T>::ctor_va_args_tag(), std::forward<Args>(args)...);
  }

  template<typename T>
  void swap(Lazy<T>& lhs, Lazy<T>& rhs) noexcept
  {
    lhs.swap(rhs);
  }

} // namespace lazy
