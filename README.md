# WEB++ (C++ Web Framework)

![Web++ Logo](./assets/logo-bordered.svg)


This is a web framework written in C++ and hopefully will be used by other programming languages other than C++ too.

It's being developed. News will be shared on [Telegram](https://t.me/webpp).

This project on:

* [Telegram](https://t.me/webpp)
* [GitHub](https://github.com/the-moisrex/webpp)
* [GitLab](https://gitlab.com/webpp/webpp)

Look at the [Core Readme file](./webpp/README.md) for the core concepts of the project.

## Project goals

- To remove the necessity for dealing with low-level networking APIs
- Cross-Platform: no need to deal with OS-specific APIs
- Modular: to use other people's written apps and modules in your code easily
- Cross-Protocol: Remove the necessity for dealing with specific protocols (CGI/FastCGI/...)
- Cross-Database-API: being able to access wide range of databases without dealing with DB-specific APIs and switch between them in the config files instead of changing the code itself.
- Parallelization Access: being able to compute things in parallel easily
- Implement Common Patterns easily

## What can you should be able to do with this library?

The project is still in development, but this framework should be able to
help you with these kinda projects someday:

- Develop a static/dynamic website easily in C++
- Using other people's already written websites/modules/components in your app
- Writing long-running web apps and store temporary data in variables instead of database (not all languages can do that)

## Why C++?

The most question that I get while talking to other developers is why C++?
Well there are multiple answers:

- C++ does have the potential specially the newer versions of C++
- Having full power of C++ and your OS at your disposal
- WebAssembly is getting stronger, you will be able to write both back-end and front-end in the same language with C++ (even though you already can if you choose JavaScript for example, but here's another option)
- Has the potential to be faster than other languages
- C++ can be easy if the library is easy to use.
- Using modern C++ is fun
- Compile-Time computation is something that you either can't achieve or it's going to be difficult to achieve in other languages
- Remove the necessity to learn other languages if you're already familiar with C++
- Using older codes in your project more easily
- Multi-threading access: not all languages provide that access easily specially in a web framework



## Example Codes

**A Simple CGI application**
```c++
auto page_one() {
    return "Page 1";
}


struct web_app {
    enable_owner_traits<default_traits> et; // holds the allocator, logger, ...
    dynamic_router router{et};              // Helps with the routing, could be used as an app too
    
    web_app() {
        // register your routes
        
        router += router / endpath >> []() noexcept {
                           return "main page";
                       };
        
        router += router / "page" / "one" / endpath >> page_one; // free functions
        router += router / "cgi-bin" % "cgi-hello-world" >> [] {
                           return "Hello world";
                       };
        
        // "/about" or "/cgi-bin/cgi-hello-world/about"
        router += (router % "about") || (router / "cgi-bin" / "cgi-hello-world" % "about") >>
                       [](context& ctx) {
                           return ctx.view("about.html");
                       };
    }
    
    auto operator()(auto&& req) {
        return router(req);
    }
};

int main() {
    // CGI Protocol
    webpp::http::cgi<web_app> cgi_application;
    
    // run the app:
    return cgi_application();
}
```

**There are a lot more features, designed to be used in a modular way:**
```c++

// Could be a sub-app of another sub-app that has no
// clue how the server works or what kinda protocol (CGI/FastCGI/Self Served/...) is being used
struct app {
    using namespace webpp;
    using namespace webpp::http;
    
    app() {
        // Tell the view manager where to look for the files
        view_man.view_roots.emplace_back("./public");
        view_man.view_roots.emplace_back("./static");
    }
    
    response index(context& ctx) {
        return "Main Page";
    }
    
    response api(request const& req) {
        json::document doc{req};     // You can choose which JSON lib you want to use in the background
        doc["user"] = "username";
        doc["token"] = "some token";
        return doc;
    }
    
    response about(request const& req) {
        response res{req};
        res.headers = http::status_code::ok;
        res.headers["Content-Type"] = "text/html";
        res.body = "About Page";
        return res;
    }
    
    auto page_one() {
        return view_man.view("pages/page1.mustache"); // We have mustache built-in
    }
    
    auto hello() {
        return view_man.view("pages/hello.html");
    }
    
  private:
    enable_traits_for<views::view_manager<>> view_man;
};

// for demonstration purposes only, we could've done this a lot easier
// The Server calls this class (the operator() of this class) for every request
// BTW, the `router` can totally replace this, you can inherit from it even
struct app_controller {
    using namespace webpp;
    using namespace webpp::http;

  private:
    enable_traits_for<dynamic_router> router;
    app my_app;
    
  public:
    
    app_controller() {
        // register your app
        router.objects.emplace_back(my_app);
        
        // register the routes:
        router += router / endpath >> &app::index;
        router += router / "page" % "one" >> &app::page_one;
        router += router / "api" / "v1" >> &app::api;
        router += router / "cgi-bin" % "cgi-hello-world" >> &app::hello;
        router += (router / "about") || (router / "cgi-bin" / "cgi-hello-world" / "about") >> &app::about;
    }

    // This operator will be called for each request
    HTTPResponse auto operator()(HTTPRequest auto&& req) {
        return router(req);
    }
};

int main() {
    webpp::http::beast<app_controller> server;
    server
      .enable_sync()        // call the app in a thread-safe manner (might be removed in the future)
      .address("127.0.0.1") // listen on localhost
      .port(8080);          // on http port
      
    // Start the server and share your website to the world like there's no tomorrow  
    return server();
}
```

## Somewhat Dependencies

These are the dependencies this project requires for certain parts of the project
to function properly. Some of these libraries haven't been integrated into the project
yet since we're still working on this project.

Some of these dependencies can replace other dependencies; the difference is usually in 
their performance but other factors are in play as well.

- `boost::asio` or `asio` (for FastCGI)
- `boost::beast` _(optional)_ (for benchmarking only)
- `fmt`: as a fallback for `std::format`
- `eve` _(optional|Disabled)_: for better performance with SIMD
- `openssl` (used by asio)
- _(optional)_ for `gzip` support one of these:
  - `zlib` _(optional)_
  - `zlib-ng` _(optional)_
  - `crypto++` _(optional)_
  - `libdeflate` _(optional)_
- `zstd` _(optional)_: for `zstd` compression support
- `brotli` _(optional)_: for `br` compression support

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

### Benchmarks
Benchmarks are done for us developers so we know which tool/class/implementation/library/framework/solution that we choose is the best. Currently we are using Googles' mini-benchmark library for this purpose.

### Documentation
We don't have a documentation/tutorial/guide at this point. You can help us write one. We're just too busy writing the code that we don't have much time writing documentations at this point. But you can be sure from the point that this project becomes production-ready, we'll have documentations.

### Build

```bash

# Download
git clone https://gitlab.com/webpp/webpp.git --depth=1
cd webpp/

# Configure CMake (default uses ninja and creates 'build' directory)
cmake --preset=default

# Build Examples (See /examples directory for the list of them)
cmake --build --preset=cgi-application
cmake --build --preset=beast-view
cmake --build --preset=cgi-hello-world
cmake --build --preset=beast-json

# Build Tests (See /tests directory for the list of tests, each file is a test)
cmake --build --preset=test-type-traits
cmake --build --preset=test-dynamic-router
cmake --build --preset=test-context
cmake --build --preset=test-cookies
cmake --build --preset=test-response

# Run the tests individually
./build/test-response
./build/test-type-traits

# Run all tests (Need to build first)
ctest --preset=tests

# install
cmake --install ./build --prefix=/usr
```
