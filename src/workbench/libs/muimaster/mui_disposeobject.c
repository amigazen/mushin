/*
    Copyright (C) 2002-2007, The AROS Development Team. All rights reserved.
*/

#include <proto/intuition.h>
#include <intuition/classes.h>

#include "muimaster_intern.h"

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
    if (obj == NULL)
        return;
    
    {
        Class *cl = OCLASS(obj);
        DisposeObject(obj);
        MUI_FreeClass(cl);
    }
} /* MUI_DisposeObject */
