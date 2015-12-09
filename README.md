# deco file extraction framework

deco is a Un*x program, written in C, that is able to extract various file
formats using small “extractor wrapper” scripts.



## Features


### Consistent command line interface

Instead of having to remember e. g.

    tar xjf 1.tar.bz2
    unrar x 2.rar
    flac -d 3.flac
    dpkg-deb --extract 4.deb

you say

    deco 1.tar.bz2 2.rar 3.flac 4.deb

and deco extracts them all, provided you’ve also installed the
[deco-archive package](https://github.com/peha/deco-archive).


### Consistent behavior

#### Where to extract

Some extractors extract in the current working directory, others use the
directory the archive resides in. deco extracts relative to the current working
directory, unless you give it the `-a` (“absolute”) option.

#### Delete archive?

Some extractors delete the archive after successful extraction, others don’t.
deco preserves the archive after successful extraction, unless you give it the
`-u` (“unlink”) option.

#### Quiet or verbose

Some extractors process the archive verbosely by default and expect you to
specify that quiet behavior is wanted. Others do the opposite. deco extracts
quietly, unless you give it the `-v` (“verbose”) option.

#### Partial content

Some extractors will leave the partial content extracted so far around if
extraction fails. Others will delete it. deco will keep partial content, unless
you give it the `-c` (“clean”) option.

#### Unknown file extensions

Some extractors refuse to even try extracting anything with an unknown
extension. deco can force an extractor to handle a file with any name. Use the
`-e` (“extension”) option: `deco -e gz myarchive`

#### Sanitized permissions

There are extractors that create files with strange permissions. deco can reset
these to sensible defaults implied by your current umask.

#### No tarbombs

When you extract an archive that contains multiple files at its top level, you
usually want them to be placed in a directory of their own. However, if the
archive contains only one file or one big directory, that behavior would be
annoying. The deco extraction algorithm does the right thing automatically:
generally, if and only if archive.foo contains multiple files at its top level,
a new directory called archive/ is created and the archive gets extracted there.

#### No overwriting

deco never overwrites files, nor does it need to prompt you for manual conflict
resolution. If the file to be extracted is named foo and a file by that name
already exists, the new file will be extracted as 1-foo, if that name also
exists, 2-foo, and so on.  Since at the top level only one new file or directory
is created, at most this one file will have its name changed like that.


### Modular design

As all the heavy lifting is done in the core algorithm, [adding support for new
archive formats requires very little work](API.md).



## Installation

1. **Configure:**  Edit config.mk to change compiler/linker settings, the
   location to install at, or the location where deco looks for extractor
   wrappers.

2. **Build:**  Run `make`. If you have trouble building on BSD, try removing
   -D_XOPEN_SOURCE=600 from CPPFLAGS in config.mk.

3. **Install:**  Run `make install`. (You will probably need to do this as the
   superuser.) To add support for popular archive formats, also install
   [deco-archive](https://github.com/peha/deco-archive).

4. **Use:**  Run `deco` to see the usage message.



## Links


### Extractor wrappers for deco

- [deco-archive](https://github.com/peha/deco-archive): support for popular
  archive formats
- [deco-diskimage](http://www.daeghnao.com/deco-diskimage/): support for filesystem disk images


### Similar programs

- [archery](http://pturing.firehead.org/software/archery/)
- [atool](http://www.nongnu.org/atool/)
- [ax](http://www.noah.org/wiki/Ax_Archive_Extract)
- [bloat](http://www.bloat.org.uk/)
- [dtrx](http://brettcsmith.org/2007/dtrx/)
- [e](http://martin.ankerl.com/2006/08/11/program-e-extract-any-archive),
  and [a modified version](http://blog.thoughtfolder.com/2008-02-01-the-script-i-use-every-day.html)
- [libarchive](https://github.com/libarchive/libarchive)
- [unarc](http://membled.com/work/apps/unarc/)
- [unfoo](http://obsoleet.org/code/unfoo/)
- [unp](http://packages.debian.org/unstable/source/unp)



## Credits

Copyright 2007-2015 Peter Hartlich. [Licensed under the terms of the GNU General
Public License, version 3.](LICENSE)


### Thanks to:

- Zoë Stephenson for several bug reports
- Reuben Thomas for the Makefile uninstall target, suggesting that the usage
  message include deco’s version, and reporting various problems
- Steve Milner for the concept of what is now the `subdirectory` file
- Dirk Jagdmann for reporting the problem with extracting large files
- Michael Schwendt for diagnosing a crash when handling archive formats that
  always extract to a subdirectory
