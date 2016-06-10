namespace lazy{

  //--------------------------------------------------------------------------
  // Constructors / Destructor / Assignment
  //--------------------------------------------------------------------------

  template<typename T, typename C>
  Lazy<T,C>::Lazy( ) noexcept
    : m_is_initialized(false),
      m_destructor(default_destructor)
  {

  }

  template<typename T, typename C>
  Lazy<T,C>::Lazy( ctor_function_type constructor,
                   dtor_function_type destructor ) noexcept
    : m_is_initialized(false),
      m_constructor(std::move(constructor)),
      m_destructor(std::move(destructor))
  {

  }

  template<typename T, typename C>
  Lazy<T,C>::Lazy( const this_type& rhs )
    noexcept( std::is_nothrow_copy_constructible<T>::value )
    : m_is_initialized(false),
      m_constructor(rhs.m_constructor),
      m_destructor(rhs.m_destructor)
  {
    construct( *rhs.ptr() );
  }

  template<typename T, typename C>
  Lazy<T,C>::Lazy( const T& rhs )
    noexcept( std::is_nothrow_copy_constructible<T>::value )
    : m_is_initialized(false),
      m_destructor(default_destructor)
  {
    construct( rhs );
  }

  template<typename T, typename C>
  Lazy<T,C>::Lazy( T&& rhs )
    noexcept( std::is_nothrow_move_constructible<T>::value )
    : m_is_initialized(false),
      m_destructor(default_destructor)
  {
     construct( rhs );
  }

  template<typename T, typename C>
  template<typename...Args, typename>
  Lazy<T,C>::Lazy( Args&&...args )
    noexcept( std::is_nothrow_constructible<T,Args...>::value )
    : m_is_initialized(false),
      m_destructor(default_destructor)
  {
    static_assert(std::is_constructible<T,Args...>::value, "Constructor for T with given arguments does not exist");
    construct( ctor_va_args_tag(), std::forward<Args>(args)... );
  }

  template<typename T, typename C>
  template<typename...Args>
  Lazy<T,C>::Lazy( ctor_va_args_tag tag, Args&&...args )
    noexcept( std::is_nothrow_constructible<T,Args...>::value )
    : m_is_initialized(false),
      m_destructor(default_destructor)
  {
    static_assert(std::is_constructible<T,Args...>::value, "Constructor for T with given arguments does not exist");
    construct( tag, std::forward(args)... );
  }

  //---------------------------------------------------------------------------

  template<typename T, typename C>
  Lazy<T,C>::~Lazy() noexcept( std::is_nothrow_destructible<T>::value )
  {
    destruct();
  }

  //---------------------------------------------------------------------------

  template<typename T, typename C>
  Lazy<T,C>& Lazy<T,C>::operator=( const this_type& rhs )
    noexcept( std::is_nothrow_copy_assignable<T>::value )
  {
    m_constructor = rhs.m_constructor;
    m_destructor  = rhs.m_destructor;
    construct(*rhs.ptr());
    return (*this);
  }

  template<typename T, typename C>
  typename Lazy<T,C>::value_type& Lazy<T,C>::operator=( const value_type& rhs )
    noexcept( std::is_nothrow_copy_assignable<T>::value )
  {
    construct(rhs);
    return *ptr();
  }

  template<typename T, typename C>
  typename Lazy<T,C>::value_type& Lazy<T,C>::operator=( value_type&& rhs )
    noexcept( std::is_nothrow_copy_assignable<T>::value )
  {
    construct(rhs);
    return *ptr();
  }

  //--------------------------------------------------------------------------
  // Casting
  //--------------------------------------------------------------------------

  template<typename T, typename C>
  Lazy<T,C>::operator Lazy<T,C>::reference() const
  {
    lazy_construct();
    return *ptr();
  }

  //--------------------------------------------------------------------------
  // Operators
  //--------------------------------------------------------------------------

  template<typename T, typename C>
  typename Lazy<T,C>::reference Lazy<T,C>::operator*()
  {
    lazy_construct();
    return *ptr();
  }

  template<typename T, typename C>
  typename Lazy<T,C>::const_reference Lazy<T,C>::operator*() const
  {
    lazy_construct();
    return *ptr();
  }

  template<typename T, typename C>
  typename Lazy<T,C>::pointer Lazy<T,C>::operator->()
  {
    lazy_construct();
    return ptr();
  }

  template<typename T, typename C>
  typename Lazy<T,C>::const_pointer Lazy<T,C>::operator->() const
  {
    lazy_construct();
    return ptr();
  }

  //---------------------------------------------------------------------
  // Private Static Member Functions
  //---------------------------------------------------------------------

  template<typename T, typename C>
  void Lazy<T,C>::default_destructor(value_type& x){}

  //---------------------------------------------------------------------
  // Private Member Functions
  //---------------------------------------------------------------------

  template<typename T, typename C>
  typename Lazy<T,C>::pointer Lazy<T,C>::ptr() const noexcept
  {
    // address-of idiom (https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Address_Of)
    return reinterpret_cast<T*>(& const_cast<char&>(reinterpret_cast<const volatile char &>(m_storage)));
  }

  template<typename T, typename C>
  void Lazy<T,C>::lazy_construct( ) const
  {
    if( !m_is_initialized )
    {
      if( !static_cast<bool>(m_constructor) ){
        throw no_lazy_initializer_error();
      }
      construct( m_constructor() );
    }
  }

  template<typename T, typename C>
  void Lazy<T,C>::construct( const T& x ) const
    noexcept(std::is_nothrow_copy_constructible<T>::value)
  {
    destruct();
    new (ptr()) T(x);
    m_is_initialized = true;
  }

  template<typename T, typename C>
  void Lazy<T,C>::construct( const value_type&& x ) const
    noexcept(std::is_nothrow_move_constructible<T>::value)
  {
    destruct();
    new (ptr()) T( std::move(x) );
    m_is_initialized = true;
  }

  template<typename T, typename C>
  template<typename...Args>
  void Lazy<T,C>::construct( ctor_va_args_tag tag, Args&&...args ) const
    noexcept(std::is_nothrow_constructible<T,Args...>::value)
  {
    destruct();
    // I'm not sure why this requires the explicit type. Possible clang bug?
    new (ptr()) T( std::forward<Args>(args)... );
    m_is_initialized = true;
  }

  template<typename T, typename C>
  template<typename...Args>
  void Lazy<T,C>::construct( const std::tuple<Args...>& args )
    noexcept(std::is_nothrow_constructible<T,Args...>::value)
  {
    destruct();
    tuple_construct(args,index_range<0,sizeof...(Args)>());
    m_is_initialized = true;
  }

  template<typename T, typename C>
  template<typename...Args, size_t...Is>
  void Lazy<T,C>::tuple_construct(const std::tuple<Args...>& args,
                                  const index_list<Is...>& unused )
    noexcept(std::is_nothrow_constructible<T,Args...>::value)
  {
    new (ptr()) T( std::get<Is>(args)... );
  }

  template<typename T, typename C>
  void Lazy<T,C>::destruct( ) const
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
  inline Lazy<T, std::tuple<Args...>(void)> make_lazy( Args&&... args )
  {
    static_assert(std::is_constructible<T,Args...>::value, "Constructor for T with given arguments does not exist");
    auto ctor = [&]()->std::tuple<Args...>{
      return std::make_tuple(args...);
    };
    return Lazy<T, std::tuple<Args...>(void)>(ctor);
  }

  template<typename T,typename...Args>
  inline Lazy<T, std::tuple<Args...>(void)> make_lazy( ctor_va_args_tag tag, Args&&... args )
  {
    static_assert(std::is_constructible<T,Args...>::value, "Constructor for T with given arguments does not exist");
    auto ctor = [&]()->std::tuple<Args...>{
      return std::make_tuple(args...);
    };
    return Lazy<T, std::tuple<Args...>(void)>(ctor);
  }
}
