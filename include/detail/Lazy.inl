namespace lazy{

  //--------------------------------------------------------------------------
  // Constructors / Destructor / Assignment
  //--------------------------------------------------------------------------

  template<typename T>
  inline Lazy<T>::Lazy( ) noexcept
    : m_is_initialized(false),
      m_destructor(default_destructor)
  {

  }

  template<typename T>
  template<typename CtorFunc, typename DtorFunc>
  inline Lazy<T>::Lazy( CtorFunc& constructor,
                 DtorFunc& destructor ) noexcept
    : m_is_initialized(false),
      m_constructor([&](){this->construct(constructor());}),
      m_destructor(destructor)
  {

  }

  template<typename T>
  inline Lazy<T>::Lazy( const this_type& rhs )
    noexcept( std::is_nothrow_copy_constructible<T>::value )
    : m_is_initialized(false),
      m_constructor(rhs.m_constructor),
      m_destructor(rhs.m_destructor)
  {
    construct( *rhs.ptr() );
  }

  template<typename T>
  inline Lazy<T>::Lazy( this_type&& rhs )
    noexcept( std::is_nothrow_move_constructible<T>::value )
    : m_is_initialized(std::move(rhs.m_is_initialized)),
      m_constructor(std::move(rhs.m_constructor)),
      m_destructor(std::move(rhs.m_destructor))
  {
    rhs.m_is_initialized=false; // Disallow rhs from destructing if initialized
  }

  template<typename T>
  inline Lazy<T>::Lazy( const T& rhs )
    noexcept( std::is_nothrow_copy_constructible<T>::value )
    : m_is_initialized(false),
      m_destructor(default_destructor)
  {
    construct( rhs );
  }

  template<typename T>
  inline Lazy<T>::Lazy( T&& rhs )
    noexcept( std::is_nothrow_move_constructible<T>::value )
    : m_is_initialized(false),
      m_destructor(default_destructor)
  {
     construct( rhs );
  }

  template<typename T>
  template<typename...Args, typename>
  inline Lazy<T>::Lazy( Args&&...args )
    noexcept( std::is_nothrow_constructible<T,Args...>::value )
    : m_is_initialized(false),
      m_destructor(default_destructor)
  {
    static_assert(std::is_constructible<T,Args...>::value, "Constructor for T with given arguments does not exist");
    construct( ctor_va_args_tag(), std::forward<Args>(args)... );
  }

  template<typename T>
  template<typename...Args>
  inline Lazy<T>::Lazy( ctor_va_args_tag tag, Args&&...args )
    noexcept( std::is_nothrow_constructible<T,Args...>::value )
    : m_is_initialized(false),
      m_destructor(default_destructor)
  {
    static_assert(std::is_constructible<T,Args...>::value, "Constructor for T with given arguments does not exist");
    construct( tag, std::forward(args)... );
  }

  //---------------------------------------------------------------------------

  template<typename T>
  inline Lazy<T>::~Lazy() noexcept( std::is_nothrow_destructible<T>::value )
  {
    destruct();
  }

  //---------------------------------------------------------------------------

  template<typename T>
  inline Lazy<T>& Lazy<T>::operator=( const this_type& rhs )
    noexcept( std::is_nothrow_copy_assignable<T>::value )
  {
    m_constructor = rhs.m_constructor;
    m_destructor  = rhs.m_destructor;
    construct(*rhs.ptr());
    return (*this);
  }

  template<typename T>
  inline typename Lazy<T>::this_type& Lazy<T>::operator=( this_type&& rhs )
    noexcept( std::is_nothrow_move_assignable<T>::value )
  {
    m_is_initialized = std::move(rhs.m_is_initialized);
    m_constructor = std::move(rhs.m_constructor);
    m_destructor = std::move(rhs.m_destructor);

    rhs.m_is_initalized = false; // disallow rhs from destructing the content
    return (*this);
  }

  template<typename T>
  inline typename Lazy<T>::value_type& Lazy<T>::operator=( const value_type& rhs )
    noexcept( std::is_nothrow_copy_assignable<T>::value )
  {
    construct(rhs);
    return *ptr();
  }

  template<typename T>
  inline typename Lazy<T>::value_type& Lazy<T>::operator=( value_type&& rhs )
    noexcept( std::is_nothrow_copy_assignable<T>::value )
  {
    construct(rhs);
    return *ptr();
  }

  //--------------------------------------------------------------------------
  // Casting
  //--------------------------------------------------------------------------

  template<typename T>
  inline Lazy<T>::operator Lazy<T>::reference() const
  {
    lazy_construct();
    return *ptr();
  }

  //--------------------------------------------------------------------------
  // Operators
  //--------------------------------------------------------------------------

  template<typename T>
  inline typename Lazy<T>::reference Lazy<T>::operator*()
  {
    lazy_construct();
    return *ptr();
  }

  template<typename T>
  inline typename Lazy<T>::const_reference Lazy<T>::operator*() const
  {
    lazy_construct();
    return *ptr();
  }

  template<typename T>
  inline typename Lazy<T>::pointer Lazy<T>::operator->()
  {
    lazy_construct();
    return ptr();
  }

  template<typename T>
  inline typename Lazy<T>::const_pointer Lazy<T>::operator->() const
  {
    lazy_construct();
    return ptr();
  }

  //---------------------------------------------------------------------
  // Private Static Member Functions
  //---------------------------------------------------------------------

  template<typename T>
  inline void Lazy<T>::default_destructor(value_type& x){}

  //---------------------------------------------------------------------
  // Private Member Functions
  //---------------------------------------------------------------------

  template<typename T>
  inline typename Lazy<T>::pointer Lazy<T>::ptr() const noexcept
  {
    // address-of idiom (https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Address_Of)
    return reinterpret_cast<T*>(& const_cast<char&>(reinterpret_cast<const volatile char &>(m_storage)));
  }

  template<typename T>
  inline void Lazy<T>::lazy_construct( ) const
  {
    if( !m_is_initialized )
    {
      if( !static_cast<bool>(m_constructor) ){
        throw no_lazy_initializer_error();
      }
      m_constructor();
    }
  }

  template<typename T>
  inline void Lazy<T>::construct( const T& x ) const
    noexcept(std::is_nothrow_copy_constructible<T>::value)
  {
    destruct();
    new (ptr()) T(x);
    m_is_initialized = true;
  }

  template<typename T>
  inline void Lazy<T>::construct( const value_type&& x ) const
    noexcept(std::is_nothrow_move_constructible<T>::value)
  {
    destruct();
    new (ptr()) T( std::move(x) );
    m_is_initialized = true;
  }

  template<typename T>
  template<typename...Args>
  inline void Lazy<T>::construct( ctor_va_args_tag tag, Args&&...args ) const
    noexcept(std::is_nothrow_constructible<T,Args...>::value)
  {
    destruct();
    // I'm not sure why this requires the explicit type. Possible clang bug?
    new (ptr()) T( std::forward<Args>(args)... );
    m_is_initialized = true;
  }

  template<typename T>
  template<typename...Args>
  inline void Lazy<T>::construct( const std::tuple<Args...>& args )
    noexcept(std::is_nothrow_constructible<T,Args...>::value)
  {
    destruct();
    tuple_construct(args,index_range<0,sizeof...(Args)>());
    m_is_initialized = true;
  }

  template<typename T>
  template<typename...Args, size_t...Is>
  inline void Lazy<T>::tuple_construct(const std::tuple<Args...>& args,
                                  const index_list<Is...>& unused )
    noexcept(std::is_nothrow_constructible<T,Args...>::value)
  {
    new (ptr()) T( std::get<Is>(args)... );
  }

  template<typename T>
  inline void Lazy<T>::destruct( ) const
    noexcept(std::is_nothrow_destructible<T>::value)
  {
    if( m_is_initialized )
    {
      m_destructor(*ptr());
      ptr()->~T();
      m_is_initialized = false;
    }
  }

  //--------------------------------------------------------------------------
  // Utility Definitions
  //--------------------------------------------------------------------------

  template<typename T,typename...Args,typename>
  inline Lazy<T> make_lazy( Args&&... args )
  {
    static_assert(std::is_constructible<T,Args...>::value, "Constructor for T with given arguments does not exist");
    auto ctor = [&]()->std::tuple<Args...>{
      return std::make_tuple(args...);
    };
    return Lazy<T>(ctor);
  }

  template<typename T,typename...Args>
  inline Lazy<T> make_lazy( ctor_va_args_tag tag, Args&&... args )
  {
    static_assert(std::is_constructible<T,Args...>::value, "Constructor for T with given arguments does not exist");
    auto ctor = [&]()->std::tuple<Args...>{
      return std::make_tuple(args...);
    };
    return Lazy<T>(ctor);
  }

  template<typename T,typename Ctor,typename Dtor,typename,typename>
  inline Lazy<T> make_lazy(Ctor& ctor, Dtor& dtor)
  {
    return Lazy<T>(ctor,dtor);
  }
}
