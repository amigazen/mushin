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

extern struct Library *MUIMasterBase;

/* Stack-safe; do not call the LVO form of MUI_FreeClass from here. */
extern VOID ZUNE_FreeClass(Class *cl);

/*****************************************************************************

    NAME */
        __asm __saveds struct MUI_CustomClass *MUI_CreateCustomClass(register __a0 struct Library *base, register __a1 ClassID supername, register __a2 struct MUI_CustomClass *supermcc, register __d0 ULONG datasize, register __a3 APTR dispatcher)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS
        Resolve the superclass with ZUNE_GetBuiltinClass /
        ZUNE_GetExternalClass (normal C calls).  Calling MUI_GetClass()
        from here is unsafe under SAS/C: that LVO expects classid in A0,
        but A0 still holds 'base' from this function's entry, so every
        subclass create returned NULL (prefs "Out of memory (20).",
        Voyager CreateCustomClass).

*****************************************************************************/
{
    struct MUI_CustomClass *mcc;
    struct IClass *cl;
    struct IClass *super;
    ClassID id;

    id = NULL;
    super = NULL;
    mcc = NULL;
    cl = NULL;

    if ((supername == NULL) && (supermcc == NULL))
    {
        ZuneTrace("zune: CreateCustomClass no supername/supermcc\n");
        return NULL;
    }

    if (!supermcc)
    {
        /*
         * Same resolution order as MUI_GetClass, but stack-safe.
         */
        super = ZUNE_GetBuiltinClass(supername, MUIMasterBase);
        if (!super)
            super = ZUNE_GetExternalClass(supername, MUIMasterBase);
        if (!super)
        {
            ZuneTrace("zune: CreateCustomClass GetClass failed super=%s\n",
                supername ? supername : (CONST_STRPTR) "(null)");
            return NULL;
        }
    }
    else
        super = supermcc->mcc_Class;

    if (!(mcc = mui_alloc_struct(struct MUI_CustomClass)))
    {
        ZuneTrace("zune: CreateCustomClass mcc alloc failed\n");
        if (!supermcc)
            ZUNE_FreeClass(super);
        return NULL;
    }

    if (base)
        id = FilePart(((struct Node *)base)->ln_Name);

    if (!(cl = MakeClass(id, NULL, super, datasize, 0)))
    {
        ZuneTrace("zune: CreateCustomClass MakeClass failed super=%s\n",
            supername ? supername : (CONST_STRPTR) "(null)");
        mui_free(mcc);
        if (!supermcc)
            ZUNE_FreeClass(super);
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

    ZuneTrace("zune: CreateCustomClass ok super=%s mcc=%lx cl=%lx\n",
        supername ? supername : (CONST_STRPTR) "(via supermcc)",
        (ULONG) mcc, (ULONG) cl);

    return mcc;
} /* MUI_CreateCustomClass */
