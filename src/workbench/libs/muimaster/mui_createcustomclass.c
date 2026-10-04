/*
    Copyright (C) 2002-2007, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/dos.h>
#include <proto/utility.h>

#include "mui.h"
#include "muimaster_intern.h"
#include "support.h"
#include "support_classes.h"

/* Forward declarations for internal functions */
struct IClass *MUI_GetClass(CONST_STRPTR classid);
VOID MUI_FreeClass(Class *cl);

/*****************************************************************************

    NAME */
        MUI_LIB_ENTRY struct MUI_CustomClass *MUI_CreateCustomClass(MUI_LIB_ARG(a0, struct Library *base), MUI_LIB_ARG(a1, CONST_STRPTR supername), MUI_LIB_ARG(a2, struct MUI_CustomClass *supermcc), MUI_LIB_ARG(d0, ULONG datasize), MUI_LIB_ARG(a3, APTR dispatcher))

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

    struct MUI_CustomClass *mcc;
    struct IClass        *cl, *super;
    ClassID                 id = NULL;

    if ((supername == NULL) && (supermcc == NULL))
        return NULL;

    if (!supermcc)
    {
        super = MUI_GetClass(supername);
        if (!super) return NULL;
    }
    else super = supermcc->mcc_Class;

    if (!(mcc = mui_alloc_struct(struct MUI_CustomClass)))
    {
        if (!supermcc)
            MUI_FreeClass(super);
        return NULL;
    }

    if (base)
        id = FilePart(((struct Node *)base)->ln_Name);

    if (!(cl = MakeClass(id, NULL, super, datasize, 0)))
    {
        mui_free(mcc);
        if (!supermcc)
            MUI_FreeClass(super);
        return NULL;
    }

    mcc->mcc_UtilityBase   = (struct Library *)UtilityBase;
    mcc->mcc_DOSBase       = (struct Library *)DOSBase;
    mcc->mcc_GfxBase       = (struct Library *)GfxBase;
    mcc->mcc_IntuitionBase = (struct Library *)IntuitionBase;

    mcc->mcc_Class  = cl;
    mcc->mcc_Super  = super;
    mcc->mcc_Module = NULL; /* _zune_class_load() will set this */

#if defined(__MAXON__) || defined(__amigaos4__)
    cl->cl_Dispatcher.h_Entry    = (HOOKFUNC)dispatcher;
#else
    cl->cl_Dispatcher.h_Entry    = (HOOKFUNC)metaDispatcher;
    cl->cl_Dispatcher.h_SubEntry = (HOOKFUNC)dispatcher;
#endif
    cl->cl_Dispatcher.h_Data     = base;

    return mcc;
    
} /* MUI_CreateCustomClass */
