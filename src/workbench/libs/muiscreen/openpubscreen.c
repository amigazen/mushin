/*
    Copyright (C) 2009-2025, The AROS Development Team. All rights reserved.
*/

#include <libraries/muiscreen.h>
#include <utility/hooks.h>
#include <intuition/screens.h>
#include <proto/intuition.h>
#include <proto/exec.h>
#include <exec/lists.h>
#include <exec/memory.h>

#define DEBUG 0
#include <aros/debug.h>

#include "muiscreen_intern.h"

/*****************************************************************************

    NAME */
        __ASM__ __SAVE_DS__ char *MUIS_OpenPubScreen(
            __REG__(a0, struct MUI_PubScreenDesc *desc),
            __REG__(a6, struct MUIScreenBase_intern *MUIScreenBase))

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
    struct Node *autocNode;
    struct Hook *backfillHook;
    char *ret;
    struct Screen *screen;
    struct Node *node;
    struct MUIS_InfoClient *client;

    backfillHook = NULL;
    ret = NULL;
    autocNode = NULL;

    D(bug("[MUIScreen] MUIS_OpenPubScreen(%lx) name %s\n",
        (ULONG)desc, desc ? desc->Name : "(null)"));

    if (desc == NULL)
        return NULL;

    /* TODO desc->CloseGadget */
    /* TODO desc->SystemPens */
    /* TODO desc->Palette */

    if (desc->AutoClose && MUIScreenBase->muisb_taskMsgPort)
        autocNode = AllocVec(sizeof(struct Node), MEMF_CLEAR);

    screen = OpenScreenTags(NULL,
            SA_Type, (ULONG)PUBLICSCREEN,
            SA_PubName, (ULONG)desc->Name,
            SA_Title, (ULONG)desc->Title,
            (backfillHook) ? SA_BackFill : TAG_IGNORE,
                (ULONG)backfillHook,
            (autocNode) ? SA_PubTask : TAG_IGNORE,
                (ULONG)MUIScreenBase->muisb_closeTask,
            (autocNode) ? SA_PubSig : TAG_IGNORE,
                (ULONG)((autocNode) ? MUIScreenBase->muisb_taskMsgPort->mp_SigBit : -1),
            SA_DisplayID, (ULONG)desc->DisplayID,
            SA_Width, (ULONG)desc->DisplayWidth,
            SA_Height, (ULONG)desc->DisplayHeight,
            SA_Depth, (ULONG)desc->DisplayDepth,
            SA_Overscan, (ULONG)desc->OverscanType,
            SA_AutoScroll, (ULONG)desc->AutoScroll,
            SA_Draggable, (ULONG)!desc->NoDrag,
            SA_Exclusive, (ULONG)desc->Exclusive,
            SA_Interleaved, (ULONG)desc->Interleaved,
            SA_Behind, (ULONG)desc->Behind,
            TAG_DONE);

    if (screen)
    {
        if ((PubScreenStatus(screen, 0) & 1) == 0)
        {
            D(bug("[MUIScreen] Can't make screen public\n"));
            if (autocNode)
                FreeVec(autocNode);
            CloseScreen(screen);
        }
        else
        {
            ret = desc->Name;

            if (autocNode)
            {
                autocNode->ln_Name = (char *)screen;
                ObtainSemaphore(&MUIScreenBase->muisb_acLock);
                AddTail(&MUIScreenBase->muisb_autocScreens, autocNode);
                ReleaseSemaphore(&MUIScreenBase->muisb_acLock);
            }

            if (desc->SysDefault)
            {
                MUIScreenBase->muisb_def = ret;
                SetDefaultPubScreen((UBYTE *)MUIScreenBase->muisb_def);
            }

            ForeachNode(&MUIScreenBase->clients, node)
            {
                client = (struct MUIS_InfoClient *)node;
                Signal(client->task, client->sigbit);
            }
        }
    }
    else
    {
        if (autocNode)
            FreeVec(autocNode);
    }

    return ret;
}
