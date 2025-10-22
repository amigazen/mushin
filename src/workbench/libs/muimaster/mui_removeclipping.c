/*
    Copyright (C) 2002-2012, The AROS Development Team. All rights reserved.
*/


#include "mui.h"
#include "muimaster_intern.h"

/*****************************************************************************

    NAME */
        __asm __saveds VOID MUI_RemoveClipping(register __a0 struct MUI_RenderInfo *mri, register __a1 APTR handle)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    MUI_RemoveClipRegion(mri, handle);
} /* MUI_RemoveClipping */
