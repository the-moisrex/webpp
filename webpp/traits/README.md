# Traits System
_Traits_ are special types that contain other types and static methods
that will help the users of the library to customize the library through
using other types for specific purposes. Fox example to use a different
`Allocator` STL uses or a different _character type_ in the strings or
even configure UTF-8.

```c++
struct traits_example {
    using char_type             = char;
    using logger_type           = stderr_logger;
    using allocator_descriptors = stl::allocator_descriptors;
    using string_view           = stl::basic_string_view<char_type, stl::char_traits<char_type>>;

    template <typename AllocT>
    using string = stl::basic_string<char_type, stl::char_traits<char_type>, AllocT>;
};
```

- `stderr_logger` is a standard _Logger_ type which satisfies the concept (look at the `logs` directory).
- `stl::alloc_descriptors` describes the possible allocators you want the library to use internally 
  (look at the `memory` directory).

