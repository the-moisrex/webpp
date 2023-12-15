# Core specifications

__Attention__: some of the things that is written here are outdated and obsolete.

## Examples

```c++
struct application {
    using namespace webpp;
    
    view_manager man;

    auto page(Context auto& ctx) const noexcept {
        int page_num = ctx.request.param("page_num");
        user _user = ctx.request.param("user_id");
        return man.view("page.mustache", {
            {"user", _user},
            {"page_num", page_num}
        });
    }
    
    HTTPResponse auto operator()(HTTPRequest auto&& req) noexcept {
        static static_router router {
            get and root / number("user_id") / "page" / number("page_num") >> &app::page
        };
        return router(req);
    }
};

int main() {
    webpp::cgi<application> app;
    return app();
}

```

## Protocols
Protocols are usually a way of communication between the high level application layer,
and the low level transmission layers.

The protocols that interests us in this project know what will the other side
understands, so they can __convert the request and responses from and to their corresponding
data structures known to the framework__.

But those are not what we are calling "protocols" in this project. These are:

- __CGI__: A _CGI_ client. (doesn't use _servers_)
- __FastCGI__: A _FastCGI_ client. (uses _servers_)
- __Self Hosted__: HTTP protocol (utilizing _servers_)
- __Beast__: a wrapper for `boost::beast` (has its own `boost::asio` _server_)

The _Protocol_:
- Creates a `request` from user requests;
- Passes the `request` to the `Applicaton`;
- `Application` returns a `response`;
- `Protocol` converts the `response` to the known format by the client;
- `Protocol` sends that `response` to the outside world.

The `Protocol` instantiates the `Application` so it does have access to it.

There can be more than one _protocol_ inside an executable.
For example, you can accept request from a FastCGI on port 2020,
and at the same time you can accept HTTP request on port 80.

### Applications
`Application`s are the struct that the user will create. It  _might_ include:

 - A `Router` and its `Route`s, `Context` and its extensions
 - templated operator()

Creating the Router and its extensions are where the user will be coding
the actual application.

#### Application Extensions
_Application extensions_ are classes that an _application_ inherits from or uses.
These classes are capable of adding arbitrary features to the _application_.
These extensions may need to access _router_ or _initial context type_ or
any other related thing. You need to see their documentation for those.

Examples of application extensions:

 - Database Access
 - Models in the MVC design pattern
 - Users' chat information in a chat-room website
 - Stats

#### Application's Router
The router is the place where all the routes are stored.
Each request will go through the router, and router, with 
the help of the routes it has, will produce a _response_ and 
returns it back to the caller.
This is not a mandatory thing to use, but you probably need it.
It will be used inside an application. A sub-application can have its own
version router. We recommend you constant router version because it
will process creation stuff at compile-time and won't cost you
anything at runtime.

There's also a _dynamic router_ which will let the user 
to dynamically change the routes at runtime, but it will cost you
some runtime processing which is not that much, and you can
ignore it for the most part.

 
## Traits System
Explained in [traits/readme](./traits/README.md) file.

## Extension System
Explained in [extensions/readme](./extensions/README.md) file.


## Allocator system
Explained in [memory/readme](./memory/README.md) file.
