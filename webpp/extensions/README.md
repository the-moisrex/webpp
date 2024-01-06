# Extension system

> **Extensions are deprecated (at least for now)!**

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

## Extension syntax
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

## Unified extension type
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


## Extension usage
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

  auto HTTPResponse operator()(auto Context& ctx) {
    return router(ctx);
  }

};
```

## Extensies

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


## Extension hierarchy
The hierarchy of the extensions is like this:

1. The _Mother Extensions_ (optional)
2. The _Mid-Level Extensie_ (required)
3. The _Child Extensions_ (optional)
4. The _Final Extensie_ (optional)

## Extensie descriptor type (internal usage)
For each extensie, it is required to write a type with these types in them so the extension pack
can detect which extensions should be used where.

- `extractor_type` (required)
- `mid_level_extensie_type` (required)
- `final_extensie_type` (required) (could be optional, but that's for later)

## Extensie descriptor carry dependencies


## Extension hierarchy assembly (internal usage)
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


