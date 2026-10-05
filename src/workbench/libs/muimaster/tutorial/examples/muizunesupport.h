/*
    Copyright (C) 2003-2011, The AROS Development Team.
    All rights reserved.

*/

#ifndef _ZUNE_MUISUPPORT_H
#define _ZUNE_MUISUPPORT_H

#include <string.h>
#include <stdio.h>

#include <exec/memory.h>
#include <exec/types.h>

#include <libraries/asl.h>
#include <libraries/mui.h>
#include <prefs/prefhdr.h>

#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/iffparse.h>
#include <proto/muimaster.h>

/* Single-TU examples: provide the base the pragmas expect. */
struct Library *MUIMasterBase;

Object *MakeLabel(STRPTR str);
LONG xget(Object *obj, ULONG attr);

#define getstring(obj) (char *)xget(obj, MUIA_String_Contents)

#define SimpleText(text) TextObject, MUIA_Text_Contents, (IPTR)text, End

/*
 * Open the installed MUI/Zune library.  MUIMASTER_NAME comes from
 * <libraries/mui.h>: "zunemaster.library" unless the example was compiled
 * with MUIMASTER_DROPIN (then "muimaster.library").  Pass a library name on
 * the command line to override, same idea as opentest.
 */
int open_muimaster(CONST_STRPTR name)
{
    if (name == NULL || name[0] == '\0')
        name = MUIMASTER_NAME;

    MUIMasterBase = OpenLibrary(name, MUIMASTER_VMIN);
    if (MUIMasterBase == NULL)
    {
        printf("OpenLibrary(\"%s\", %ld) failed\n",
            name, (long)MUIMASTER_VMIN);
        printf("Build/install the library first, or pass another name:\n");
        printf("  HelloZune muimaster.library\n");
        printf("  HelloZune zunemaster.library\n");
        return 0;
    }
    printf("Using %s at 0x%08lx\n", name, (unsigned long)MUIMasterBase);
    return 1;
}

void close_muimaster(void)
{
    if (MUIMasterBase != NULL)
    {
        CloseLibrary(MUIMasterBase);
        MUIMasterBase = NULL;
    }
}

/****************************************************************
 Open needed libraries
*****************************************************************/
int open_libs(CONST_STRPTR libname)
{
    if (open_muimaster(libname))
        return 1;
    return 0;
}

/****************************************************************
 Close opened libraries
*****************************************************************/
void close_libs(void)
{
    close_muimaster();
}

/****************************************************************
 Create a simple label
*****************************************************************/
Object *MakeLabel(STRPTR str)
{
    return (MUI_MakeObject(MUIO_Label, str, 0));
}

/****************************************************************
 Easy getting an attributes value
*****************************************************************/
LONG xget(Object *obj, ULONG attr)
{
    LONG x = 0;

    get(obj, attr, &x);
    return x;
}

#endif /* _ZUNE_MUISUPPORT_H */
