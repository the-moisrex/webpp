# Property

I'm thinking about some ideas about properties; it's not a good idea to implement them all (maybe). So here I am trying to gather my thoughts about this topic in one place.

## Possible features

- [ ] Adding watchers (signal & slot)
- [ ] Sync

## Additional memory usage

The memory usage needs to be low because we're going to use this for lots of things (well the user of this library is going to actually).

Idea: combine multiple properties into one property (user can do it with a custom class type).

### Separate memory for each property

- [ ] easier for the user and the implementer


### All in one place

- [ ] Is there a reason to have separate storages?

### Stored in an allocator

- [ ] Passing an allocator for just an int?

## Performance
Since there is a sync penalty, we have to optimize the syncing implementations so it uses the best implementation.

## Custom Allocators

- [ ] An idea is to just leave the allocator to the user type and put everything on the stack

## Using inside an Application class

Example usage:

````c++
class app {

  sync<int> visits = 0;

  auto page() {
    return "page";
  }

  auto operator(Request auto&& req) {
    // ...
    visits++;
    return res;
  }

};
````


