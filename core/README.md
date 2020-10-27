# Core specifications

## Examples

```c++
struct app {
    using namespace webpp;

    router _router {
        path() / number("user_id") / "page" / number("page_num") >> app::page
    };

    auto page(Context auto& ctx) const noexcept {
        auto page_num = ctx.path.template get<int>("page_num");
        auto _user = ctx.path.template get<user>("user_id");
        return ctx.response.file("page.html", _user, page_num);
    }
    
    Response auto operator()(Request auto&& req) noexcept {
        return _router(req);
    }
};
```

## Protocols
Protocols are usually a way of communication between the high level application layer,
and the low level transmission layers.

Protocols:

- Can depend on another protocols.

Examples of protocols:

- **HTTP**
- **HTTPS**
- **WebRTC**
- **FTP**

## HTTP Protocol
The rest of this article is about HTTP protocol.

### Interfaces
Interfaces are part of the HTTP protocol and they are the part 
that have contact with the outside world.

Each interface implements an specific communication protocol. Including:

 - CGI
 - FastCGI
 - Self Hosted

The `interface`:
 - Implements and creates a `request`;
 - Passes it to the `Applicaton` as a reference;
 - `Application` returns a `response`;
 - `Protocol` sends that `response` to the outside world.

The `interface` instantiates the `Application`.

There can be more than one `interface` inside an executable.
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
     - [ ] On-The-Fly Re-Prioritization
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
Context extensions are added features to the contexts. These features
will be used by the routes in order to have a easier way of doing
things. For example you could use a context extension to parse
data in one router, and pass those data to another router, and
that router will use those data knowing that it's already been parsed.

**Extension requirements:**
 - [ ] Having a default constructor
 - [ ] Having a copy constructor for extensions
 - [ ] Having a move constructor for extensions

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


**Internal Extension Handling:**

 We can customize every single route to check if the extension is
 present in the returned context and then act accordingly, but that's
 not scalable; so in order to do this, we're gonna call "something"
 on each extension in these times (if the extension has its method):
   - **pre_subroute**: Before every call to a sub-route
   - **post_subroute**: After every call to a sub-route
   - **pre_entryroute**: Before every call to an entry route
   - **post_entryroute**: After every call to an entry route
   - **pre_firstroute**: Before calling the first entry route if possible
   - **post_lastroute**: When we get the final result and we're
       about to send it to the user.


**todo**: Extension dependency:
 We need a way of saying that an extension needs another extension to work.


**todo**: Runtime modification of Initial context type:
We need a way to achieve this; we need a way to specify the initial
context type that will be used for every single time.



## traits
_Traits_ are special types that contain other types and static methods
that will help the users of the library to customize the library through
using other types for specific purposes. Fox example to use a different
`Allocator` STL uses or a different _character type_ in the strings or
even configure UTF-8.

------------------------------------------------------------------





## Extension system

Definitions:

- __Extension__: some type that adds features to the extensie, e.g.: cookie
- __Extensie__: the type that extensions will add features to; e.g.: request, response, context

There are 2 types of extensions:

- Mother extensions
- Child extensions

**Mother Extensions** are the type of extensions that the extensies will extend from.

__Child Extensions__ are the type of extensions that will inherit extensies;
they will replace the original extension type, but they have access to
the extensie.

### Extension syntax
The extensions are just a type that has a __templated using statement__.
The difference between extension types are just which extension they have
access to.


__Mother Extension__:

Can only have access to _traits type_.
```c++
struct mother_extension {
    
    template <Traits TraitsType>
    using type {
        // has to have at least a default constructor
        // features ...
    };
};

struct portable_extension_packs {
    using context_extensions = extension_pack<mother_extension>;
};
```

__Attention__: mother extensions are not able to require other child extensions as dependencies.

__Child Extension__:

Child extensions have access to the _traits type_ and also the _extensie_.
Using child extensions are a big harder because they need to have access to their
parents' fields and methods.

```c++
template <Traits TraitsType, ResponseHeaders C>
struct cookies : public virtual C {
  // has to have a default constructor
  auto get_cookies() { ... }
};

struct cookies_child_extensions {

  template <Traits TraitsType, ResponseHeaders C>
  using type = cookies<TraitsType, C>;
};

struct router_level_extensions {
    using response_header_extensions = extension_pack<cookies_child_extension>;
};
```

or the shortened version:

```c++
template <Traits TraitsType = std_traits, ResponseHeader C = default_response_header>
struct cookies : public virtual C {
    
    template <Traits TT, Context CC>
    using type = cookies<TT, CC>;
};

struct router_level_extension {
    using header_extensions = extension_pack<cookies<>>;
};
```

### Unified extension type
With a __unified extension type__, you can have an extension that can
introduce other extensions or require other extensions to be presented.

A unified extension will be passed to routers; so unified extensions are
only able to add extensions to the types that are being created by router
or any type down its chain.

Features of a unified extension type:

- `router_extensions`: a pack of router extensions
- `context_extensions`: a pack of context extensions
- `response_extensions`: a pack of response extensions
- `response_header_extensions`: a pack of response header extensions
- `response_header_field_extensions`: a pack of response header field extensions
- `response_body_extensions`: a pack of response body extensions


### Extension usage
Here's an example of how you can use extensions inside your application layer:

```c++
struct app {
  using extensions = extension_pack<cookies, sqlite>;
  
  const_router<extensions> router {
      path() / "home" >> app::home
  };

  app () {
    router.sqlite.username = "admin";
    router.sqlite.password = "password";
    router.sqlite.file = "file.sqlite";
    router.sqlite.connect();
  }

  auto home(auto Context& ctx) {
    return "Home Page";
  }

  auto Response operator()(auto Context& ctx) {
    return router(ctx);
  }

};
```

### Extensies

Extensies are these types:

- const_router / dynamic_router
- context
- response
- response_headers
- response_header_field
- response_body

There are two level for each extensies:

- Mid-Level extensie
- Final extensie


__Mid-Level__ extensies are extensies which:

- can extend from _mother extensions_
- _child extensions_ can extend from it
- are not a __final__
- can specify a _final extensie_

__Final Extensies__ are the extensies that:

- cannot be extended from
- inherits from all the _child extensions_ and the _mid-level extensie_


### Extension hierarchy 
The hierarchy of the extensions is like this:

1. The _Mother Extensions_ (optional)
2. The _Mid-Level Extensie_ (required)
3. The _Child Extensions_ (optional)
4. The _Final Extensie_ (optional)

### Extensie descriptor type (internal usage)
For each extensie, it is required to write a type with these types in them so the extension pack
can detect which extensions should be used where.

- `has_related_extension_pack` (required)
- `related_extension_pack_type` (required)
- `mid_level_extensie_type` (required)
- `final_extensie_type` (required) (could be optional, but that's for later)

### Extensie descriptor carry dependencies


### Extension hierarchy assembly (internal usage)
To assemble the extensions and the extensies into one single type we have to:

1. [ ] Extract the type _extension pack_ from the router level extensions
2. [X] Extract _mother extensions_ from the extension pack
3. [X] Filter the _mother extensions_ that have extensions for this extensie
4. [X] Extract the extension packs of the extensie, from the extracted _mother extensions_
5. [X] Merge the extracted extension packs into one extension pack
6. [X] Apply the _mother extensions_ to the _mid-level extensie_
7. [X] Extract the _child extensions_ from the extension pack
8. [X] Filter the _child extensions_ that have extensions for this extensie
9. [X] Extract the extension packs of the extensie, from the extracted _child extensions_
10. [X] Merge the extracted extension packs into one extension pack
11. [X] Apply the _mid-level extensie_ to all the _child extension_
12. [X] Extract the _final extensie_ from the _extensie descriptor_
13. [X] Apply _child extension_ to the _final extensie_

