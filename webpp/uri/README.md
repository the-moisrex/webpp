# URL / URI

In this directory you can find URI related algorithms and structures.

### Standards

| Name                        | Standard                                                                                               | Usage                     | Version | Last Updated  |
|-----------------------------|--------------------------------------------------------------------------------------------------------|---------------------------|---------|---------------|
| WHATWG's URL Standard       | [URL Standard](https://url.spec.whatwg.org/commit-snapshots/1c3e6ed5995938fb082e50dcc4fccef1b7413bd4/) | Router, HTML Headers, ... |         | 25 March 2024 |
| Unicode Normalization Forms | Read [unicode / README](../unicode/README.md)                                                          |                           |         |               |
| IDNA                        | Read [idna / README](./idna/README.md)                                                                 |                           |         |               |

**Please**: if there's a new version of these documents, notify us or help us update the codes to the latest versions.

### Usage

```c++
#include <webpp/uri/uri.hpp>

int test() {
    using webpp::uri::uri;
    
    uri const local = "http://localhost/page/one";
    
    cout << local.scheme() << endl;    // http
    cout << local.hostname() << endl;  // localhost
    cout << local.path() << endl;      // /page/one
    
    local.hostname("127.1");           // yes, it's valid, because WHATWG says so
    
    cout << local.href() << endl;      // http://127.0.0.1/page/one
}
```

There are a lot more features included in this directly.

## FAQ

### URI vs. URL

We're using the term URI instead of URL in this project even though we're well aware that WHATWG is using the term URL.
Consider URL and URI the same term even though URI has a broader use cases than URL, that's why we chose URI instead of
using URL like WHATWG.

```
    .~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.
    !                               {URI}                                 :
    !        .-------------------------------------------------------.    :
    !        |                                                       |    :
    !        |                        {URL}                          |    :
    !        |                                                       |    :
    !        `-------------------------------------------------------'    :
    !                                     .--------------------------.    :
    !                                     |                          |    :
    !                                     |          {URN}           |    :
    !                                     |                          |    :
    !                                     `--------------------------'    :
    !                                                                     :
    `~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~'
```
