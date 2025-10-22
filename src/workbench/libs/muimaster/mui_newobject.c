/*
    Copyright (C) 2002-2007, The AROS Development Team. All rights reserved.
*/

#include <stdarg.h>
#include <proto/exec.h>
#include <proto/intuition.h>

#include "mui.h"
#include "muimaster_intern.h"

/* #define MYDEBUG 1 */
#include "debug.h"

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

        bug("*** Could not create object of %s\n", classid);
        MUI_FreeClass(cl);
    }

    bug("*** Couldn't find %s\n", classid);

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
    
    tagList = (struct TagItem *)&classname;
    retval = MUI_NewObjectA(classname, tagList);
    
    return retval;
    
} /* MUI_NewObject */
