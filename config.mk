CC       = c99
CPPFLAGS = -D_XOPEN_SOURCE=600 -D_FILE_OFFSET_BITS=64
CFLAGS   = -O2
LDFLAGS  = -s

DESTDIR =
PREFIX  = /usr/local
BIN     = $(PREFIX)/bin
SHARE   = $(PREFIX)/share/deco
