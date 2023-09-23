# `io_uring` implementation

This part of the library is originally copied from liburing 2.4 (or 2.5, [here's the exact commit](https://github.com/axboe/liburing/commit/b58921e0b0ae84b6f1cd22b87c66a6e91a540ec8)).
It's been modified to be used inside the a header-only C++ library.

Changes from liburing:

- Removal of some platform specific stuff
- Removal of no-libc support feature, this library is a C++ that can't function without the standard library, so we're ok
- ...

### License

I've included the MIT license in the source code, but I'm not sure if it's required or not because I've changed so much.
