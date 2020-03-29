# webpp
C++ web framework

This is a web framework written in C++ and hopefully will be used by other programming languages other than C++ too.

It's being developed. News will be shared on [Telegram](https://t.me/webpp).

This project on:

* [Telegram](https://t.me/webpp)
* [GitHub](https://github.com/the-moisrex/webpp)
* [GitLab](https://gitlab.com/the.moisrex/webpp)

## Development
You can help us develop this project if you're familiar with C++ programming language and web development. Even if you're not really good at web development but you know C++ very well, specially C++17 and C++20 (clang and gcc c++2a is enough), then we have lots of things that you can help with which doesn't have anything to do with web development directly.

### Examples
In the `examples` directory you can find examples. That's not much but it'll give you they high level viewpoint. Take a look at them and you can be sure that we'll try to write tests for all of them; so if you read tests, you'll learn even more about the project.

### Write tests
There are many tests you can read, confirm, fix, and/or write. They are the perfect place to start learning about the project after the examples.

### Documentation
We don't have a documentation/tutorial/guide at this point. You can help us write one. We're just too busy writing the code that we don't have much time writing documentaions at this point. But you can be sure from the point that this project becomes production-ready, we'll have documentations.

### Build
Requirements for building and testing:

* At least C++17 (we will be using C++20/23/... as they come and depricate old versions)
* CMake
* Boost
* Googletest (gtest)

```bash
git clone https://gitlab.com/the.moisrex/webpp.git
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
