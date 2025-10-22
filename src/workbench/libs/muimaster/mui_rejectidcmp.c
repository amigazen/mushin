/*
    Copyright (C) 2002-2007, The AROS Development Team. All rights reserved.
*/

#include <proto/intuition.h>
#include <intuition/classusr.h>
#include <clib/alib_protos.h>

#include "muimaster_intern.h"
#include "mui.h"

/*****************************************************************************

    NAME */
        __asm __saveds VOID MUI_RejectIDCMP(register __a0 Object *obj, register __d0 ULONG flags)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS
        The function itself is a bug ;-) Remove it!

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    if (muiRenderInfo(obj) && (_flags(obj) & MADF_SETUP) && _win(obj))
    {
        if (muiAreaData(obj)->mad_hiehn.ehn_Events)
        {
            DoMethod(_win(obj), MUIM_Window_RemEventHandler,
                (IPTR)&muiAreaData(obj)->mad_hiehn);
        }
        muiAreaData(obj)->mad_hiehn.ehn_Events &= ~flags;
        if (muiAreaData(obj)->mad_hiehn.ehn_Events)
        {
            DoMethod(_win(obj), MUIM_Window_AddEventHandler,
                (IPTR)&muiAreaData(obj)->mad_hiehn);
        }
    }
    else
    {
        muiAreaData(obj)->mad_hiehn.ehn_Events &= ~flags;
    }
} /* MUI_RejectIDCMP */
