# deco 1.* programming interface

Let’s assume you want to support foo archives, which can be composed of multiple
parts — archive.foo, archive.f00, possibly up to archive.f99:

- In deco’s extractor wrappers directory (system-wide typically
  /usr/local/share/deco, or .deco in your user’s home directory) add a new
  directory. This directory’s name needs to be a (case-insensitive, POSIX
  extended) regular expression that matches the archive file extension (it will
  automatically be prefixed with a literal dot and anchored to the end of the
  archive file name). For our example, a good choice would be: `f(oo|[0-9]{2})`

- Inside your newly created directory, add an executable file called `extract`,
  maybe a small shell script.

  - Whenever deco wants to extract a file some/where/archive.foo, it runs your
    script with the parameter `./archive.foo`. The file will be symlinked in
    extract’s working directory.

  - There are two environment variables specifying preferred extractor behavior.
    Such a variable will be either enabled (= set to the empty string) or
    disabled (= unset):

    - `Verbose`, if enabled, requests verbose output.

    - `Keep`, if enabled, requests that the extractor continue extracting in
      case of an extraction error instead of aborting and deleting partial
      content.

  - For your convenience, the environment variable `Name` will be set to
    `archive` (no extension), but usually you won’t need to use it.

  - Your script should then extract the archive in whatever way is appropriate.
    It doesn’t matter whether or not the archive symlink is removed during
    extraction.

- You can create additional files in the `f(oo|[0-9]{2})` directory to modify
  deco’s behavior:

  - If the contents of archive.foo should always be put into a new directory
    generally called archive/, even when the archive only contains a single
    entry at its top level, add an empty file called `subdirectory`.

  - In case your extractor is known to create files with weird permissions and
    you want them changed to the defaults implied by the current umask, add an
    empty file called `permissions`.

- Anything the extract program writes to stdout is automatically redirected to
  stderr.

- The longest matching extension is used, unless the `-e` option is in effect.



## Example `extract` script

    #!/bin/sh
    exec unfoo ${Keep+-k} ${Verbose--q} "$1"
