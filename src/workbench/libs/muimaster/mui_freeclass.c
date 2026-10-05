/*
    Copyright (C) 2002-2007, The AROS Development Team. All rights reserved.
*/

#include <string.h>

#include <proto/intuition.h>
#include <proto/exec.h>

#include "muimaster_intern.h"
#include "support_classes.h"
#include "support.h"
#include "debug.h"

extern struct Library *MUIMasterBase;
typedef struct MUIMasterBase_intern MUIMasterBase_intern;

/*
 * Stack-callable FreeClass body.  Library code must use this (or an LVO
 * trampoline that loads A0), never a direct C call to MUI_FreeClass().
 */
VOID ZUNE_FreeClass(Class *cl)
{
    STRPTR id;
    ULONG flags;
    ULONG ud;
    ULONG hdata;
    ULONG opencnt;
    struct Library *lib;

    if (cl == NULL)
        return;

    id = (STRPTR) "?";
    flags = cl->cl_Flags;
    ud = cl->cl_UserData;
    hdata = (ULONG) cl->cl_Dispatcher.h_Data;
    opencnt = MUIMasterBase ? MUIMasterBase->lib_OpenCnt : 0;
    if (cl->cl_ID != NULL)
        id = cl->cl_ID;

    ZuneTrace("zune: FreeClass cl=%lx id=%s flags=%lx ud=%ld hdata=%lx opencnt=%ld\n",
        (ULONG) cl, id, flags, ud, hdata, opencnt);

    ObtainSemaphore(&((struct MUIMasterBase_intern *)MUIMasterBase)->ZuneSemaphore);

    /* CLF_INLIST tells us that this class is a builtin class */
    if (cl->cl_Flags & CLF_INLIST)
    {
        /*
         * Builtin classes stay until library expunge.  UserData==0 used to
         * FreeClass() the class and then MUI_FreeClass(cl_Super).  Notify
         * ObjectCount does not include Application/Window instances, so
         * FreeClass(Notify) succeeded while those objects were still in
         * OM_DISPOSE - PC then jumped into the freed class (data).
         */
        if (cl->cl_UserData > 0)
            cl->cl_UserData--;

        ReleaseSemaphore(&((struct MUIMasterBase_intern *)MUIMasterBase)->ZuneSemaphore);
        ZuneTrace("zune: FreeClass INLIST done cl=%lx ud=%ld\n",
            (ULONG) cl, cl->cl_UserData);
    }
    else
    {
        /*
         * External MCC: h_Data is the MCC library base (set by
         * MUI_CreateCustomClass).  Always CloseLibrary once per
         * GetExternalClass OpenLibrary so OpenCnt stays balanced.
         * cl_UserData is the GetClass count (for traces / parity with
         * builtins); do not gate CloseLibrary on it or OpenCnt leaks
         * and Expunge never runs.
         *
         * Builtin dispatchers stash MUIMasterBase in h_Data for A6 -
         * never CloseLibrary that.  Also reject non-Library pointers.
         */
        if (cl->cl_UserData > 0)
            cl->cl_UserData--;

        lib = (struct Library *)cl->cl_Dispatcher.h_Data;

        ReleaseSemaphore(&((struct MUIMasterBase_intern *)MUIMasterBase)->ZuneSemaphore);

        if (lib != NULL && lib != MUIMasterBase
            && lib->lib_Node.ln_Type == NT_LIBRARY)
        {
            ZuneTrace("zune: FreeClass CloseLibrary %lx (ud now %ld)\n",
                (ULONG) lib, cl->cl_UserData);
            CloseLibrary(lib);
        }
        else
        {
            ZuneTrace("zune: FreeClass skip CloseLibrary hdata=%lx ud=%ld type=%ld\n",
                (ULONG) lib, cl->cl_UserData,
                (ULONG) (lib ? lib->lib_Node.ln_Type : 0));
        }
    }
    ZuneTrace("zune: FreeClass return cl=%lx\n", (ULONG) cl);
}

/*****************************************************************************

    NAME */
        __asm __saveds VOID MUI_FreeClass(register __a0 Class *cl)

/*  FUNCTION
        Frees a class returned by MUI_GetClass(). This function is
        obsolete. Use MUI_DeleteCustomClass() instead.

    INPUTS
        cl - The pointer to the class.

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
        MUI_GetClass(), MUI_CreateCustomClass(), MUI_DeleteCustomClass()

    INTERNALS

*****************************************************************************/
{
    ZUNE_FreeClass(cl);
} /* MUI_FreeClass */
