namespace lazy{

  //--------------------------------------------------------------------------
  // Constructors / Destructor / Assignment
  //--------------------------------------------------------------------------

  template<typename T>
  inline Lazy<T>::Lazy( ) noexcept
    : m_is_initialized(false),
      m_constructor([this](){this->construct(ctor_va_args_tag());}),
      m_destructor(default_destructor)
  {
    static_assert(std::is_default_constructible<T>::value,"No matching default constructor for type T");
  }

  template<typename T>
  template<typename CtorFunc, typename DtorFunc>
  inline Lazy<T>::Lazy( CtorFunc& constructor,
                        DtorFunc& destructor ) noexcept
    : m_is_initialized(false),
      m_constructor([this,constructor](){this->construct(constructor());}),
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
    static_assert(std::is_copy_constructible<T>::value,"No matching copy constructor for type T");

    if(rhs.m_is_initialized)
    {
      construct(*rhs);
    }
  }

  template<typename T>
  inline Lazy<T>::Lazy( this_type&& rhs )
    noexcept( std::is_nothrow_move_constructible<T>::value )
    : m_is_initialized(false),
      m_constructor(std::move(rhs.m_constructor)),
      m_destructor(std::move(rhs.m_destructor))
  {
    static_assert(std::is_move_constructible<T>::value,"No matching move constructor for type T");

    if(rhs.m_is_initialized)
    {
      construct(std::move(*rhs));
      rhs.m_is_initialized=false; // Disallow rhs from destructing if initialized
    }
  }

  template<typename T>
  inline Lazy<T>::Lazy( const T& rhs )
    noexcept( std::is_nothrow_copy_constructible<T>::value )
    : m_is_initialized(false),
      m_constructor([this,rhs](){this->construct(rhs);}),
      m_destructor(default_destructor)
  {
    static_assert(std::is_copy_constructible<T>::value,"No matching copy constructor for type T");
  }

  template<typename T>
  inline Lazy<T>::Lazy( T&& rhs )
    noexcept( std::is_nothrow_move_constructible<T>::value )
    : m_is_initialized(false),
      m_constructor([this,rhs](){this->construct(rhs);}),
      m_destructor(default_destructor)
  {
    static_assert(std::is_move_constructible<T>::value,"No matching move constructor for type T");
  }

  template<typename T>
  template<typename...Args, typename>
  inline Lazy<T>::Lazy( Args&&...args )
    noexcept( std::is_nothrow_constructible<T,Args...>::value )
    : m_is_initialized(false),
      m_constructor([this,&args...](){this->construct(ctor_va_args_tag(), std::forward<Args>(args)...);}),
      m_destructor(default_destructor)
  {
    static_assert(std::is_constructible<T,Args...>::value, "No matching constructor for type T with given arguments");
  }

  template<typename T>
  template<typename...Args>
  inline Lazy<T>::Lazy( ctor_va_args_tag tag, Args&&...args )
    noexcept( std::is_nothrow_constructible<T,Args...>::value )
    : m_is_initialized(false),
      m_constructor([this,&args...](){this->construct(ctor_va_args_tag(), std::forward<Args>(args)...);}),
      m_destructor(default_destructor)
  {
    static_assert(std::is_constructible<T,Args...>::value, "No matching constructor for type T with given arguments");
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
    noexcept( std::is_nothrow_copy_assignable<T>::value &&
              std::is_nothrow_copy_constructible<T>::value )
  {
    static_assert(std::is_copy_assignable<T>::value,"No matching copy assignment operator for type T");
    static_assert(std::is_copy_constructible<T>::value,"No matching copy constructor for type T");

    m_constructor = rhs.m_constructor;
    m_destructor  = rhs.m_destructor;

    if(m_is_initialized){
      assign(*rhs);
    }else{
      construct(*rhs);
    }
    return (*this);
  }

  template<typename T>
  inline typename Lazy<T>::this_type& Lazy<T>::operator=( this_type&& rhs )
    noexcept( std::is_nothrow_move_assignable<T>::value &&
              std::is_nothrow_move_constructible<T>::value )
  {
    static_assert(std::is_move_assignable<T>::value,"No matching move assignment operator for type T");
    static_assert(std::is_move_constructible<T>::value,"No matching move constructor for type T");

    m_constructor = std::move(rhs.m_constructor);
    m_destructor  = std::move(rhs.m_destructor);

    if(rhs.m_is_initialized)
    {
      if(m_is_initialized){
        assign(std::move(*rhs));
      }else{
        construct(std::move(*rhs));
      }
      rhs.m_is_initialized = false; // disallow rhs from destructing the content
    }

    return (*this);
  }

  template<typename T>
  inline typename Lazy<T>::value_type& Lazy<T>::operator=( const value_type& rhs )
    noexcept( std::is_nothrow_copy_assignable<T>::value &&
              std::is_nothrow_copy_constructible<T>::value )
  {
    static_assert(std::is_copy_assignable<T>::value,"No matching copy assignment operator for type T");
    static_assert(std::is_copy_constructible<T>::value,"No matching copy constructor for type T");

    if(m_is_initialized){
      assign(rhs);
    }else{
      construct(rhs);
    }

    return *ptr();
  }

  template<typename T>
  inline typename Lazy<T>::value_type& Lazy<T>::operator=( value_type&& rhs )
    noexcept( std::is_nothrow_move_assignable<T>::value &&
              std::is_nothrow_move_constructible<T>::value )
  {
    static_assert(std::is_move_assignable<T>::value,"No matching move assignment operator for type T");
    static_assert(std::is_move_constructible<T>::value,"No matching move constructor for type T");

    if(m_is_initialized){
      assign(rhs);
    }else{
      construct(rhs);
    }

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
    new (ptr()) T(x);
    m_is_initialized = true;
  }

  template<typename T>
  template<typename...Args>
  inline void Lazy<T>::construct( ctor_va_args_tag tag, Args&&...args ) const
    noexcept(std::is_nothrow_constructible<T,Args...>::value)
  {
    destruct();
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
    static_assert(std::is_constructible<T,Args...>::value,"No matching constructor for type T with given arguments");

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

  template<typename T>
  inline void Lazy<T>::assign( const value_type& rhs ) const
    noexcept(std::is_nothrow_copy_assignable<T>::value)
  {
    (*ptr()) = rhs;
  }

  template<typename T>
  inline void Lazy<T>::assign( value_type&& rhs ) const
    noexcept(std::is_nothrow_move_assignable<T>::value)
  {
    (*ptr()) = rhs;
  }

}
