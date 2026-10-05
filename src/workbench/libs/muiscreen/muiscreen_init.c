/*
    Copyright (C) 2009-2025, The AROS Development Team. All rights reserved.

    AmigaOS / SAS/C library init and expunge for muiscreen.library.
    Opens the libraries SAS/C pragmas need as real globals, starts the
    autoclosure task, and tears everything down on expunge.
*/

#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/memory.h>
#include <exec/tasks.h>
#include <exec/execbase.h>
#include <intuition/intuitionbase.h>
#include <graphics/gfxbase.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/iffparse.h>

#include <clib/alib_protos.h>

#include "muiscreen_intern.h"

#define DEBUG 0
#include <aros/debug.h>

#define MUIS_STACKSIZE 4096

#define DOS_MIN_VERSION         37
#define UTILITY_MIN_VERSION     37
#define GRAPHICS_MIN_VERSION    39
#define INTUITION_MIN_VERSION   39
#define IFFPARSE_MIN_VERSION    37

/*
 * Same pattern as muimaster_init.c: pragma libcalls resolve against these
 * globals by identifier, so macros that redirect into the base struct are
 * useless for SAS/C.  Assign both the globals and the struct fields.
 */
struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct DosLibrary *DOSBase;
struct Library *UtilityBase;
struct Library *IFFParseBase;
struct Library *MUIScreenBase;

#define LC_LIBHEADERTYPEPTR struct Library *

void SAVEDS STDARGS LC_BUILDNAME(L_ExpungeLib)(LC_LIBHEADERTYPEPTR _MUIScreenBase);

static void SAVEDS pubscreenClose_Task(void);

/****************************************************************************************/

ULONG SAVEDS STDARGS LC_BUILDNAME(L_InitLib)(LC_LIBHEADERTYPEPTR _MUIScreenBase)
{
    struct MUIScreenBase_intern *libBase;
    struct Task *task;
    APTR stack;

    libBase = (struct MUIScreenBase_intern *)_MUIScreenBase;
    MUIScreenBase = (struct Library *)libBase;

    D(bug("Inside Init func of muiscreen.library\n"));

    if (!(DOSBase = (struct DosLibrary *)OpenLibrary("dos.library", DOS_MIN_VERSION)))
        goto fail;
    libBase->dosbase = DOSBase;

    if (!(UtilityBase = OpenLibrary("utility.library", UTILITY_MIN_VERSION)))
        goto fail;
    libBase->utilitybase = UtilityBase;

    if (!(GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", GRAPHICS_MIN_VERSION)))
        goto fail;
    libBase->gfxbase = GfxBase;

    if (!(IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", INTUITION_MIN_VERSION)))
        goto fail;
    libBase->intuibase = IntuitionBase;

    if (!(IFFParseBase = OpenLibrary("iffparse.library", IFFPARSE_MIN_VERSION)))
        goto fail;
    libBase->iffparsebase = IFFParseBase;

    NewList(&libBase->clients);
    NewList(&libBase->muisb_autocScreens);
    InitSemaphore(&libBase->muisb_acLock);

    libBase->muisb_def = NULL;
    libBase->muisb_taskMsgPort = NULL;
    libBase->muisb_closeTask = NULL;

    task = AllocMem(sizeof(struct Task), MEMF_PUBLIC | MEMF_CLEAR);
    if (task == NULL)
        goto fail;

    NewList(&task->tc_MemEntry);
    task->tc_Node.ln_Type = NT_TASK;
    task->tc_Node.ln_Name = "PUBSCREEN handler";
    task->tc_Node.ln_Pri = 0;
    task->tc_UserData = libBase;

    stack = AllocMem(MUIS_STACKSIZE, MEMF_PUBLIC);
    if (stack == NULL)
    {
        FreeMem(task, sizeof(struct Task));
        goto fail;
    }

    /* Classic Amiga: stacks grow downwards; SP starts at the upper end. */
    task->tc_SPLower = stack;
    task->tc_SPUpper = (BYTE *)stack + MUIS_STACKSIZE;
    task->tc_SPReg = task->tc_SPUpper;

    Forbid();
    if (AddTask(task, pubscreenClose_Task, NULL) == NULL)
    {
        Permit();
        FreeMem(stack, MUIS_STACKSIZE);
        FreeMem(task, sizeof(struct Task));
        goto fail;
    }
    libBase->muisb_closeTask = task;
    Permit();

    return TRUE;

fail:
    L_ExpungeLib(_MUIScreenBase);
    return FALSE;
}

/****************************************************************************************/

static void SAVEDS pubscreenClose_Task(void)
{
    struct Task *thistask;
    struct MUIScreenBase_intern *MUIScreenBase;
    ULONG signals;
    ULONG sigs;
    struct Node *autocNode;
    struct Node *tmp;
    struct Screen *screen;

    thistask = FindTask(NULL);
    MUIScreenBase = (struct MUIScreenBase_intern *)thistask->tc_UserData;

    D(bug("[MUIScreen] close task thisTask=%lx base=%lx\n",
        (ULONG)thistask, (ULONG)MUIScreenBase));

    if ((MUIScreenBase->muisb_taskMsgPort = CreateMsgPort()) != NULL)
    {
        signals = (1L << MUIScreenBase->muisb_taskMsgPort->mp_SigBit) | SIGBREAKF_CTRL_C;
        do
        {
            sigs = Wait(signals);
            if (sigs & (1L << MUIScreenBase->muisb_taskMsgPort->mp_SigBit))
            {
                D(bug("[MUIScreen] msgport signal received\n"));

                ObtainSemaphore(&MUIScreenBase->muisb_acLock);
                ForeachNodeSafe(&MUIScreenBase->muisb_autocScreens, autocNode, tmp)
                {
                    screen = (struct Screen *)autocNode->ln_Name;
                    if (screen != NULL && screen->FirstWindow == NULL)
                    {
                        D(bug("[MUIScreen] closing Screen @ %lx\n", (ULONG)screen));
                        Remove(autocNode);
                        CloseScreen(screen);
                        FreeVec(autocNode);
                    }
                }
                ReleaseSemaphore(&MUIScreenBase->muisb_acLock);
            }
            if (sigs & SIGBREAKF_CTRL_C)
                break;
        } while (1);

        DeleteMsgPort(MUIScreenBase->muisb_taskMsgPort);
        MUIScreenBase->muisb_taskMsgPort = NULL;
    }

    MUIScreenBase->muisb_closeTask = NULL;
}

/****************************************************************************************/

void SAVEDS STDARGS LC_BUILDNAME(L_ExpungeLib)(LC_LIBHEADERTYPEPTR _MUIScreenBase)
{
    struct MUIScreenBase_intern *libBase;
    LONG waits;

    libBase = (struct MUIScreenBase_intern *)_MUIScreenBase;

    if (libBase->muisb_closeTask != NULL)
    {
        Signal(libBase->muisb_closeTask, SIGBREAKF_CTRL_C);
        /*
         * Brief wait so the handler can DeleteMsgPort and clear
         * muisb_closeTask before we tear down the library base.
         */
        for (waits = 0; libBase->muisb_closeTask != NULL && waits < 50; waits++)
        {
            if (DOSBase != NULL)
                Delay(1);
        }
    }

    if (libBase->iffparsebase)
    {
        CloseLibrary(libBase->iffparsebase);
        libBase->iffparsebase = NULL;
        IFFParseBase = NULL;
    }
    if (libBase->intuibase)
    {
        CloseLibrary((struct Library *)libBase->intuibase);
        libBase->intuibase = NULL;
        IntuitionBase = NULL;
    }
    if (libBase->gfxbase)
    {
        CloseLibrary((struct Library *)libBase->gfxbase);
        libBase->gfxbase = NULL;
        GfxBase = NULL;
    }
    if (libBase->utilitybase)
    {
        CloseLibrary(libBase->utilitybase);
        libBase->utilitybase = NULL;
        UtilityBase = NULL;
    }
    if (libBase->dosbase)
    {
        CloseLibrary((struct Library *)libBase->dosbase);
        libBase->dosbase = NULL;
        DOSBase = NULL;
    }

    MUIScreenBase = NULL;
}
