/* fnmatch.h - modified from emx+gcc */

#ifndef _FNMATCH_H
#define _FNMATCH_H

#define FNM_NOMATCH        1

#define FNM_NOESCAPE       16
#define FNM_PATHNAME       32
#define FNM_PERIOD         64

#define _FNM_MATCH         0
#define _FNM_ERR           2

#define _FNM_STYLE_MASK    15

#define _FNM_POSIX         0
#define _FNM_OS2           1
#define _FNM_DOS           2

#define _FNM_IGNORECASE    128
#define _FNM_PATHPREFIX    256

int _fnmatch (const char *mask, const char *name, int flags);

#define fnmatch _fnmatch

#endif /* not _FNMATCH_H */
