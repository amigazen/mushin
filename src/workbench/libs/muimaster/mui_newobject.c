/*
    Copyright (C) 2002-2007, The AROS Development Team. All rights reserved.
*/

#include <stdarg.h>
#include <proto/exec.h>
#include <proto/intuition.h>

#include "mui.h"
#include "muimaster_intern.h"
#include "support.h"

/* #define MYDEBUG 1 */
#include "debug.h"

/* Stack-safe helpers (LVO forms expect args in registers). */
extern __asm __saveds struct IClass *MUI_GetClass(register __a0 ClassID classid);
extern VOID ZUNE_FreeClass(Class *cl);
/*****************************************************************************

    NAME */
        __asm __saveds Object *MUI_NewObjectA(register __a0 ClassID classid, register __a1 struct TagItem *tags)

/*  FUNCTION
        Create object from MUI class.

    INPUTS
        classid - case sensitive name/ID string of a MUI class.
        taglist - attribute/value pairs for the new object.

    RESULT
        Pointer to object. NULL means failure.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
        MUI_DisposeObject(), intuition.library/SetAttrsA()
        intuition.library/GetAttr()

    INTERNALS

*****************************************************************************/
{
    Class  *cl;

    cl = MUI_GetClass(classid);
    if (cl)
    {
        Object *obj = NewObjectA(cl, NULL, tags);

        if (obj) return obj;

        ZuneTrace("zune: NewObject failed class=%s\n",
            classid ? classid : (CONST_STRPTR) "(null)");
        ZUNE_FreeClass(cl);
    }
    else
    {
        ZuneTrace("zune: GetClass failed class=%s\n",
            classid ? classid : (CONST_STRPTR) "(null)");
    }

    return NULL;
} /* MUI_NewObjectA */

/*****************************************************************************

    NAME */
        Object * MUI_NewObject (

/*  SYNOPSIS */
        CONST_STRPTR classname,
        ...)

/*  FUNCTION
        Create a new object from a class.

    INPUTS
        classname - Name of the class to create
        ... - Tag list parameters

    RESULT
        Object pointer or NULL on failure

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

*****************************************************************************/
{
    struct TagItem *tagList;
    Object *retval;
    
    tagList = (struct TagItem *)(&classname + 1);
    retval = MUI_NewObjectA(classname, tagList);
    
    return retval;
    
} /* MUI_NewObject */
