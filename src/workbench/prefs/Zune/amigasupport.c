/*
 * Small AmigaOS helpers used by the Zune prefs sources.
 * DoSuperNewTags / snprintf live inside muimaster.library on AROS builds
 * that link the support objects; a prefs executable that only OpenLibrarys
 * the master needs its own copies.
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <exec/types.h>
#include <intuition/classusr.h>
#include <intuition/classes.h>
#include <utility/tagitem.h>
#include <clib/alib_protos.h>
#include <libraries/mui.h>
#include <proto/intuition.h>

#ifndef __AROS__

#ifndef __GNUC__
IPTR XGET(Object *obj, Tag attr)
{
    IPTR storage = 0;

    GetAttr(attr, obj, &storage);
    return storage;
}

#endif

Object *DoSuperNewTagList(struct IClass *cl, Object *obj, void *dummy,
                          struct TagItem *tags)
{
    (void)dummy;
    return (Object *)DoSuperMethod(cl, obj, OM_NEW, tags, NULL);
}

#ifdef __SASC
Object *VARARGS68K DoSuperNewTags(struct IClass *cl, Object *obj, void *dummy, ...)
{
    va_list argptr;

    /* Same pattern as support_amigaos.c for classic Amiga. */
    va_start(argptr, dummy);
    obj = DoSuperNewTagList(cl, obj, dummy, (struct TagItem *)argptr);
    va_end(argptr);
    return obj;
}
#else
Object *VARARGS68K DoSuperNewTags(struct IClass *cl, Object *obj, void *dummy, ...)
{
    va_list argptr;
    struct TagItem *tagList;

    va_start(argptr, dummy);
    tagList = (struct TagItem *)argptr;
    obj = DoSuperNewTagList(cl, obj, dummy, tagList);
    va_end(argptr);
    return obj;
}
#endif

#ifdef __SASC
int snprintf(char *buf, int size, const char *fmt, ...)
{
    va_list args;
    int n;
    char tmp[256];

    if (buf == NULL || size <= 0)
        return 0;

    va_start(args, fmt);
    n = vsprintf(tmp, fmt, args);
    va_end(args);

    if (n < 0)
        n = 0;
    if (n >= size)
        n = size - 1;
    memcpy(buf, tmp, (size_t)n);
    buf[n] = '\0';
    return n;
}

#endif /* __SASC */

#endif /* !__AROS__ */
