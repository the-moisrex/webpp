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

There can be more than one `interface` inside an executable. For example you can accept request from a FastCGI on port 2020, and at the same time you can accept HTTP request on port 80; but in order to implement this, you need the `Server`.

## Applications
`Application`s include:

 - A `Router` and its `Route`s, `Context` and its extensions
 - Any number of `Application Extensions`.

### Master Application
There can be multiple `Application`s. For example, you might have written a whole website before in a different project, now you can use that whole application just by including it in the project an with the help of the `Master Application`, you can combine two or more applications into one single application and use it normally.

### Application Extensions
### Application's Router
#### Route
#### Context
##### Context Extensions
