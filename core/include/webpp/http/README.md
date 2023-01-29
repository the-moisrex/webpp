# HTTP(s)

### Headers

Headers a container of fields.

Fields:

- `field.name`
- `field.value`
- They can be a **string view** or a **string** (we the actual types from the _traits system_)

We plan to have plenty of pre-defined types that represent each standard header types.
Read more about **Pre-Defined Headers** in the [headers directory](./headers/README.md).

### Bodies

Bodies have the concept of communicators. Body communicators are the way, you (the user of the library)
communicate with the library itself and how you would pass your body types to us and how we can pass our
request's body's data to you.

Body Communicator Primitives:

- **Text Based Body Communicator**: for string-like types
- **Stream Based Body Communicator**: for streams
- **C-Stream Based Body Communicator**: for streams with c-like functions

Every other body types should use the functions of these communicator primitives
to communicate with the library's response and request bodies.


You can write your own function to use these primitives to create your own custom bodies.
Read more about **Custom Bodies** in the [bodies directory](./bodies/README.md).

### Request

Requests have:

- `request.headers`
- `request.body`

### Response

Responses have:

- `response.headers`
- `response.body`


### Protocols

Protocols are the way the HTTP is transported to the user of the app.

- **CGI**: The server (another software) runs our executable for every request.
- **FastCGI**: Special custom protocol to send and receive from the server.
- **Beast**: Using `boost::beast` library as a server


---------------------

## Dynamic Routing System

Dynamic routing system is a runtime initialized dynamic router and lets you
use it without caring much about its type.

### Request View

