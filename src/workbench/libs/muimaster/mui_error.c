/*
    Copyright (C) 2002-2007, The AROS Development Team. All rights reserved.
*/

#include "muimaster_intern.h"

/*****************************************************************************

    NAME */
        __asm __saveds LONG MUI_Error(void)

/*  FUNCTION
        Obsolete function. Use SetIoErr()/IoErr() instead.

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
        MUI_SetError()

    INTERNALS

*****************************************************************************/
{
    return 0;
} /* MUI_Error */
