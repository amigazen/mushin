/*
    Copyright (C) 2002, The AROS Development Team.
    All rights reserved.
    
*/

#include <utility/tagitem.h>
#include <proto/asl.h>
#include <stdarg.h>

/*****************************************************************************

    NAME */
extern APTR MUI_AllocAslRequest(ULONG reqType, struct TagItem *tagList);

        APTR MUI_AllocAslRequestTags (

/*  SYNOPSIS */
        ULONG reqType,
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
    APTR retval;
    
    va_start(args, reqType);
    tagList = (struct TagItem *)va_arg(args, Tag);
    
    retval = MUI_AllocAslRequest(reqType, tagList);
    
    va_end(args);
    return retval;
} /* MUI_AllocAslRequestTags */
