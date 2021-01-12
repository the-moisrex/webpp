# wpm: Web++ Package Manager
It's a package manager for the webpp project.

## Brief usage from CLI

* `wpm install extension/[extension-name]`: install a new extension (replace `[extension-name]` with your extension name)
* `wpm i app/simple-blog`: install _simple-bloc_ app
* `wpm gi app/simple-app`: copy the source code into the global copy and link local to global
* `wpm install system/boost`: install boost with system's package manager if any
* `wpm delete app/simple-blog`
* `wpm info ext/auth`: get the information about the extension _auth_
* `wpm update`: update the local copy of available repositories
* `wpm clear`: clear all local caches
* `wpm g clear`: clear global caches
* `wpm create migration --title="add new table"`

Any command done with `wpm g` or `wpm global` is done globally; we probably don't need `--global` idea anymore.

## Packages
There are many different packages you can install; these are the types of those packages:

| Name               | Full               | Short      | Shorter |
|--------------------|--------------------|------------|---------|
| **Extension**:     | `extension/...`    | `ext/...`  | `e/...` |
| **Application**:   | `application/...`  | `app/...`  | `a/...` |
| **Library**:       | `library/...`      | `lib/...`  | `l/...` |
| **System Library** | `system/...`       | `sys/...`  | `s/...` |


## Sources
There will be different sources to download, upload, store, and install from;
these sources will hold the packages. Here are the packages that we're planning to
have (but the ability to add more should be possible):

- __npm__ package manager
- __git__ repositories
- __archive files__ stored locally or on remote location
- __Operating System Package Managers__

## Package Manifest Stores
The package stores are a remote or local storage that holds a manifest of all the available packages known to that store.

Store types:

- __Git Repo__
- __Local or Remote file__:
	- __txt file__
	- __json file__

### Manifest details:
- __name__
- __type__: `extension` / `app` / `library`
- __urls__: [array]
	- url
	- versioning system: `git tags` / `in url` / ...


