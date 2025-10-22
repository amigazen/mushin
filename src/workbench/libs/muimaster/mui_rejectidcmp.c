/*
    Copyright (C) 2002-2007, The AROS Development Team. All rights reserved.
*/

#include <proto/intuition.h>
#include <intuition/classusr.h>
#include <clib/alib_protos.h>

#include "muimaster_intern.h"
#include "mui.h"
#include "area_macros.h"

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
    if (((struct __dummyAreaData__ *)(obj))->mad.mad_RenderInfo && (((struct __dummyAreaData__ *)(obj))->mad.mad_Flags & MADF_SETUP) && ((struct __dummyAreaData__ *)(obj))->mad.mad_RenderInfo->mri_WindowObject)
    {
        if (((struct __dummyAreaData__ *)(obj))->mad.mad_hiehn.ehn_Events)
        {
            DoMethod(((struct __dummyAreaData__ *)(obj))->mad.mad_RenderInfo->mri_WindowObject, MUIM_Window_RemEventHandler,
                (IPTR)&((struct __dummyAreaData__ *)(obj))->mad.mad_hiehn);
        }
        ((struct __dummyAreaData__ *)(obj))->mad.mad_hiehn.ehn_Events &= ~flags;
        if (((struct __dummyAreaData__ *)(obj))->mad.mad_hiehn.ehn_Events)
        {
            DoMethod(((struct __dummyAreaData__ *)(obj))->mad.mad_RenderInfo->mri_WindowObject, MUIM_Window_AddEventHandler,
                (IPTR)&((struct __dummyAreaData__ *)(obj))->mad.mad_hiehn);
        }
    }
    else
    {
        ((struct __dummyAreaData__ *)(obj))->mad.mad_hiehn.ehn_Events &= ~flags;
    }
} /* MUI_RejectIDCMP */
