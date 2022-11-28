#ifndef isomaster_h
#define isomaster_h

#include "common.h"

#include "bk/bk.h"
#include "window.h"
#include "browser.h"
#include "fsbrowser.h"
#include "isobrowser.h"
#include "settings.h"
#include "error.h"
#include "boot.h"
#include "about.h"
#include "editfile.h"

/* c99 doesn't define the following (posix file types) */
#ifndef S_IFDIR
#  define S_IFDIR  0040000
#endif
#ifndef S_IFREG
#  define S_IFREG  0100000
#endif

#endif
