/*
    Copyright (C) 2002-2007, The AROS Development Team. All rights reserved.
*/

#include <string.h>

#include <proto/intuition.h>
#include <proto/exec.h>

#include "muimaster_intern.h"
#include "support_classes.h"
#include "debug.h"

extern struct Library *MUIMasterBase;
typedef struct MUIMasterBase_intern MUIMasterBase_intern;

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
    ObtainSemaphore(&((struct MUIMasterBase_intern *)MUIMasterBase)->ZuneSemaphore);

    /* CLF_INLIST tells us that this class is a builtin class */
    if (cl->cl_Flags & CLF_INLIST)
    {
        Class *super = cl->cl_Super;
        char *count = (char *)cl->cl_Dispatcher.h_Data;

        if (--count == 0)
        {
              ZUNE_RemoveBuiltinClass(cl, MUIMasterBase);

            if (FreeClass(cl))
            {
                CloseLibrary(MUIMasterBase);
                if (strcmp(super->cl_ID, ROOTCLASS) != 0)
                    MUI_FreeClass(super);
            }
            else
            {
                /* Re-add the class to the list since freeing it failed */
                ZUNE_AddBuiltinClass(cl, MUIMasterBase);

                /* And also increase the reference counter again */
                count++;
            }
        }
        cl->cl_Dispatcher.h_Data = count;

        ReleaseSemaphore(&((struct MUIMasterBase_intern *)MUIMasterBase)->ZuneSemaphore);
    }
    else
    {
        ReleaseSemaphore(&((struct MUIMasterBase_intern *)MUIMasterBase)->ZuneSemaphore);

        CloseLibrary((struct Library *)cl->cl_Dispatcher.h_Data);
    }
} /* MUI_FreeClass */
