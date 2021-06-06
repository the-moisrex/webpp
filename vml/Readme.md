# View Markup Language (VML)
This is a custom domain specific language that'll be used
to generate HTML/CSS/JS/WASM/Media outputs and God willing
someday will grow to output other stacks as well.

## Concepts
This language is made of two sub-DSLs (Domain Specific Language):

- Input DSL
- Output DSL

The input DSL is the one that you will make your website with
and the output DSL is the one that actually decides what
the output will look like.

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
