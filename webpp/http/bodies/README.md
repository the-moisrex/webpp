# Custom Bodies

Custom bodies are a way to convert the request/response bodies into custom
and user specified types.

We're using `tag_invoke` technique that's proposed in [P1895](https://wg21.link/P1895) for standardisation.

```c++

struct user {
    std::string name;
    std::string email;

    friend void tag_invoke(serialize_body_tag, user const& usr, HTTPBody auto& body) {
        json::document doc{body.get_traits()};
        doc["name"] = usr.name;
        doc["email"] = usr.email;
        body = doc; // the library already knows how to serialize json
    }

    template <typename T>
      requires (stl::same_as<T, user>)
    friend user tag_invoke(deserialize_body_tag, HTTPBody auto& body) {
        json::document doc = body.as();
        user usr;
        usr.name = doc["name"];
        usr.email = doc["email"];
        return usr;
    }
};

auto page(Context auto&& ctx) {
    std::string req_body_as_string = ctx.request.as();
    user        req_body_as_user   = ctx.request.as();
}
```


## (De)Serializer functions

### `serialize_body`

This Customization Point Object (CPO) should convert the custom body into the request/response body itself.

```c++
void tag_invoke(
        stl::tag_t<serialize_body>,     // same as serialize_body_tag
        custom_body_type,               // Your custom body type which you want to serialize
        HTTPBody auto& body             // A reference to the body that you want to serialize into
        );
```



### `deserialize_body`

This CPO should convert the request/response bodies into the custom body and return it.


```c++
template <typename CustomBodyType>
  requires (stl::same_as<CustomBodyType, ...>)
CustomBodyType tag_invoke(
                stl::tag_t<deserialize_body>,       // deserialize_body_tag
                stl::type_identity<CustomBodyType>, // this is required to make the tag_invoke technique work
                HTTPBody auto const& body);         // the body reference that (request or response body)
```



### `serialize_response_body`

The same as `serialize_body` but gets a response instead of the body.

If this CPO exists, then the `serialize_body` won't be called; so
the presence of this CPO has precedence over `serialize_body`.

```c++
void tag_invoke(
        stl::tag_t<serialize_response_body>,    // serialize_response_body_tag
        custom_body_type val,                   // Your custom body that you want to serialize
        HTTPResponse auto& res                  // A reference to the response that you want to serialize into
        );
```

The usage of this can be to add required headers to the `response.headers`
and then call `serialize_body` which serializes the body.

For example, `json` serializer can add `Content-Type: application/json` to
the headers and then serialize the json itself to the body.



### `deserialize_response_body`

The same as `deserialize_body` but gets a response instead of the body.

If this CPO exists, then the `deserialize_body` won't be called; so 
the presence of this CPO has precedence over `deserialize_body`.

```c++
template <typename CustomBodyType>
  requires (stl::same_as<CustomBodyType, ...>)
CustomBodyType tag_invoke(
        stl::tag_t<deserialize_response_body>,      // deserialize_response_body_tag
        stl::type_identity<CustomBodyType>,         // Your custom type that you want to create from the body
        HTTPResponse auto const& res);              // The response

template <typename CustomBodyType>
  requires (stl::same_as<CustomBodyType, ...>)
CustomBodyType tag_invoke(
        stl::tag_t<deserialize_response_body>,      // deserialize_response_body_tag
        stl::type_identity<CustomBodyType>,         // Your custom body type
        HTTPBody auto const& body);                 // The actual response body (not the response)
```

if you specify the `HTTPResponse`, then you'll get a response, if you specify
that you want a body, you'll get a body.

### `deserialize_request_body`

The same as `deserialize_body` but gets a request instead of the body.

If this function exists, then the `deserialize_body` won't be called; so
the presence of this function has precedence over `deserialize_body`.

```c++
template <typename CustomBodyType>
  requires (stl::same_as<CustomBodyType, ...>)
CustomBodyType tag_invoke(
        stl::tag_t<deserialize_request_body>,       // deserialize_request_body_tag
        stl::type_identity<CustomBodyType>,         // Your custom type
        HTTPRequest auto const& req);               // Request that you want to deserialize into CustomBodyType

template <typename CustomBodyType>
  requires (stl::same_as<CustomBodyType, ...>)
CustomBodyType tag_invoke(
        stl::tag_t<deserialize_request_body>,       // deserialize_request_body_tag
        stl::type_identity<CustomBodyType>,         // Your custom body type that you want to deserialize into
        HTTPBody auto const& body);                 // The request body (not the request itself)
```

if you specify the `HTTPResponse`, then you'll get a response, if you specify
that you want a body, you'll get a body.


### `serialize_request_body`

Currently, request modification is not supported; open up an issue to talk more about this.
