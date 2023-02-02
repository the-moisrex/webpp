# Custom Bodies

Custom bodies are a way to convert the request/response bodies into custom
and user specified types.

```c++

struct user {
    std::string name;
    std::string email;
};

void serialize_body(user const& usr, HTTPBody auto& body) {
    json::document doc{body.get_traits()};
    doc["name"] = usr.name;
    doc["email"] = usr.email;
    body = doc; // the library already knows how to serialize json
}

template <typename T>
requires (stl::same_as<T, user>)
user deserialize_body(HTTPBody auto& body) {
    json::document doc = body.as();
    user usr;
    usr.name = doc["name"];
    usr.email = doc["email"];
    return usr;
}

auto page(Context auto&& ctx) {
    std::string req_body_as_string = ctx.request.as();
    user        req_body_as_user   = ctx.request.as();
}
```


## (De)Serializer functions

### `serialize_body`

This function should convert the custom body into the request/response body itself.

```c++
void serialize_body(
        custom_body_type,   // The custom body
        HTTPBody auto& body // A reference to the body itself
        );
```



### `deserialize_body`

This function should convert the request/response bodies into the custom body and return it.


```c++
template <typename CustomBodyType>
  requires (stl::same_as<CustomBodyType, ...>)
CustomBodyType deserialize_body(HTTPBody auto const& body);
```
The custom body's type MUST be constrained through C++20 concepts or other methods like `enable_if`s.




### `serialize_response_body`

The same as `serialize_body` but gets a response instead of the body.

If this function exists, then the `serialize_body` won't be called; so
the presence of this function has precedence over `serialize_body`.

```c++
void serialize_response_body(
        custom_body_type val,    // The custom body
        HTTPResponse auto& res   // A reference to the response
        );
```

The usage of this can be to add required headers to the `response.headers`
and then call `serialize_body` which serializes the body.

For example, `json` serializer can add `Content-Type: application/json` to
the headers and then serialize the json itself to the body.



### `deserialize_response_body`

The same as `deserialize_body` but gets a response instead of the body.

If this function exists, then the `deserialize_body` won't be called; so 
the presence of this function has precedence over `deserialize_body`.

```c++
template <typename CustomBodyType>
  requires (stl::same_as<CustomBodyType, ...>)
CustomBodyType deserialize_response_body(HTTPResponse auto const& res);

template <typename CustomBodyType>
  requires (stl::same_as<CustomBodyType, ...>)
CustomBodyType deserialize_response_body(HTTPBody auto const& body);
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
CustomBodyType deserialize_request_body(HTTPResponse auto const& res);

template <typename CustomBodyType>
  requires (stl::same_as<CustomBodyType, ...>)
CustomBodyType deserialize_request_body(HTTPBody auto const& body);
```

if you specify the `HTTPResponse`, then you'll get a response, if you specify
that you want a body, you'll get a body.


### `serialize_request_body`

Currently, request modification is not supported; open up an issue to talk more about this.
