#ifndef __BITSC_UTILS_H__
#define __BITSC_UTILS_H__

#include "type.h"
#include "memory.h"

#define tb_arrayn(x)                     (sizeof((x)) / sizeof((x)[0]))

#define tb_min(x, y)                     ((x) < (y)) ? (x) : (y)

#define tb_free(p)                       do { if(p) free(p); p = NULL } while (0);

#endif
