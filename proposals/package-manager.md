# wpm: Web++ Package Manager
It's a package manager for the webpp project.

## Brief usage from CLI

* `wpm install extension/[extension-name]`: install a new extension (replace `[extension-name]` with your extension name)
* `wpm update`: update the local copy of available repositories


## Packages
There are many different packages you can install; these are the types of those pacakges:

- **Extension**:     `extension/...`    or `e/...`
- **Application**:   `application/...`  or `app/...`  or `a/...`
- **Library**:       `library/...`      or `lib/...`  or `l/...`

## Sources
There will be different sources to download, upload, store, and install from;
these sources will hold the packages. Here are the packages that we're planning to
have (but the ability to add more should be possible):

- __npm__ package manager
- __git__ repositories
- __archive files__ storred locally or on remote location
- __Operating System Package Managers__

