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
        static router _router {
            get and root / number("user_id") / "page" / number("page_num") >> &app::page
        };
        return _router(req);
    }
};

int main() {
    webpp::cgi<application> app;
    return app();
}

```

## Servers
Even though it's not the best name for it, we do call them _servers_ because
they don't know about the protocol that they're transferring (except TCP/UDP/...);
but they know how to __communicate with the operating system__.

Examples used in the [server](./include/webpp/server) directory:

- __asio__: utilizing `boost::asio` or `asio` or `std::net` (when it comes)
- __posix__: communicating directly with the *nix OSes to do the same thing.

## Protocols
Protocols are usually a way of communication between the high level application layer,
and the low level transmission layers.

The protocols that interests us in this project know what will the other side
understands so they can __convert the request and responses from and to their corresponding
data structures known to the framework__.

But those are not what we call protocols in this project. These are:

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
For example you can accept request from a FastCGI on port 2020,
and at the same time you can accept HTTP request on port 80.

### Applications
`Application`s are the struct that the user will create. it  _might_ includes:

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
to dynamically change the routes at runtime but it will cost you
some runtime processing which is not that much and you can
ignore it for the most part.

##### Route
Routes are being used inside a router. With routes, you can specify
which function should be run when a request is given to you.

Types or routes:

 - based on position:
   - Entry routes
   - Sub routes
 - based on returned type:
   - Condition route
   - Response route
   - Migrator route

Definitions:

   - **Route**: Possible ways that are capable of handling user requests
   - **Global Route**: A series of route that will run on every request
       before and/or after the requests based on choice.
   - **Entry Route**: The main routes that will be checked first
   - **Sub Route**: The routes belong to the each Entry Routes
   - **Condition route**: Which can be an entry route or a sub route.
       This kind of route only returns boolean and its job is to check 
       if the specified request can be handled by the sub routes of this
       condition route.
   - **Response route**: A route that will terminate the route checking 
       process and returns a response that will be sent to the client.
   - **Migrator route**: A route that will change the context in a way that 
       will be useful for the other sub routes or even other entry routes.
   - **Route chain**: A route chain is exactly one entry route and undefined
       number of sub routes of that one entry route.
   - **Context**: An object of arbitrary type that will contain everything
       that routes will need including:
        - some types:
          - Traits
          - Protocol
          - Next sub route
        - references to:
          - Request
          - Response
          - Response Route in this route chain
        - Previous entry routes context changes
        - Previous sub routes context changes
        - Original entry routes level context
        - Original sub routes context changes
   - **Context Passing Pattern**:
                   A pattern designed to share arbitrary data down the
                   routing chain.
   - **Context extensions**:
                   A class that extends the original context and will be
                   used in context-switching process to add more
                   features to the context so it can be used in the
                   sub routes down the routing chain.

Features we need:

   - [ ] Extendable entry and sub routes
     - [ ] Use class operator() as a place to add more sub-routes
   - [ ] Having access to the context class in the callables
   - [ ] Having access to the request and the response
   - [ ] Termination of continuation of checking the sub-routes by parents
   - [ ] Termination of continuation of checking the entry-routes by any
         previous routes, or sub-routes.
   - [ ] Context modification
     - [ ] Sub-Route local context modification by any previous sub-routes
     - [ ] Inter-Entry-Route context modification by any previous
           (sub/entry) routes
   - [ ] Entry-Route prioritization
     - [ ] Auto prioritization
     - [ ] Manual prioritization
     - [ ] Hinted prioritization
     - [ ] On-The-Fly (runtime) Re-Prioritization
   - [ ] Dynamic route generation / Dynamic route switching
   - [ ] Context Passing pattern
   - [ ] Context extensions
   - [ ] Deactivated routes

Return types of the operator() and their meaning:
  - Condition Routes: _\[bool\]_
      Returning true means that the request is a match for this route,
      and false means that the request is not a match for this route
  - Response Route:
    _\[response<...>\]_
    _\[using of >> operator\]_
    _\[any type that can be converted into response\]_
      This makes the route to finish checking the routes and start
      sending the response to the user.
      This action can be overwritten using the context capabilities
  - Context Switching route: _\[context<...>\]_
      This context will be passed to the other sub routes chain; this
      will not affect the entry level routes' context.
 
Overloaded operators:
  - `&`   : and also check this sub route              [returns: new route]
  - `&&`  : and also check this sub route              [returns: new route]
  - `|`   : or check this sub route instead            [returns: new route]
  - `||`  : or check this sub route instead            [returns: new route]
  - `^`   : either this route or that sub route (xor)  [returns: new route]
  - `>>`  : check this sub route no matter what        [returns: new route]
  - `()`  : run the route                              [returns: .........]


Usage examples:
   - `.on(get and "/about"_path >> about_page)`
   - `( get and "/home"_path ) or ( post && "/home/{page}"_tpath ) >> ...`
   - `opath() / "home" / integer("page")`
   - `empty / action_list("action", {"open", "delete"}) >> []{...}`
   - `(...) >> &non_templated_class_with_templated_callable_operator`
   - `get and "/profile/"_tpath and set_by_class_constructor`
 

##### Entry-Route input & outputs:
Possible arguments:

- void
- Context
- Request reference

Possible return types:

- Context: entry-route-level context-switching
- Response: send the response to the user, and terminate the routing
- Convertible To Response: same as Response
- void: ignored
- bool: ignored

Definitive return type of operator() of router: Response

##### Sub-Route input & output:
Possible arguments: same as Entry-Level-Routes

Possible return types:

- Context: sub-route-level context-switching
- Response: terminate the routing, send the response to the router to be sent to the user
- Convertible to response: same as response
- void: ignored
- bool: used in the &&, ||, ^ boolean routing operations

Definitive return types of operator() of entry-route:

- Context
- Response
- void
- bool (probably a user mistake if used)

##### Context
Contexts are a way of passing information from one route to another.
The burden of carrying request and responses in the router is upon
the context. Each route may change the context type by returning a
new context.

Definitions:
 - **Context**: An object of arbitrary type that will contain everything
                that routes will need including:
      - some types:
        - Traits
        - Protocol
        - Next sub route
      - references to:
        - Request
        - Response
        - Response Route in this route chain
      - Previous entry routes context changes
      - Previous sub routes context changes
      - Original entry routes level context
      - Original sub routes context changes
 - **Context Passing Pattern**:
                 A pattern designed to share arbitrary data down the
                 routing chain.
 - **Context extensions**:
                 A class that extends the original context and will be
                 used in context-switching process to add more
                 features to the context so it can be used in the
                 sub routes down the routing chain.


Features we need:
 - [X] Having access to the request and the response
 - [ ] Termination of continuation of checking the sub-routes by parents
 - [ ] Termination of continuation of checking the entry-routes by any
       previous routes, or sub-routes.
 - [ ] Context modification
   - [X] Sub-Route local context modification by any previous sub-routes
   - [ ] Inter-Entry-Route context modification by any previous
         (sub/entry) routes
 - [ ] Entry-Route prioritization
   - [ ] Auto prioritization
   - [ ] Manual prioritization
   - [ ] Hinted prioritization
   - [ ] On-The-Fly Re-Prioritization
 - [ ] Dynamic route generation / Dynamic route switching
 - [X] Context Passing pattern
 - [X] Context extensions
   - [ ] Deactivate route extension
   - [X] Map extension


Public fields:
 - [ ] **app**: a reference to the application instance
 - [X] **request**: a const reference to the the request object
 - [X] **response**: a non-const reference to the response object

public methods:
 - auto clone<extensions...>()
     get a clone of itself with different type
     designed to add extensions




###### Context Extensions
_first read more about the extension system in [extensions/readme](./include/webpp/extensions/README.md) file._

Context extensions are added features to the contexts. These features
will be used by the routes in order to have a easier way of doing
things. For example you could use a context extension to parse
data in one router, and pass those data to another router, and
that router will use those data knowing that it's already been parsed.

**Extension collision:**
 It is possible to try to add an extension to the context and get
 compile time errors saying that it's a collision or an ambiguous call
 to some method, to solve this, you can use "Extension As Field"
 features, which means you can clone a context like this:

```c++
return context.clone<as_field<map<traits, string, string>>>();
```

 It's also possible to simplify this line of code with
 "Extension aware context" struct.


## Traits System
Explained in [traits/readme](./include/webpp/traits/README.md) file.

## Extension System
Explained in [extensions/readme](./include/webpp/extensions/README.md) file.


## Allocator system
Explained in [memory/readme](./include/webpp/memory/README.md) file.
