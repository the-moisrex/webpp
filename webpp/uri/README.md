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

<svg width="765.749" height="547.993" viewBox="0 0 202.605 144.99" xmlns="http://www.w3.org/2000/svg">
    <g transform="translate(-3.889 -2.378)">
        <ellipse style="fill:#000;stroke-width:.264583" cx="105.191" cy="74.873" rx="101.302" ry="72.495"/>
        <ellipse style="fill:#666;stroke-width:.264583" cx="90.643" cy="76.142" rx="74.97" ry="55.585"/>
        <ellipse style="fill:#666;stroke-width:.264583" cx="183.543" cy="75.967" rx="15.729" ry="15.276"/>
        <text xml:space="preserve" style="font-size:6.35px;fill:#f9f9f9;stroke-width:.264583" x="99.428" y="12.534"><tspan style="stroke-width:.264583" x="99.428" y="12.534">URI</tspan></text>
        <text xml:space="preserve" style="font-size:6.35px;fill:#f9f9f9;stroke-width:.264583" x="80.068" y="77.718"><tspan style="stroke-width:.264583" x="80.068" y="77.718">URL</tspan></text>
        <text xml:space="preserve" style="font-size:6.35px;fill:#f9f9f9;stroke-width:.264583" x="178.059" y="78.49"><tspan style="stroke-width:.264583" x="178.059" y="78.49">URN</tspan></text>
    </g>
</svg>
