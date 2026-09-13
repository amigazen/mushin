/*
    Copyright (C) 2002-2012, The AROS Development Team. All rights reserved.
*/


#define MUIMASTER_DEFINING_CLIPPING
#include "mui.h"
#include "muimaster_intern.h"
#include "support.h"

VOID ZuneRemoveClipping(struct MUI_RenderInfo *mri, APTR handle)
{
    ZuneRemoveClipRegion(mri, handle);
}

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
    ZuneRemoveClipping(mri, handle);
} /* MUI_RemoveClipping */
