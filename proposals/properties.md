I'm thinking about some ideas about properties; it's not a good idea to implement them all (maybe). So here I am trying to gather my thoughts about this topic in one place.

## Possible features

- [ ] Adding watchers (signal & slot)

## Additional memory usage

The memory usage needs to be low because we're going to use this for lots of things (well the user of this library is going to actually).

### Separate memory for each property

- [ ] easier for the user and the implementer


### All in one place

- [ ] Is there a reason to have separate storages?

### Stored in an allocator

- [ ] Passing an allocator for just an int?

## Performance

## Custom Allocators

## Using inside an Application class

