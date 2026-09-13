/*
    Copyright (C) 2002-2007, The AROS Development Team. All rights reserved.
*/

#include <proto/intuition.h>
#include <intuition/classes.h>

#include "muimaster_intern.h"
#include "support.h"

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
    volatile ULONG clsave;
    STRPTR id;
    ULONG flags;
    ULONG ud;
    Class *cl;

    if (obj == NULL)
        return;

    /*
     * clsave is volatile so SAS/C must reload it from the stack for
     * MUI_FreeClass (A0).  A plain ULONG was kept in A0; DisposeObject(obj)
     * then left A0==obj and FreeClass decremented the freed object (logged
     * cl==obj, id=?).  Do not call anything between DisposeObject and
     * MUI_FreeClass.
     */
    cl = OCLASS(obj);
    clsave = (ULONG) cl;
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
        (ULONG) obj, clsave, id, flags, ud);
    DisposeObject(obj);
    MUI_FreeClass((Class *) clsave);
} /* MUI_DisposeObject */
