/*
    Copyright (C) 2002-2007, The AROS Development Team. All rights reserved.
*/

#include <proto/asl.h>

#include "muimaster_intern.h"

/*****************************************************************************

    NAME */
        __asm __saveds VOID MUI_FreeAslRequest(register __a0 APTR requester)

/*  FUNCTION
        Interface to asl.library

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
        asl.library/FreeAslRequest()

    INTERNALS

*****************************************************************************/
{
    FreeAslRequest(requester);
} /* MUI_FreeAslRequest */
