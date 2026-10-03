/*
    Copyright (C) 2002-2007, The AROS Development Team. All rights reserved.
*/

#include <proto/asl.h>

#include "muimaster_intern.h"

/*****************************************************************************

    NAME */
        MUI_LIB_ENTRY BOOL MUI_AslRequest(MUI_LIB_ARG(a0, APTR requester), MUI_LIB_ARG(a1, struct TagItem *tagList))

/*  FUNCTION
        Interface to asl.library.

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
        asl.library/AslRequest()

    INTERNALS

*****************************************************************************/
{
    return AslRequest(requester, tagList);
} /* MUI_AslRequest */
