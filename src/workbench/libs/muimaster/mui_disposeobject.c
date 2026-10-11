/*
    Copyright (C) 2002-2007, The AROS Development Team. All rights reserved.
    Copyright (C) 2026, amigazen project. All rights reserved.
*/

#include <string.h>
#include <proto/intuition.h>
#include <intuition/classes.h>

#include "muimaster_intern.h"
#include "support.h"

extern VOID ZUNE_FreeClass(Class *cl);

/*****************************************************************************

    NAME */
        __asm __saveds VOID MUI_DisposeObject(register __a0 Object *obj)

/*  FUNCTION
        Deletes MUI object and its child objects.
        
    INPUTS
        obj - pointer to MUI object created with MUI_NewObject. May be NULL,
              in which case this function has no effect.
        
    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS
        MUI will call DisposeObject(), then call CloseLibrary() on
        OCLASS(obj)->h_Data if cl_ID!=NULL && h_Data!=NULL.

*****************************************************************************/
{
    Class *cl;
    volatile Class *clsave;
    STRPTR id;
    ULONG flags;
    ULONG ud;

    if (obj == NULL)
        return;

    /*
     * Save the class before DisposeObject. Rootclass frees the block,
     * and a later FreeClass must not read the object: SAS/C leaves A0
     * holding obj across the call.
     */
    cl = OCLASS(obj);
    clsave = cl;
    id = (STRPTR) "?";
    flags = 0;
    ud = 0;
    if (cl != NULL)
    {
        flags = cl->cl_Flags;
        ud = cl->cl_UserData;
        if (cl->cl_ID != NULL)
            id = cl->cl_ID;
    }
    ZuneTrace("zune: DisposeObject obj=%lx cl=%lx id=%s flags=%lx ud=%ld\n",
        (ULONG) obj, (ULONG) cl, id, flags, ud);
    /* After this, builtin methods and MUI_NewObjectA are logged. */
    if (cl != NULL && cl->cl_ID != NULL &&
        strcmp(cl->cl_ID, "Menuitem.mui") == 0)
        Zune_LogAfterMenu = 1;
    /*
     * Drop a family or group link before the block goes back to the
     * pool. The next MenuitemObject of this size is often this address,
     * and a list that still names it walks into the new object.
     */
    ZuneUnlinkObject(obj);
    DisposeObject(obj);
    ZUNE_FreeClass((Class *) clsave);
    ZuneTrace("zune: DisposeObject return %lx\n", (ULONG) obj);
} /* MUI_DisposeObject */
