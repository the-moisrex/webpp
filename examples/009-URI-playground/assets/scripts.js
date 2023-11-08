console.log(
  "%cURI Playground by M. Bahoosh",
  "color: purple; display: inline-block; background: white; padding: 0.5em 1em; margin: 1em 0; border-radius: 5px; border: 1px purple solid; font-family: sans-serif;");

function setError(err) {
    const generalResponse = document.querySelector("#general-response");
    const generalError    = document.querySelector("#general-error");
    const resultsElement  = document.querySelector(".results");
    resultsElement.classList.add("hidden");
    generalResponse.classList.add("hidden");
    generalError.classList.remove("hidden");
    generalError.innerText = err;
    console.error(err);
}

function setPerfResult(diff, networkTime = "0ms") {
    const generalResponse = document.querySelector("#general-response");
    const generalError    = document.querySelector("#general-error");
    generalResponse.classList.remove("hidden");
    generalError.classList.add("hidden");
    generalResponse.querySelector("#parsing-time").innerText = diff;
    generalResponse.querySelector("#network-time").innerText = networkTime;
}

function setResult(result) {
    const resultsElement = document.querySelector(".results");
    resultsElement.classList.remove("hidden");
    Object.keys(result).forEach(seg => {
        const segElement = resultsElement.querySelector(`[data-uri-segment=${seg}]`);
        if (!result[seg]) {
            segElement.classList.add("hidden");
            return;
        }
        segElement.classList.remove("hidden");
        segElement.querySelector(".content").innerText = result[seg];
        segElement.querySelector(".desc").innerText    = seg;
    });
}

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
            const start = Date.now()
            const uri   = new URL(uriText);
            const fin   = Date.now();

            this.result.scheme   = uri.protocol;
            this.result.username = uri.username;
            this.result.password = uri.password;
            this.result.host     = uri.hostname;
            this.result.port     = uri.port;
            this.result.path     = uri.pathname;
            this.result.queries  = uri.search;
            this.result.fragment = uri.hash;

            setPerfResult(`${fin - start}ms`);
            setResult(this.result);
        } catch (err) {
            setError(err);
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
        const start     = Date.now()
        let query_url = new URL(document.querySelector("#uri-form").action);
        let   queries   = new URLSearchParams;
        queries.set("uri", uriText);
        query_url.search = "?" + queries.toString();

        try {
            let response = await fetch(query_url.toString());
            if (!response.ok) {
                setError(new Error("Network error"));
                return;
            }
            response             = response.json();
            this.result.scheme   = response["scheme"];
            this.result.username = response["username"];
            this.result.password = response["password"];
            this.result.host     = response["host"];
            this.result.port     = response["port"];
            this.result.path     = response["path"];
            this.result.queries  = response["queries"];
            this.result.fragment = response["fragment"];
            const fin            = Date.now();
            setPerfResult(`${response["parsing-time"]}ns`, `${fin - start}ms`);
            setResult(this.result);
        } catch (err) {
            setError(err);
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

        const uriInput = document.querySelector("#uri-form input[name=uri]");
        uriInput.value = this.uriTextValue;

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

    const form     = document.querySelector("#uri-form");
    const uriInput = form.querySelector("input[name=uri]");
    const onUpdate = debounce(updatePage, 200);

    let cur_url    = new URL(window.location);
    let cur_params = cur_url.searchParams;


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

    activeParser.uriText = cur_params.get("uri");
});
