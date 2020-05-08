# Core specifications

## Server
Server is an `Interface Abstracter` meaning it can hold:

 - Two or more `Interface`s .
 - One `Application` or `Master Application`

## Interfaces
Interfaces are the part that have contact with the outside world.

Each interface implements an specific protocol. Including:

 - CGI
 - FastCGI
 - Self Hosted

The `interface`:
 - Implements and creates a `request`;
 - Passes it to the `Applicaton` as a reference;
 - `Application` returns a `response`;
 - `Interface` sends that `response` to the outside world.

The `interface` instantiates the `Application`.

There can be more than one `interface` inside an executable.
For example you can accept request from a FastCGI on port 2020,
and at the same time you can accept HTTP request on port 80;
but in order to implement this, you need the `Server`.

## Applications
`Application`s are the struct that the user will create. it includes:

 - A `Router` and its `Route`s, `Context` and its extensions
 - Any number of `Application Extensions`.
 - templated operator()

Creating the Router and its extensions are where the user will be coding
the actual application.

### Master Application
There can be multiple `Application`s. For example, you might have 
written a whole website before in a different project, now you can use
that whole application just by including it in the project an with the
help of the `Master Application`, you can combine two or more applications
into one single application and use it normally.

### Application Extensions
_Application extensions_ are classes that an _application_ inherits from.
These classes are capable of adding arbitrary features to the _application_
so it'll be available to the _routes_ through the _contexts_.

Examples of application extensions:

 - Database Access
 - Models in the MVC design pattern
 - Users' chat information in a chat-room website
 - Stats

### Application's Router
The router is the place where all the routes are stored. 
Each request will go through the router, and router, with 
the help of the routes it has, will produce a _response_ and 
returns it back to the caller.

There's also a _dynamic router_ which will let the user 
to dynamically change the routes at runtime.

#### Route
Types or routes:

 - based on position:
   - Entry routes
   - Sub routes
 - based on returned type:
   - Condition route
   - Response route
   - Migrator route

###### Definitions:

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
          - Interface
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

###### Features we need:

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

###### A bit hard to implement places:

   - Prioritization of the entry routes:
     It's easy to do so actually but it'll be a problem because I don't
     want to use dynamically allocated containers in the implementation
     of this. Adding indirection will cause the routing system to be
     processed at run-time. That's not something I'm willing to do yet.


###### Usage examples:
   - `.on(get and "/about"_path >> about_page)`
   - `( get and "/home"_path ) or ( post && "/home/{page}"_tpath ) >> ...`
   - `opath() /"home" / integer("page") /`
   - `empty / action_list("action", {"open", "delete"}) >> []{...}`
   - `(...) >> &non_templated_class_with_templated_callable_operator`
   - `get and "/profile/"_tpath and set_by_class_constructor`
 
#### Context
###### Definitions:
 - **Context**: An object of arbitrary type that will contain everything
                that routes will need including:
      - some types:
        - Traits
        - Interface
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


###### Features we need:
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


###### Public fields:
 - [ ] **app**: a reference to the application instance
 - [X] **priority**: to check/change this route chain
 - [X] **request**: a const reference to the the request object
 - [X] **response**: a non-const reference to the response object

###### public methods:
 - auto clone<extensions...>()
     get a clone of itself with different type
     designed to add extensions




##### Context Extensions

###### Extension requirements:
 - [ ] Having a default constructor

###### Extension collision:
 It is possible to try to add an extension to the context and get
 compile time errors saying that it's a collision or an ambiguous call
 to some method, to solve this, you can use "Extension As Field"
 features, which means you can clone a context like this:

```c++
return context.clone<as_field<map<traits, string, string>>>();
```

 It's also possible to simplify this line of code with
 "Extension aware context" struct.


###### Internal Extension Handling:
 We can customize every single route to check if the extension is
 present in the returned context and then act accordingly, but that's
 not scalable; so in order to do this, we're gonna call "something"
 on each extension in these times (if the extension has its method):
   - **pre_subroute**: Before every call to a sub-route
   - **post_subroute**: After every call to a sub-route
   - **pre_entryroute**: Before every call to an entry route
   - **post_entryroute**: After every call to an entry route
   - **pre_globalroute**: Before calling the global route
   - **post_globalroute**: After calling the global route
   - **post_thisroute**: Call it once; right after this route
   - **pre_termination**: When we get the final result and we're
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

