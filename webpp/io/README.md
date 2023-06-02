# I/O Operations

There are many I/O designs out there, many event loops, many thread-pools, etc. The problem seems to be that it's really hard to have it all and still be easy and performant.

Our goal for now is:

- Multi-IO design: we should be (at least in the future) be able to change the underlying io design more easily.
- Easy to use, hard to misuse (from io_uring)
- Performance, duh


