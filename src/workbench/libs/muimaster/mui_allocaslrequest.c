/*
    Copyright (C) 2002-2007, The AROS Development Team. All rights reserved.
*/

#include <proto/asl.h>

#include "muimaster_intern.h"

/*****************************************************************************

    NAME */
        __asm __saveds APTR MUI_AllocAslRequest(register __d0 ULONG reqType, register __a0 struct TagItem *tagList)

/*  FUNCTION
        Interface to asl.library.

    INPUTS
        see asl.library/AllocAslRequest()

    RESULT
        Pointer to AslRequest

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
        asl.library/AllocAslRequest()

    INTERNALS

*****************************************************************************/
{
    return AllocAslRequest(reqType, tagList);
} /* MUI_AllocAslRequest */
