# TODO

- [ ] Write benchmark for trentor's InetAddress vs ipv4/ipv6 performance; add InetAddress if necessary.


## GTask
About GTask (task management tool) (Git + TaskWarrior)

We use `gtask` (a custom script built on top of [TaskWarrior](https://taskwarrior.org/) for task management) as a ToDo list. Of course long-term todos will not be there so the ideas don't get out before they need to.

### Usage:

```bash
gtask list
gtask add project:net "This is a todo"
```

Learn more about TaskWarrior in their own documentation.


Current Todo List (use `./gtask-markdown list` to generate this table again):


## Todo List
Last updated time: Tue Jan 18 10:53:46 UTC 2022

| ID | Age    | Project     | Description                                                                                                                                     | Urg |
| -- | ------ | ----------- | ----------------------------------------------------------------------------------------------------------------------------------------------- | ---- |
| 11 | 2.3y   |             | put the validations in their own file                                                                                                           |    2 |
| 18 | 2.0y   |             | Use Steganography in your project, it's cool                                                                                                    |    2 |
| 50 | 1.8y   |             | add thread_pool support                                                                                                                         |    2 |
| 62 | 1.8y   |             | add jetbrains logo                                                                                                                              |    2 |
| 74 | 1.1y   |             | remove magic_enums as dependency                                                                                                                |    2 |
| 77 | 1.1y   | STL         | should we add 'using namespace std;' in webpp::istl?                                                                                            |    3 |
|  8 | 2.4y   | ams         | consider help functions for webassembly                                                                                                         |    3 |
| 63 | 1.7y   | clang       | Add custom clang-check matchers to chatch some bugs before they happen for the final user                                                       |    3 |
| 53 | 1.8y   | cluster     | add clustering communication base classes                                                                                                       |    3 |
| 54 | 1.8y   | cluster     | clusters can run a thread_pool too                                                                                                              |    3 |
| 55 | 1.8y   | cluster     | implement thread_pool classes                                                                                                                   |    3 |
| 56 | 1.8y   | cluster     | thread-safe properties with the help of marcos or pre-compile code generation based on the spacified class or type                              |    3 |
| 57 | 1.8y   | cluster     | see if it's possible to use operator overloading magics to implement threadsafe properties                                                      |    3 |
|  3 | 2.5y   | cmake       | use components like cefeika                                                                                                                     |    3 |
|  6 | 2.4y   | cmake       | consider cpp modules                                                                                                                            |    3 |
|  7 | 2.4y   | cmake       | add different packages for distribution                                                                                                         |    3 |
| 28 | 1.9y   | cmake       | add pre-compiling the static files feature in the cmake for different file types in the generate_static_file function                           |    3 |
| 30 | 1.8y   | cmake       | configure and use sanitizers                                                                                                                    |    3 |
| 76 | 1.1y   | cmake       | use source_group to group files logically                                                                                                       |    3 |
| 29 | 1.9y   | codegen     | forms for auto code generation, so the user can easily write a boilerplate and change the code later.                                           |    3 |
| 65 | 1.6y   | cookies     | parse response cookies                                                                                                                          |    3 |
| 66 | 1.6y   | cookies     | add cookie extensions                                                                                                                           |    3 |
| 67 | 1.6y   | cookies     | add extension for session management                                                                                                            |    3 |
| 68 | 1.6y   | cookies     | implement encrypted cookies                                                                                                                     |    3 |
| 81 |   3mo  | crypto      | implement gzip using openssl if possible                                                                                                        | 1.58 |
| 32 | 1.8y   | fcgi        | convert endpoint fromats                                                                                                                        |    3 |
| 33 | 1.8y   | fcgi        | find where to add the thread_pool                                                                                                               |    3 |
| 34 | 1.8y   | fcgi        | add the ctor of threadpool                                                                                                                      |    3 |
| 35 | 1.8y   | fcgi        | use threadpool in running the io_context                                                                                                        |    3 |
| 36 | 1.8y   | fcgi        | restart the server when the endpoints change                                                                                                    |    3 |
| 37 | 1.8y   | fcgi        | learn boost::cpp03::server4::server::operator() example                                                                                         |    3 |
| 38 | 1.8y   | fcgi        | add thread_count in fcgi class                                                                                                                  |    3 |
| 39 | 1.8y   | fcgi        | implement request methods in fcgi                                                                                                               |    3 |
| 40 | 1.8y   | fcgi        | write a fcgi hello world example                                                                                                                |    3 |
| 41 | 1.8y   | fcgi        | write fcgi tests                                                                                                                                |    3 |
| 42 | 1.8y   | fcgi        | read the fcgi header async                                                                                                                      |    3 |
| 43 | 1.8y   | fcgi        | parse the fcgi header                                                                                                                           |    3 |
| 44 | 1.8y   | fcgi        | read the rest of the fcgi request                                                                                                               |    3 |
| 45 | 1.8y   | fcgi        | parse the rest of the fcgi request                                                                                                              |    3 |
| 46 | 1.8y   | fcgi        | add constructors for protocols' file's classes                                                                                                  |    3 |
| 47 | 1.8y   | fcgi        | figure out a way to listen on multiple endpoints                                                                                                |    3 |
| 12 | 2.1y   | functional  | implement trailing in debounce class                                                                                                            |    3 |
| 13 | 2.1y   | functional  | add a way to use a thread pool in the debounce class                                                                                            |    3 |
| 14 | 2.1y   | functional  | make property class thread safe if the user wants it                                                                                            |    3 |
| 15 | 2.1y   | functional  | add functionality to check if the user is running the program in debug mode or release mode                                                     |    3 |
| 16 | 2.1y   | functional  | add functionality to the properties class and debounced class to check if the user is going to have a race condition or not if all possible     |    3 |
| 17 | 2.1y   | functional  | check if you actually need the function_ref class or not                                                                                        |    3 |
| 64 | 1.6y   | headers     | give the user the ability to change the status phrase of status code                                                                            |    3 |
|  1 | 2.5y   | interfaces  | implement fcgi                                                                                                                                  |    3 |
|  5 | 2.4y   | interfaces  | implement wsgi                                                                                                                                  |    3 |
| 80 |  11mo  | log         | use c++20 std::source_location for logging                                                                                                      | 2.84 |
| 19 | 1.9y   | modules     | add mother modules to the server's templtes                                                                                                     |    3 |
| 10 | 2.3y   | net         | add mac address class                                                                                                                           |    3 |
| 21 | 1.9y   | net         | add fragment routing methods in uri (think about it first)                                                                                      |    3 |
| 22 | 1.9y   | net         | implement put_query in uri                                                                                                                      |    3 |
| 23 | 1.9y   | net         | implement get_query in uri                                                                                                                      |    3 |
| 24 | 1.9y   | net         | implement remove_query in uri                                                                                                                   |    3 |
| 25 | 1.9y   | net         | implement append_query in uri                                                                                                                   |    3 |
| 26 | 1.9y   | net         | implement pop_path in uri                                                                                                                       |    3 |
| 27 | 1.9y   | net         | implement append_path in uri                                                                                                                    |    3 |
| 75 | 1.1y   | net         | add 128bit support to ipv6 class for systems that supports them                                                                                 |    3 |
|  2 | 2.5y   | performance | implement half-lazy loading of images/... stuff                                                                                                 |    3 |
| 49 | 1.8y   | performance | optimize ipv6 and checkout the benchmark                                                                                                        |    3 |
| 70 | 1.1y   | performance | add logger_allocator custom allocator                                                                                                           |    3 |
| 71 | 1.1y   | performance | add add null_allocator custom allocator to help debug allocations                                                                               |    3 |
| 72 | 1.1y   | performance | add add allocator category concept                                                                                                              |    3 |
| 73 | 1.1y   | performance | add add allocator categories to the traits                                                                                                      |    3 |
| 48 | 1.8y   | pub         | Make travis build successful                                                                                                                    |    3 |
| 20 | 1.9y   | request     | sort paths in router based on views and how much time it takes for them to be evaluated                                                         |    3 |
| 51 | 1.8y   | tests       | add a high level testing library that uses gtest and catch2 and boost.test and others                                                           |    3 |
| 52 | 1.8y   | tests       | convert existing tests into your own testing library                                                                                            |    3 |
| 69 | 1.1y   | tests       | write tests for utils/errors                                                                                                                    |    3 |
| 58 | 1.8y   | thread      | add fast and thread-safe class template for containers                                                                                          |    3 |
| 78 | 1.1y   | traits      | remove stateful and stateless from allocator features                                                                                           |    3 |
| 31 | 1.8y   | uri         | add support for encrypted URIs                                                                                                                  |    3 |
| 79 | 1.0y   | uri         | add support for RFC 6570 (URI Template)                                                                                                         |    3 |
|  9 | 2.4y   | utils       | implement property class                                                                                                                        |    3 |
| 82 |   3mo  | utils       | Add archive file compression utilities like .zip                                                                                                | 1.57 |
| 59 | 1.8y   | valves      | a valve that limits user visits                                                                                                                 |    3 |
| 60 | 1.8y   | valves      | a valve that only allows guests                                                                                                                 |    3 |
| 61 | 1.8y   | valves      | a valve that only allows registered users                                                                                                       |    3 |
|  4 | 2.4y   | webrtc      | watch WebRTC tutorials                                                                                                                          |    3 |
