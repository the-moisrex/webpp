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

## TODO
About GTask (task management tool) (Git + TaskWarrior)

We use `gtask` (a custom script built on top of [TaskWarrior](https://taskwarrior.org/) for task management) as a ToDo list. Of course long-term todos will not be there so the ideas don't get out before they need to.

### Usage:

```bash
gtask list
gtask add project:net "This is a todo"
```

Learn more about TaskWarrior in their own documentation.


Current Todo List (use `gtask-markdown list` to generate this table again):



| ID | Age  | Project     | Description                                                                                                                                     | Urg |
| -- | ---- | ----------- | ----------------------------------------------------------------------------------------------------------------------------------------------- | ---- |
| 11 | 1.2y |             | put the validations in their own file                                                                                                           |    2 |
| 18 | 10mo |             | Use Steganography in your project, it's cool                                                                                                    | 1.77 |
| 50 | 7mo  |             | add thread_pool support                                                                                                                         |  1.3 |
| 62 | 7mo  |             | add jetbrains logo                                                                                                                              | 1.24 |
|  8 | 1.2y | ams         | consider help functions for webassembly                                                                                                         |    3 |
| 63 | 6mo  | clang       | Add custom clang-check matchers to chatch some bugs before they happen for the final user                                                       | 2.13 |
| 53 | 7mo  | cluster     | add clustering communication base classes                                                                                                       | 2.28 |
| 54 | 7mo  | cluster     | clusters can run a thread_pool too                                                                                                              | 2.28 |
| 55 | 7mo  | cluster     | implement thread_pool classes                                                                                                                   | 2.28 |
| 56 | 7mo  | cluster     | thread-safe properties with the help of marcos or pre-compile code generation based on the spacified class or type                              | 2.25 |
| 57 | 7mo  | cluster     | see if it's possible to use operator overloading magics to implement threadsafe properties                                                      | 2.25 |
|  3 | 1.4y | cmake       | use components like cefeika                                                                                                                     |    3 |
|  6 | 1.2y | cmake       | consider cpp modules                                                                                                                            |    3 |
|  7 | 1.2y | cmake       | add different packages for distribution                                                                                                         |    3 |
| 28 | 9mo  | cmake       | add pre-compiling the static files feature in the cmake for different file types in the generate_static_file function                           | 2.51 |
| 30 | 8mo  | cmake       | configure and use sanitizers                                                                                                                    | 2.35 |
| 29 | 9mo  | codegen     | forms for auto code generation, so the user can easily write a boilerplate and change the code later.                                           |  2.5 |
| 65 | 6mo  | cookies     | parse response cookies                                                                                                                          | 2.02 |
| 66 | 6mo  | cookies     | add cookie extensions                                                                                                                           | 2.02 |
| 67 | 6mo  | cookies     | add extension for session management                                                                                                            | 2.02 |
| 68 | 6mo  | cookies     | implement encrypted cookies                                                                                                                     | 2.02 |
| 32 | 8mo  | fcgi        | convert endpoint fromats                                                                                                                        | 2.35 |
| 33 | 8mo  | fcgi        | find where to add the thread_pool                                                                                                               | 2.35 |
| 34 | 8mo  | fcgi        | add the ctor of threadpool                                                                                                                      | 2.35 |
| 35 | 8mo  | fcgi        | use threadpool in running the io_context                                                                                                        | 2.35 |
| 36 | 8mo  | fcgi        | restart the server when the endpoints change                                                                                                    | 2.35 |
| 37 | 8mo  | fcgi        | learn boost::cpp03::server4::server::operator() example                                                                                         | 2.35 |
| 38 | 8mo  | fcgi        | add thread_count in fcgi class                                                                                                                  | 2.35 |
| 39 | 8mo  | fcgi        | implement request methods in fcgi                                                                                                               | 2.35 |
| 40 | 8mo  | fcgi        | write a fcgi hello world example                                                                                                                | 2.35 |
| 41 | 8mo  | fcgi        | write fcgi tests                                                                                                                                | 2.35 |
| 42 | 8mo  | fcgi        | read the fcgi header async                                                                                                                      | 2.35 |
| 43 | 8mo  | fcgi        | parse the fcgi header                                                                                                                           | 2.35 |
| 44 | 8mo  | fcgi        | read the rest of the fcgi request                                                                                                               | 2.35 |
| 45 | 8mo  | fcgi        | parse the rest of the fcgi request                                                                                                              | 2.35 |
| 46 | 8mo  | fcgi        | add constructors for protocols' file's classes                                                                                                  | 2.35 |
| 47 | 8mo  | fcgi        | figure out a way to listen on multiple endpoints                                                                                                | 2.35 |
| 12 | 11mo | functional  | implement trailing in debounce class                                                                                                            | 2.95 |
| 13 | 11mo | functional  | add a way to use a thread pool in the debounce class                                                                                            | 2.95 |
| 14 | 11mo | functional  | make property class thread safe if the user wants it                                                                                            | 2.95 |
| 15 | 11mo | functional  | add functionality to check if the user is running the program in debug mode or release mode                                                     | 2.95 |
| 16 | 11mo | functional  | add functionality to the properties class and debounced class to check if the user is going to have a race condition or not if all possible     | 2.95 |
| 17 | 11mo | functional  | check if you actually need the function_ref class or not                                                                                        | 2.95 |
| 64 | 6mo  | headers     | give the user the ability to change the status phrase of status code                                                                            | 2.03 |
|  1 | 1.4y | interfaces  | implement fcgi                                                                                                                                  |    3 |
|  5 | 1.2y | interfaces  | implement wsgi                                                                                                                                  |    3 |
| 19 | 9mo  | modules     | add mother modules to the server's templtes                                                                                                     | 2.56 |
| 10 | 1.2y | net         | add mac address class                                                                                                                           |    3 |
| 21 | 9mo  | net         | add fragment routing methods in uri (think about it first)                                                                                      | 2.56 |
| 22 | 9mo  | net         | implement put_query in uri                                                                                                                      | 2.56 |
| 23 | 9mo  | net         | implement get_query in uri                                                                                                                      | 2.56 |
| 24 | 9mo  | net         | implement remove_query in uri                                                                                                                   | 2.56 |
| 25 | 9mo  | net         | implement append_query in uri                                                                                                                   | 2.56 |
| 26 | 9mo  | net         | implement pop_path in uri                                                                                                                       | 2.56 |
| 27 | 9mo  | net         | implement append_path in uri                                                                                                                    | 2.56 |
|  2 | 1.4y | performance | implement half-lazy loading of images/... stuff                                                                                                 |    3 |
| 49 | 7mo  | performance | optimize ipv6 and checkout the benchmark                                                                                                        | 2.31 |
| 48 | 8mo  | pub         | Make travis build successful                                                                                                                    | 2.33 |
| 20 | 9mo  | request     | sort paths in router based on views and how much time it takes for them to be evaluated                                                         | 2.56 |
| 51 | 7mo  | tests       | add a high level testing library that uses gtest and catch2 and boost.test and others                                                           |  2.3 |
| 52 | 7mo  | tests       | convert existing tests into your own testing library                                                                                            |  2.3 |
| 69 | 19h  | tests       | write tests for utils/errors                                                                                                                    |    1 |
| 58 | 7mo  | thread      | add fast and thread-safe class template for containers                                                                                          | 2.25 |
| 31 | 8mo  | uri         | add support for encrypted URIs                                                                                                                  | 2.35 |
|  9 | 1.2y | utils       | implement property class                                                                                                                        |    3 |
| 59 | 7mo  | valves      | a valve that limits user visits                                                                                                                 | 2.24 |
| 60 | 7mo  | valves      | a valve that only allows guests                                                                                                                 | 2.24 |
| 61 | 7mo  | valves      | a valve that only allows registered users                                                                                                       | 2.24 |
|  4 | 1.3y | webrtc      | watch WebRTC tutorials                                                                                                                          |    3 |




## Supported by:
[JetBrains](https://www.jetbrains.com/?from=webpp)
