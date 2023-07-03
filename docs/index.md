## WEB++ (Modern C++ Web Framework)

![Web++ Logo](../assets/logo-bordered.svg)

Develop your backend, and God willing the front-end someday, with C++.

### Why C++?

Even though C++ practically runs the web, and you can see C++'s steps every step of the way that your request goes, from your browser to web servers,
people still think that C++ is not a good language for web development. It may have been true for a long time; but in the modern C++, we think C++ is
one of the best languages that can help you make the best websites and services.

C++ is good for web development because:

* It's as fast as a language can get
* It doesn't let you make stupid mistakes that a static analyzer can catch
* It can be run in unsafe environments since you don't have to share your source codes
* Has access to every corner of the operating system
* Has access to hardware like GPUs
* It can put life into the old codes
* You can run unsafe codes if your job depends on it :)
* Your web app doesn't have to re-run every time a request comes, it can remember everything
* Your website can be used in small IoT devices
* You can share your website's binaries and let the users run them in their own systems without sharing your sources
* It can run other languages; other languages use web servers that are written in C++ after all

I mean, C++ is already running the internet, it's a shame not to take over the web services too :)

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
    webpp::beast<app_controller> server;
    server
      .enable_sync()        // call the app in a thread-safe manner (might be removed in the future)
      .address("127.0.0.1") // listen on localhost
      .port(8080);          // on http port
      
    // Start the server and share your website to the world like there's no tomorrow  
    return server();
}
```
