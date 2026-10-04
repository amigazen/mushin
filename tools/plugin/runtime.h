#ifndef MUSHIN_PLUGIN_RUNTIME_H
#define MUSHIN_PLUGIN_RUNTIME_H
#include <string.h>

/* Read libnix's declarations and inline definitions before redirecting
 * imported callers to our helpers. Some toolchain releases supply strlcpy. */
size_t mushin_strlcpy(char *dst, const char *src, size_t size);
size_t mushin_strlcat(char *dst, const char *src, size_t size);
#define strlcpy mushin_strlcpy
#define strlcat mushin_strlcat
#endif
