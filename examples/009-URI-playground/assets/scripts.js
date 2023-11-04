console.log(
  "%cURI Playground by M. Bahoosh",
  "color: purple; display: inline-block; background: white; padding: 0.5em 1em; margin: 1em 0; border-radius: 5px; border: 1px purple solid; font-family: sans-serif;");

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

function setResult(res) {
    console.log(res);
}

async function updatePage(event) {
    event.preventDefault();
    const form     = document.querySelector("form");
    const uri_text = form.querySelector("input[type=text]").value;

    // find the URI
    let query_url = new URL(form.action);
    let queries   = new URLSearchParams;
    queries.set("uri", uri_text);
    query_url.search = "?" + queries.toString();

    // change the address
    let cur_url    = new URL(window.location);
    let cur_params = cur_url.searchParams;
    cur_params.set("uri", uri_text);
    cur_url.search = cur_params.toString();
    window.history.replaceState({}, null, cur_url.toString());

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

document.addEventListener("DOMContentLoaded", () => {
    document.querySelector("#dark-mode-switch").addEventListener("click", () => {
        document.body.classList[document.body.classList.contains("dark") ? "remove" : "add"]("dark");
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
});
