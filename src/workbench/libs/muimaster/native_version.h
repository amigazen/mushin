#ifndef MUSHIN_NATIVE_VERSION_H
#define MUSHIN_NATIVE_VERSION_H

/* Native distribution identity, shared by GNU and SAS/C library builds.
 * These are Mushin release numbers, not a promise of a MUI feature level.
 * Keep class implementation versions independent of the library version;
 * external classes retain their own MUIA_Version/MUIA_Revision handlers.
 */
#define MUSHIN_LIBRARY_VERSION 19
#define MUSHIN_LIBRARY_REVISION 50
#define MUSHIN_LIBRARY_DATE "27.06.2003"
#define MUSHIN_STRINGIFY_(x) #x
#define MUSHIN_STRINGIFY(x) MUSHIN_STRINGIFY_(x)
#define MUSHIN_LIBRARY_VERSION_STRING \
    MUSHIN_STRINGIFY(MUSHIN_LIBRARY_VERSION) "." \
    MUSHIN_STRINGIFY(MUSHIN_LIBRARY_REVISION)

/* Retain the established implementation series until a release changes it.
 * Do not raise these to bypass an application's unsupported-feature check. */
#define MUSHIN_BUILTIN_VERSION 1
#define MUSHIN_BUILTIN_REVISION 1

#endif
