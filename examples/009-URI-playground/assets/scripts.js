console.log(
  "%cURI Playground by M. Bahoosh",
  "color: purple; display: inline-block; background: white; padding: 0.5em 1em; margin: 1em 0; border-radius: 5px; border: 1px purple solid; font-family: sans-serif;");

const browserURIParser = {
    name: "browser",
    result: {
        scheme: null,
        username: null,
        password: null,
        host: null,
        port: null,
        path: null,
        queries: null,
        fragment: null
    },
    parse(uriText) {
        try {
            const uri            = new URL(uriText);
            this.result.scheme   = uri.protocol;
            this.result.username = uri.username;
            this.result.password = uri.password;
            this.result.host     = uri.host;
            this.result.port     = uri.port;
            this.result.path     = uri.pathname;
            this.result.queries  = uri.search;
            this.result.fragment = uri.hash;
        } catch (err) {
            console.log(err); // todo
        }
    }
};

const webppURIParser = {
    name: "webpp",
    result: {
        scheme: null,
        username: null,
        password: null,
        host: null,
        port: null,
        path: null,
        queries: null,
        fragment: null
    },
    async parse(uriText) {
        // todo
        // find the URI
        let query_url = new URL(document.querySelector("#uri-form").action);
        let queries   = new URLSearchParams;
        queries.set("uri", uriText);
        query_url.search = "?" + queries.toString();

        try {
            const response = await fetch(query_url.toString());
            if (!response.ok) {
                setResult(new Error("Network error"));
                return;
            }
            console.log(response);
        } catch (err) {
            setResult(err);
        }
    }
}

const activeParser = {
    parsers: [webppURIParser, browserURIParser],
    activeParserValue: webppURIParser,
    get active() {
        return this.activeParserValue;
    },
    set active(parser) {
        if (this.parsers.filter(item => parser?.name === item.name).length) {
            this.activeParserValue = parser;
        } else {
            const found = this.parsers.filter(item => item.name === parser);
            if (found.length === 1) {
                this.activeParserValue = found[0];
            } else {
                this.activeParserValue = this.parsers[0];
            }
        }
        localStorage.setItem("active-parser", this.activeParserValue.name);

        const buttons = document.querySelector(".settings .button-group");
        buttons.querySelector(".active").classList.remove("active");
        buttons.querySelector(`[data-uri-parser=${this.activeParserValue.name}]`).classList.add("active");
        this.run();
    },

    uriTextValue: "",
    get uriText() {
        return this.uriTextValue;
    },
    set uriText(val) {
        this.uriTextValue = val;

        // change the address
        let cur_url    = new URL(window.location);
        let cur_params = cur_url.searchParams;
        cur_params.set("uri", this.uriTextValue);
        cur_url.search = cur_params.toString();
        window.history.replaceState({}, null, cur_url.toString());

        this.run();
    },
    run() {
        this.active.parse(this.uriText);

        const segments = document.querySelector("#uri-segments");
        Object.keys(this.active.result).forEach(seg => {
            const segElement = segments.querySelector(`[data-uri-segment=${seg}]`);
            if (!seg) {
                segElement.classList.add("empty");
            } else {
                segElement.classList.remove("empty");
                segElement.querySelector(".content").innerText = seg?.text || "";
            }
        });
    },
    switch (ev) {
        this.active = ev.target.getAttribute("data-uri-parser");
    },
    setupEvents() {
        const buttons = document.querySelectorAll(".settings .button-group button");
        buttons.forEach(button => button.addEventListener("click", this.switch.bind(this)));
    }
};


function debounce(func, wait = 100, immediate = undefined) {
    let timeout;
    return function() {
        let context = this, args = arguments;
        let callNow = immediate && !timeout;
        clearTimeout(timeout);
        timeout = setTimeout(function() {
            timeout = null;
            if (!immediate) {
                func.apply(context, args);
            }
        }, wait);
        if (callNow) {
            func.apply(context, args);
        }
    }
}

async function updatePage(event) {
    event.preventDefault();
    const form       = document.querySelector("#uri-form");
    const newUriText = form.querySelector("input[type=text]").value;
    if (newUriText === activeParser.uriText) {
        return;
    }
    activeParser.uriText = newUriText;
}

document.addEventListener("DOMContentLoaded", () => {
    const prefersDarkScheme = window.matchMedia("(prefers-color-scheme: dark)").matches;
    document.body.classList[prefersDarkScheme ? "add" : "remove"]("dark");
    document.body.classList[!prefersDarkScheme ? "add" : "remove"]("light");
    document.querySelector("#dark-mode-switch").addEventListener("click", () => {
        const to_light = document.body.classList.contains("dark");
        document.body.classList[to_light ? "remove" : "add"]("dark");
        document.body.classList[!to_light ? "remove" : "add"]("light");
    });

    const form     = document.querySelector("form");
    const uriInput = form.querySelector("form input[name=uri]");
    const onUpdate = debounce(updatePage, 200);

    let cur_url    = new URL(window.location);
    let cur_params = cur_url.searchParams;
    uriInput.value = cur_params.get("uri");


    form.addEventListener("submit", onUpdate);
    uriInput.addEventListener("change", onUpdate);
    uriInput.addEventListener("keypress", onUpdate);
    uriInput.addEventListener("paste", onUpdate);
    uriInput.addEventListener("input", onUpdate);

    activeParser.setupEvents();

    const activeParserStored = localStorage.getItem("active-parser");
    if (activeParserStored) {
        activeParser.active = activeParserStored;
    }
});
