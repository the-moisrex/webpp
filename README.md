# WEB++ (C++ Web Framework)
[![Build Status](https://travis-ci.com/the-moisrex/webpp.svg?branch=master)](https://travis-ci.com/the-moisrex/webpp)

This is a web framework written in C++ and hopefully will be used by other programming languages other than C++ too.

It's being developed. News will be shared on [Telegram](https://t.me/webpp).

This project on:

* [Telegram](https://t.me/webpp)
* [GitHub](https://github.com/the-moisrex/webpp)
* [GitLab](https://gitlab.com/webpp/webpp)

Look at the [Core Readme file](./core/README.md) for the core concepts of the project.

## Dependencies

These are the dependencies this project requires for certain parts of the project
to function properly. Some of these libraries haven't been integrated into the project
yet since we're still working on this project.

Some of these dependencies can replace other dependencies; the difference is usually in 
their performance but other factors are in play as well.

- `boost` (you don't need it for the core)
- `boost::asio` or `asio` (for FastCGI and Self-Hosted interfaces only)
- `boost::beast` _(optional)_ (for benchmarking only)
- `fmt`: as a fallback for `std::format`
- `eve` _(optional)_: for better performance with SIMD
- `openssl` (used by asio)
- _(optional)_ for `gzip` support one of these:
  - `zlib` _(optional)_
  - `zlib-ng` _(optional)_
  - `crypto++` _(optional)_
  - `libdeflate` _(optional)_
- `zstd` _(optional)_: for `zstd` compression support
- `brotli` _(optional)_: for `br` compression support
- `CTRE` _(optional)_: compile-time-regular-expression
- `modp_64`: modified version is included in the sources

## Development
You can help us develop this project if you're familiar with C++ programming language and web development. Even if you're not really good at web development but you know C++ very well, specially C++17 and C++20 (clang and gcc c++2a is enough), then we have lots of things that you can help with which doesn't have anything to do with web development directly.

I'd appreciate any help of any kind. Even if you're not interested in coding, here are some ways you can help out:
- **Writing tests**; I dare you to make me mad by writing too much tests 😁
- **Writing benchmarks**: I write lots of benchmarks (even outside the scope of this project) to make sure I'm using the right tool/library/feature/class/template/... .
- **Make jokes**: make (not) funny jokes in our [@webpp_discuss](https://t.me/webpp_discuss) group
- **Share ideas**: I have plenty of ideas, and I want more.
- **Report bugs**: you can find me in the [@webpp_discuss](https://t.me/webpp_discuss) group; report the bugs there; or open issues on github/gitlab.
- **Financial support**: this project is open-source; we'll need some money for hosting and advertising.
- **Share on social-media**: I'll appreciate any shout-out about this project; even if you give a bad feedback.
- **Learn &/ Teach**: we can teach you and also learn from you; so let's talk code!
- **Suggest someone** who can help: I'd appreciate any help on finding people who can help in any way.
- **Write documentation**: writing documentation is so much fun that I don't want to do it alone! 😂


### Examples
In the `examples` directory you can find examples. That's not much but it'll give you they high level viewpoint. Take a look at them and you can be sure that we'll try to write tests for all of them; so if you read tests, you'll learn even more about the project.

### Tests
We currently use `googletest` library for unit/integrated testing. This might change in a few years though.

There are many tests you can read, confirm, fix, and/or write. They are the perfect place to start learning about the project after the examples.

You currently can run tests just by running the `webpptest` executable. It's as easy as that.

### Benchmarks
Benchmarks are done for us developers so we know which tool/class/implementation/library/framework/solution that we choose is the best. Currently we are using Googles' mini-benchmark library for this purpose.

### Documentation
We don't have a documentation/tutorial/guide at this point. You can help us write one. We're just too busy writing the code that we don't have much time writing documentations at this point. But you can be sure from the point that this project becomes production-ready, we'll have documentations.

### Build
Requirements for building and testing:

* At least C++20 (we will be using C++20/23/... as they come and deprecate old versions)
* CMake
* Boost
* Googletest (gtest)

```bash
git clone https://gitlab.com/webpp/webpp.git
cd webpp/
mkdir build/
cd build
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug ..
ninja
sudo ninja install # needed only for building the examples
```

And then in order to run the tests, you just have to run the `webpptest` binary in that directory. You don't need to build tests individually but you have to build examples individually; but this may change when we hit our first production-ready version.

## About GTASK (task management tool)

We use `gtask` (a custom script built on top of [TaskWarrior](https://taskwarrior.org/) for task management) as a ToDo list. Of course long-term todos will not be there so the ideas don't get out before they need to.

### Usage:

```bash
gtask list
gtask add project:net "This is a todo"
```

Learn more about TaskWarrior in their own documentation.

## Supported by:
[JetBrains](https://www.jetbrains.com/?from=webpp)
