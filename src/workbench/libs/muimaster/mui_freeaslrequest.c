/*
    Copyright (C) 2002-2007, The AROS Development Team. All rights reserved.
*/

#include <proto/asl.h>

#include "muimaster_intern.h"

/*****************************************************************************

    NAME */
        MUI_LIB_ENTRY VOID MUI_FreeAslRequest(MUI_LIB_ARG(a0, APTR requester))

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
