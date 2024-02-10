# URL / URI
In this directory you can find URI related algorithms and structures.

### Usage

```c++
#include <webpp/uri/uri.hpp>

int test() {
    using webpp::uri::uri;
    
    uri local = "http://localhost/page/one";
    
    cout << local.get_scheme() << endl;    // http
    cout << local.get_hostname() << endl;  // localhost
    cout << local.get_path() << endl;      // /page/one
    
    local.set_hostname("127.1"); // yes, it's valid, because WHATWG says so
    
    cout << local.get_href() << endl;      // http://127.0.0.1/page/one
}
```

There are a lot more features included in this directly.

## FAQ

### URI vs. URL

We're using the term URI instead of URL in this project even though we're well aware that WHATWG is using the term URL.
Consider URL and URI the same term even though URI has a broader use cases than URL, that's why we chose URI instead of
using URL like WHATWG.

