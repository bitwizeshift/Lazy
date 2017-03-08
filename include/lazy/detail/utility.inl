namespace lazy {
  namespace detail {

    template <typename T, typename Tuple, std::size_t... I>
    inline T* uninitialized_tuple_construct_at_impl( void* ptr, Tuple&& t, std::index_sequence<I...> )
    {
      new (ptr) T(std::get<I>(std::forward<Tuple>(t))...);
      return static_cast<T*>(ptr);
    }

  } // namespace detail

  template<typename T, typename Tuple>
  inline T* uninitialized_tuple_construct_at( void* ptr, Tuple&& tuple )
  {
    return detail::uninitialized_tuple_construct_at_impl<T>(
      ptr,
      std::forward<Tuple>(tuple),
      std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>()
    );
  }

  template<typename T>
  inline void destroy_at( T* p )
  {
    p->~T();
  }
} // namespace lazy
