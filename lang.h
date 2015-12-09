#include "ver.h"


#define MSG_USAGE \
	"deco file extraction framework " VERSION " <https://github.com/peha/deco>\n" \
	"\n"                                                                          \
	"USAGE: deco [option...] archive...\n"                                        \
	"\n"                                                                          \
	"OPTIONS: -a      absolute (extract in archive's directory)\n"                \
	"         -c      clean up partial contents if extraction fails\n"            \
	"         -e ext  act as if archive's longest extension was .ext\n"           \
	"         -u      unlink archive and auxiliary files if successful\n"         \
	"         -v      verbose\n"

#define MSG_EMPTY      "empty archive"
#define MSG_EXTRACT    "could not extract archive"
#define MSG_FIND_EXTR  "no matching extractor"
#define MSG_IS_DIR     "is a directory"
#define MSG_NORM_PERMS "could not normalize permissions"
#define MSG_NOT_EXIST  "does not exist"
#define MSG_NO_EXTRS   "no extractor wrappers found"

#define MSG_ALLOC      "failed to allocate memory"
#define MSG_CHDIR      "failed to change working directory"
#define MSG_DEL        "failed to delete"
#define MSG_OUT_TO_ERR "failed to redirect stdout to stderr"
#define MSG_EXEC       "failed to run command"
#define MSG_FORK       "failed to fork"
#define MSG_LINK       "failed to create hard link"
#define MSG_MKDIR      "failed to create directory"
#define MSG_OPENDIR    "failed to open directory"
#define MSG_PUTENV     "failed to modify environment"
#define MSG_READLINK   "failed to get target of symbolic link"
#define MSG_REGCOMP    "failed to compile regular expression"
#define MSG_RENAME     "failed to rename"
#define MSG_STAT       "failed to get information"
#define MSG_SYMLINK    "failed to create symbolic link"
