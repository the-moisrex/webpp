# Allocator System

__Attention__: this article is a work in progress. Still not implemented.

The reason behind the whole project being a header-only project is originally lies around
the allocators. Even though allocators are cheap enough for a web server and other programming
languages don't even have such a thing, and honestly it's a little hard to have them all over
the code base, I decided that it's too good of a thing to just dismiss.

## Why not polymorphic allocators only?

With dynamic allocators are great, but the thing is that they usually require
at least one more indirection and I promised myself that I will not put any
unnecessary indirections in the project specially in high performance places
(which means no `virtual` keyword in high throughput places of the core library).

We totally could've avoided `trait` types if we wanted to use `virtual`s and thus
we probably could've saved ourselves lots of meta programmings as well, and we also
could've been able to use dynamic linking as well as static linking (thus the project
could become non-header-only library as well).

But that's the decision that I made from almost the beginnings of this project
because implementing that much code with `virtual` being allowed, there's no going
back that easily.

## Allocator Pack

There are a few places that one type of allocator can help more than the others.
The `trait`s type that the user provides uses an allocator pack instead of just one
allocator type.

This way, the core library can pick the best allocator from that allocator pack.

- __New And Delete Allocators__: the `std::allocator` essentially
- __Linear Allocators__: it only `free`s the memory only on destruction. Useful for _buffers_, _string generators_, ...
- __Simple Segregated Storage__: like `boost.pool`
- ...

### Possible allocator features

These are the features that each type can choose between them, or
combine them so the allocator pack can choose the best allocator from
the allocator pack.

- __noop_free__: don't need to `free` on `deallocate`; but only on destroy.
- __equal_sizes__: we're going to allocate lots of stuff with equal sizes.
- __contagious__: we need to use it in contagious memory
- __non_contagious__: the opposite of _contagious_.
- __sync__: is going to be used in a multithreaded environment
- __no_sync__: don't care about thread-synchronization.
- __fixed__: fixed size, don't need more than specified size.
- __local__: not good for the duration of the process (https://youtu.be/nZNd5FjSquk?t=1009)
- __global__: good for duration of the process
- __monotonic__:
- __stateless__
- __stateful__
- __stack__

From (https://youtu.be/nZNd5FjSquk?t=355):
- __fast__
- __shared__
- __protected__
- __mapped__
- __testing__
- __debugging__
- __measuring__
- __better_locality__
- __less_connections__

### some types of allocators:
from: https://slembcke.github.io/2020/10/12/CustomAllocators.html

- __Slab Allocator__: When you need to keep a pool of short-lived allocations that are all the same size.
- __Linear Allocator__: When you need fast temporary memory with a finite lifespan.
- __Zone Allocator__: When you need fast temporary memory with a finite lifespan, but don’t know how much you’ll need.
- __Buddy Block Allocator__: When you need a general purpose allocator with predictable performance.

from: https://metricpanda.com/rival-fortress-update-16-custom-memory-allocators/

- __PushAllocator__: A simple stack based allocator that, given a block of memory, partitions it out by incrementing a location in a similar manner to how the stack pointer works.
- __SlidingAllocator__: Works on top of a stack allocator and grows until the parent allocator has memory. The characteristic of this allocator is that it can rewind all allocations in order to reset the allocated memory. This is useful as a “one-frame-only” allocator for temporary operations that get wiped at the end of the frame.
- __FreeableAllocator__: This is one of the few allocators that can reclaim memory and repartition it. It has the capability of merging contiguous free blocks and reallocate existing pointers.
- __ScopeAllocator__: A temporary allocator that discards any allocations and rewinds the end of the scope.
- __ArrayAllocator__: Used for growing lists of lists.
- __DoubleEndStackAllocator__: Similar to the stack allocator, but can grow from both the head or the tail of the memory block.
- __ObjectPool__: A simple bounded pool of struct that can get recycled by last usage time. This is used in the engine to retire old rendering “chunks” and replace them with new ones without needing to allocate.
- __StringPool__: A string interner used for all strings used by the game and engine that allows fast lookups through hashes and string recycling.

#### Some implementations
https://github.com/mtrebi/memory-allocators
https://github.com/foonathan/memory
https://godbolt.org/z/rvfofv

### Allocation strategies:
https://youtu.be/nZNd5FjSquk?t=1894

Dimensional Characteristics(https://youtu.be/CFzuFNSpycI?t=1099):
- Fragmentablity
- Allocation Density
- Chunk-Size Variation
- Access Locality
- Memory Utilization
- Allocator Contention

### Density and Variation
https://youtu.be/CFzuFNSpycI?t=1130

Global allocators are very bad for "short" and "focused" systems while ignoring Contention.

### Locality
https://youtu.be/CFzuFNSpycI?t=2865
for long-running systems, locality matters and how long the
allocation and deallocation took don't.

#### For long-running systems:
 - find the parts with high utilization
 - use local allocators there
 - or copy the data to a local allocator, use it, and copy back to the actual allocator

this is because the long-running systems, the time to allocate/deallocate don't matter.
ONLY LOCALITY MATTERS IN LONG RUNNING SYSTEMS.


### Utilization
https://youtu.be/CFzuFNSpycI?t=3195

For systems with High Utilization, a monotonic_features is very performant.

Never use monotonic_features allocator by itself when the utilization
is low and total number of bytes allocated is large

### Contention
https://youtu.be/CFzuFNSpycI?t=3372

### Fragmentability
https://youtu.be/CFzuFNSpycI?t=3473

### Performance improvements
https://youtu.be/CFzuFNSpycI?t=3547

- Ensure physical locality of allocated memory
- Avoid memory diffusion in long-running systems
- Obviate deallocation of individual allocations
- Sidestep contention during concurrent allocations
- Separate unrelated data to avoid false sharing
- Compose effective allocation strategies

### Kinds of memories
https://youtu.be/CFzuFNSpycI

- Static memory
- Memory-mapped memory
- Read/Write protectable memory
- Fast memory (special architectures)
- Shared memory (special allocators)


### Debugging allocators
https://youtu.be/CFzuFNSpycI?t=3564

- Counting (auditing) allocator
- Test allocator
- Limit allocator
- Read/Write protectable memory allocator
- Alarm allocator (if monotonic_features allocator overflows)


## Object Utilities

Some of the utilities that I can think of:

```c++
int main() {
    allocator_pack<stl::pmr::allocator_descriptors> alloc_pack;
    
    // use the local object
    local<std::string> str1{alloc_pack, "hello world"};
    
    // use the templated version
    local<std::basic_string> str2{alloc_pack, "hello world 2"};
    
    // two on the same stack:
    resource::stack<1024> buf{alloc_pack};
    local<std::string> str3{buf, "hello world 3"};
    local<std::string> str4{buf, "hello world 4"};
}
```
or with the name of resource_pack?

```c++
int main() {
    resource_pack<stl::pmr::allocator_descriptors> res_pack;
    
    // use the local object
    local<std::string> str1{res_pack, "hello world"};
    
    // use the templated version
    local<std::basic_string> str2{res_pack, "hello world 2"};
    
    // two on the same stack:
    resource::stack<1024> buf{res_pack};
    local<std::string> str3{buf, "hello world 3"};
    local<std::string> str4{buf, "hello world 4"};
}
```

and in structs:

```c++
// this is the best way, to be as portable as possible
template <typename Allocator>
struct pen {};

template <Traits TraitsType>
struct heavy_pen {
    using traits_type          = TraitsType;
    using allocator_pack_type  = traits::allocator_pack<traits_type>;
    static constexpr auto features = general_features;
    using string_type          = traits::string<traits_type, features>; // global string
    
    allocator_pack_type allocs{};
    
    string_type one() {
        local<string_type> str{allocs};
        // calculate some string
        return str.global_copy(); // convert to general string
    }
    
    string_type two() {
        string_type str{allocs.template get_for<string_type>()};
        // ...
        return str;
    }
    
    string_type three() {
        auto str = make<string_type>(allocs);
        // ...
        return str;
    }
};
```
