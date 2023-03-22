# Improved Standard Library (iSTL)

Throughout the development of this library I found that the standard library has
some shortcomings and I could implement them myself so here's some things that
help our project that might be helpful to other projects as well.

Of course there are some features here that probably are not the best implementations
(for example, String and StringView concepts), but they suffice our needs now.

As I'm writing this, I'm totally surprised how much energy and how much time I
invested in this part of the library; probably more than I should.


- **Do not use other parts of the library in this directory**; our goal someday
  might be to separate this directory and I want making this directory its own
  repository as trivial as possible.
- `webpp::istl` is used for new features
- `webpp::stl` is used for `::std` and its polyfill if necessary (no new features here, 
  but only features that are in the STL but not yet implemented everywhere)


