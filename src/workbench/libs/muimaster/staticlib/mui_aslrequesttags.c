/*
    Copyright (C) 2002, The AROS Development Team.
    All rights reserved.
    
*/

#include <utility/tagitem.h>
#include <proto/asl.h>
#include <stdarg.h>

/*****************************************************************************

    NAME */
extern BOOL MUI_AslRequest(APTR requester, struct TagItem *tagList);

        BOOL MUI_AslRequestTags (

/*  SYNOPSIS */
        APTR requester,
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
    BOOL retval;
    
    va_start(args, requester);
    tagList = (struct TagItem *)va_arg(args, Tag);
    
    retval = MUI_AslRequest(requester, tagList);
    
    va_end(args);
    return retval;
} /* MUI_AslRequestTags */
