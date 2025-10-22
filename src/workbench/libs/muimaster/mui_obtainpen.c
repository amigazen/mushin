/*
    Copyright (C) 2002-2012, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <proto/graphics.h>

#include <string.h>
#include <stdlib.h>

#include "mui.h"
#include "penspec.h"
#include "muimaster_intern.h"

/*****************************************************************************

    NAME */
        __asm __saveds LONG MUI_ObtainPen(register __a0 struct MUI_RenderInfo *mri, register __a1 struct MUI_PenSpec *spec, register __d0 ULONG flags)

/*  FUNCTION
        Turns struct MUI_PenSpec (the result of a Poppen object) into
        a pen for SetAPen().

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
        MUI_ReleasePen()

    INTERNALS

*****************************************************************************/
{
    LONG retval = -1;
    struct MUI_PenSpec_intern intern;
    
    if (!spec || !mri || !mri->mri_Colormap) return -1;

    if (!zune_pen_spec_to_intern(spec, &intern))
        return -1;
    if (!zune_penspec_setup(&intern, mri))
        return -1;
    retval = intern.p_pen;
    if ((retval != -1) && (intern.p_is_allocated))
    {
        /* flag to indicate that ReleasePen() needs to be called
           in MUI_ReleasePen() */
                       
        retval |= 0x10000;
    }

    return retval;
} /* MUI_ObtainPen */
