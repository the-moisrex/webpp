# WEB++ (C++ Web Framework)
[![Build Status](https://travis-ci.com/the-moisrex/webpp.svg?branch=master)](https://travis-ci.com/the-moisrex/webpp)

This is a web framework written in C++ and hopefully will be used by other programming languages other than C++ too.

It's being developed. News will be shared on [Telegram](https://t.me/webpp).

This project on:

* [Telegram](https://t.me/webpp)
* [GitHub](https://github.com/the-moisrex/webpp)
* [GitLab](https://gitlab.com/webpp/webpp)

Look at the [Core Readme file](./core/README.md) for the core concepts of the project.

## Project goals

- To remove the necessity for dealing with low-level networking APIs
- Cross-Platform: no need to deal with OS-specific APIs
- Modular: to use other people's written apps and modules in your code easily
- Cross-Protocol: Remove the necessity for dealing with specific protocols (CGI/FCGI/...)
- Cross-Database-API: being able to access wide range of databases without dealing with DB-specific APIs and switch between them in the config files instead of changing the code itself.
- Parallelization Access: being able to compute things in parallel easily
- Implement Common Patterns easily

## What can you should be able to do with this library?

The project is still in development, but this framework should be able to
help you with these kinda projects someday:

- Develop a static/dynamic website easily in C++
- Using other people's already written websites/modules/components in your app
- Writing long-runnig web apps and store temporary data in variables instead of database (not all languages can do that)

## Why C++?

The most question that I get while talking to other developers is why C++?
Well there are multiple answers:

- C++ does have the potential specially the newer versions of C++
- Having full power of C++ and your OS at your disposal
- WebAssembly is getting stronger, you will be able to write both back-end and front-end in the same language with C++ (even though you already can if you choose JavaScript for example, but here's another option)
- Has the potiential to be faster than other languages
- C++ can be easy if the library is easy to use.
- Using modern C++ is fun
- Compile-Time computation is something that youeither can't achive or it's going to be difficult to achieve in other languages
- Remove the necessity to learn other languages if you're already familiar with C++
- Using older codes in your project more easily
- Multi-threading access: not all languages provide that access easily specially in a web framework

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


Current Todo List (use `./gtask-markdown list` to generate this table again):

### Todo List
Last updated time: Tue Oct  5 06:00:27 PM UTC 2021

| ID | Age    | Project     | Description                                                                                                                                     | Urg |
| -- | ------ | ----------- | ----------------------------------------------------------------------------------------------------------------------------------------------- | ---- |
| 11 | 2.0y   |             | put the validations in their own file                                                                                                           |    2 |
| 18 | 1.7y   |             | Use Steganography in your project, it's cool                                                                                                    |    2 |
| 50 | 1.5y   |             | add thread_pool support                                                                                                                         |    2 |
| 62 | 1.5y   |             | add jetbrains logo                                                                                                                              |    2 |
| 74 |  10mo  |             | remove magic_enums as dependency                                                                                                                | 1.67 |
| 77 |   9mo  | STL         | should we add 'using namespace std;' in webpp::istl?                                                                                            | 2.53 |
|  8 | 2.1y   | ams         | consider help functions for webassembly                                                                                                         |    3 |
| 63 | 1.4y   | clang       | Add custom clang-check matchers to chatch some bugs before they happen for the final user                                                       |    3 |
| 53 | 1.5y   | cluster     | add clustering communication base classes                                                                                                       |    3 |
| 54 | 1.5y   | cluster     | clusters can run a thread_pool too                                                                                                              |    3 |
| 55 | 1.5y   | cluster     | implement thread_pool classes                                                                                                                   |    3 |
| 56 | 1.5y   | cluster     | thread-safe properties with the help of marcos or pre-compile code generation based on the spacified class or type                              |    3 |
| 57 | 1.5y   | cluster     | see if it's possible to use operator overloading magics to implement threadsafe properties                                                      |    3 |
|  3 | 2.2y   | cmake       | use components like cefeika                                                                                                                     |    3 |
|  6 | 2.1y   | cmake       | consider cpp modules                                                                                                                            |    3 |
|  7 | 2.1y   | cmake       | add different packages for distribution                                                                                                         |    3 |
| 28 | 1.6y   | cmake       | add pre-compiling the static files feature in the cmake for different file types in the generate_static_file function                           |    3 |
| 30 | 1.5y   | cmake       | configure and use sanitizers                                                                                                                    |    3 |
| 76 |   9mo  | cmake       | use source_group to group files logically                                                                                                       | 2.59 |
| 29 | 1.6y   | codegen     | forms for auto code generation, so the user can easily write a boilerplate and change the code later.                                           |    3 |
| 65 | 1.4y   | cookies     | parse response cookies                                                                                                                          |    3 |
| 66 | 1.4y   | cookies     | add cookie extensions                                                                                                                           |    3 |
| 67 | 1.4y   | cookies     | add extension for session management                                                                                                            |    3 |
| 68 | 1.4y   | cookies     | implement encrypted cookies                                                                                                                     |    3 |
| 81 |   1d   | crypto      | implement gzip using openssl if possible                                                                                                        | 1.01 |
| 32 | 1.5y   | fcgi        | convert endpoint fromats                                                                                                                        |    3 |
| 33 | 1.5y   | fcgi        | find where to add the thread_pool                                                                                                               |    3 |
| 34 | 1.5y   | fcgi        | add the ctor of threadpool                                                                                                                      |    3 |
| 35 | 1.5y   | fcgi        | use threadpool in running the io_context                                                                                                        |    3 |
| 36 | 1.5y   | fcgi        | restart the server when the endpoints change                                                                                                    |    3 |
| 37 | 1.5y   | fcgi        | learn boost::cpp03::server4::server::operator() example                                                                                         |    3 |
| 38 | 1.5y   | fcgi        | add thread_count in fcgi class                                                                                                                  |    3 |
| 39 | 1.5y   | fcgi        | implement request methods in fcgi                                                                                                               |    3 |
| 40 | 1.5y   | fcgi        | write a fcgi hello world example                                                                                                                |    3 |
| 41 | 1.5y   | fcgi        | write fcgi tests                                                                                                                                |    3 |
| 42 | 1.5y   | fcgi        | read the fcgi header async                                                                                                                      |    3 |
| 43 | 1.5y   | fcgi        | parse the fcgi header                                                                                                                           |    3 |
| 44 | 1.5y   | fcgi        | read the rest of the fcgi request                                                                                                               |    3 |
| 45 | 1.5y   | fcgi        | parse the rest of the fcgi request                                                                                                              |    3 |
| 46 | 1.5y   | fcgi        | add constructors for protocols' file's classes                                                                                                  |    3 |
| 47 | 1.5y   | fcgi        | figure out a way to listen on multiple endpoints                                                                                                |    3 |
| 12 | 1.8y   | functional  | implement trailing in debounce class                                                                                                            |    3 |
| 13 | 1.8y   | functional  | add a way to use a thread pool in the debounce class                                                                                            |    3 |
| 14 | 1.8y   | functional  | make property class thread safe if the user wants it                                                                                            |    3 |
| 15 | 1.8y   | functional  | add functionality to check if the user is running the program in debug mode or release mode                                                     |    3 |
| 16 | 1.8y   | functional  | add functionality to the properties class and debounced class to check if the user is going to have a race condition or not if all possible     |    3 |
| 17 | 1.8y   | functional  | check if you actually need the function_ref class or not                                                                                        |    3 |
| 64 | 1.4y   | headers     | give the user the ability to change the status phrase of status code                                                                            |    3 |
|  1 | 2.3y   | interfaces  | implement fcgi                                                                                                                                  |    3 |
|  5 | 2.1y   | interfaces  | implement wsgi                                                                                                                                  |    3 |
| 80 |   7mo  | log         | use c++20 std::source_location for logging                                                                                                      | 2.27 |
| 19 | 1.6y   | modules     | add mother modules to the server's templtes                                                                                                     |    3 |
| 10 | 2.0y   | net         | add mac address class                                                                                                                           |    3 |
| 21 | 1.6y   | net         | add fragment routing methods in uri (think about it first)                                                                                      |    3 |
| 22 | 1.6y   | net         | implement put_query in uri                                                                                                                      |    3 |
| 23 | 1.6y   | net         | implement get_query in uri                                                                                                                      |    3 |
| 24 | 1.6y   | net         | implement remove_query in uri                                                                                                                   |    3 |
| 25 | 1.6y   | net         | implement append_query in uri                                                                                                                   |    3 |
| 26 | 1.6y   | net         | implement pop_path in uri                                                                                                                       |    3 |
| 27 | 1.6y   | net         | implement append_path in uri                                                                                                                    |    3 |
| 75 |  10mo  | net         | add 128bit support to ipv6 class for systems that supports them                                                                                 | 2.67 |
|  2 | 2.2y   | performance | implement half-lazy loading of images/... stuff                                                                                                 |    3 |
| 49 | 1.5y   | performance | optimize ipv6 and checkout the benchmark                                                                                                        |    3 |
| 70 |  10mo  | performance | add logger_allocator custom allocator                                                                                                           | 2.69 |
| 71 |  10mo  | performance | add add null_allocator custom allocator to help debug allocations                                                                               | 2.69 |
| 72 |  10mo  | performance | add add allocator category concept                                                                                                              | 2.69 |
| 73 |  10mo  | performance | add add allocator categories to the traits                                                                                                      | 2.69 |
| 48 | 1.5y   | pub         | Make travis build successful                                                                                                                    |    3 |
| 20 | 1.6y   | request     | sort paths in router based on views and how much time it takes for them to be evaluated                                                         |    3 |
| 51 | 1.5y   | tests       | add a high level testing library that uses gtest and catch2 and boost.test and others                                                           |    3 |
| 52 | 1.5y   | tests       | convert existing tests into your own testing library                                                                                            |    3 |
| 69 |  10mo  | tests       | write tests for utils/errors                                                                                                                    | 2.69 |
| 58 | 1.5y   | thread      | add fast and thread-safe class template for containers                                                                                          |    3 |
| 78 |   9mo  | traits      | remove stateful and stateless from allocator features                                                                                           | 2.53 |
| 31 | 1.5y   | uri         | add support for encrypted URIs                                                                                                                  |    3 |
| 79 |   9mo  | uri         | add support for RFC 6570 (URI Template)                                                                                                         |  2.5 |
|  9 | 2.1y   | utils       | implement property class                                                                                                                        |    3 |
| 82 |   1h   | utils       | Add archive file compression utilities like .zip                                                                                                |    1 |
| 59 | 1.5y   | valves      | a valve that limits user visits                                                                                                                 |    3 |
| 60 | 1.5y   | valves      | a valve that only allows guests                                                                                                                 |    3 |
| 61 | 1.5y   | valves      | a valve that only allows registered users                                                                                                       |    3 |
|  4 | 2.1y   | webrtc      | watch WebRTC tutorials                                                                                                                          |    3 |



## Supported by:
[JetBrains](https://www.jetbrains.com/?from=webpp)
