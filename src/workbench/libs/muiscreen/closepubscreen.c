/*
    Copyright (C) 2009-2025, The AROS Development Team. All rights reserved.
*/

#include <libraries/muiscreen.h>
#include <proto/intuition.h>
#include <intuition/screens.h>
#include <proto/exec.h>
#include <exec/lists.h>
#include <exec/memory.h>
#include <string.h>

#define DEBUG 0
#include <aros/debug.h>

#include "muiscreen_intern.h"

/*****************************************************************************

    NAME */
        __ASM__ __SAVE_DS__ BOOL MUIS_ClosePubScreen(
            __REG__(a0, char *name),
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
    struct List *pubscrlist;
    struct PubScreenNode *pubscrnode;
    BOOL found;
    BOOL retval;
    struct Node *node;
    struct Node *tmpnode;
    struct MUIS_InfoClient *client;

    found = FALSE;
    retval = FALSE;

    D(bug("MUIS_ClosePubScreen(%s)\n", name ? name : "(null)"));

    if (name == NULL)
        return FALSE;

    pubscrlist = LockPubScreenList();
    ForeachNode(pubscrlist, pubscrnode)
    {
        if (strcmp(pubscrnode->psn_Node.ln_Name, name) == 0)
        {
            found = TRUE;
            break;
        }
    }
    UnlockPubScreenList();

    if (MUIScreenBase->muisb_def && !strcmp(MUIScreenBase->muisb_def, name))
        SetDefaultPubScreen("");

    if (found)
    {
        PubScreenStatus(pubscrnode->psn_Screen, PSNF_PRIVATE);
        ObtainSemaphore(&MUIScreenBase->muisb_acLock);
        ForeachNodeSafe(&MUIScreenBase->muisb_autocScreens, node, tmpnode)
        {
            if (node->ln_Name == (char *)pubscrnode->psn_Screen)
            {
                Remove(node);
                FreeVec(node);
                break;
            }
        }
        ReleaseSemaphore(&MUIScreenBase->muisb_acLock);
        CloseScreen(pubscrnode->psn_Screen);

        ForeachNode(&MUIScreenBase->clients, node)
        {
            client = (struct MUIS_InfoClient *)node;
            Signal(client->task, client->sigbit);
        }

        retval = TRUE;
    }

    return retval;
}
