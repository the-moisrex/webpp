# View Markup Language (VML)
This is a custom domain specific language that'll be used
to generate HTML/CSS/JS/WASM/Media outputs and God willing
someday will grow to output other stacks as well.

## Concepts
There are 3 layers to this:

- [ ] Input DSL (Domain Specific Language)
- [ ] Back-end AST
- [ ] Output Generator

The input DSL is what you write.

The output generator is what generates the output based on the AST.

The Back-End AST is what the Input DSL gets converted to (it's the middle man whom everyone knows).


## Language Features
I hope this language will have these features at least:

- [ ] Modular
- [ ] Component based
- [ ] Customizable components from parent components
- [ ] Customizable components from child component
- [ ] Custom attributes
- [ ] Multiple output formats

## Implementation Features

- [ ] Performant
- [ ] Multithreaded when necessary: for example the task of downloading a remote file or processing a media can be multithreaded to save time.
- [ ] Constexpr friendly

## Output types
There can be infinite number of outputs; but we'll start
with some simple ideas:

- [ ] Self contained HTML file.
- [ ] 3 HTML/CSS/JS files
- [ ] Qt Quick
- [ ] Android XML
- [ ] ReactJS
- [ ] VueJS
- [ ] GTK with C


## DSL Examples
````
version 1.0

import Page, Header, Footer
import html


root {
  Header{}
  for page in pages:
    Page {
      name: page.name
    }
  Footer {
    div.page > div#footer-center {
      // what does it mean for a class or an ID in other
      // output generators?
      text: html("this is <b>bold</b>")
    }
  }
}
````

````
<Page>
  
</Page>
````
