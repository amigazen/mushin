/*
    Copyright (C) 2002, The AROS Development Team.
    All rights reserved.
    
*/

#include <utility/tagitem.h>
#include <proto/alib.h>
#include <stdarg.h>

/*****************************************************************************

    NAME */
#define NO_INLINE_STDARG /* turn off inline def */
#include <proto/muimaster.h>
extern struct Library * MUIMasterBase;

        Object * MUI_NewObject (

/*  SYNOPSIS */
        CONST_STRPTR classname,
        ...)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

*****************************************************************************/
{
    va_list args;
    struct TagItem *tagList;
    Object *retval;
    
    va_start(args, classname);
    tagList = (struct TagItem *)va_arg(args, Tag);
    
    retval = MUI_NewObjectA(classname, tagList);
    
    va_end(args);
    return retval;
} /* MUI_NewObject */
