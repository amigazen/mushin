/*
    Copyright (C) 2002-2007, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/intuition.h>

#include "mui.h"
#include "muimaster_intern.h"
#include "support.h"

/*****************************************************************************

    NAME */
        __asm __saveds BOOL MUI_DeleteCustomClass(register __a0 struct MUI_CustomClass *mcc)

/*  FUNCTION
        Delete private or public custom classes.

    INPUTS
        mcc - pointer from MUI_CreateCustomClass()

    RESULT
        TRUE  : success
        FALSE : some objects or sub classes were still in use.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
        MUI_CreateCustomClass()

    INTERNALS

*****************************************************************************/
{
    if (mcc)
    {
        Class *super = mcc->mcc_Super;
        
        if (FreeClass(mcc->mcc_Class))
        {
            mui_free(mcc);

            MUI_FreeClass(super);

            return TRUE;
        }
    }

    return FALSE;
} /* MUI_DeleteCustomClass */
