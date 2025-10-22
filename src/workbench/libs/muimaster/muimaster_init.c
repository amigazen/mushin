/*
    Copyright (C) 2002, The AROS Development Team.
    All rights reserved.
*/

#include <exec/types.h>
#include <exec/libraries.h>
#include <rexx/storage.h>

/* Use proto includes for function prototypes and library base externs */
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/layers.h>
#include <proto/commodities.h>
#include <proto/rexxsyslib.h>
#include <proto/gadtools.h>
#include <proto/keymap.h>
#include <proto/datatypes.h>
#include <proto/iffparse.h>
#include <proto/diskfont.h>
#include <proto/icon.h>
#include <proto/wb.h>
#include <proto/muiscreen.h>
#include <proto/cybergraphics.h>

#include <clib/alib_protos.h>

#include "muimaster_intern.h"
#include "mui.h"

/* Typedef for cleaner casting */
typedef struct MUIMasterBase_intern MUIMasterBase_intern;

/****************************************************************************************/

/* #define MYDEBUG 1 */
#include "debug.h"

struct Library *MUIMasterBase;
struct Library **MUIMasterBasePtr = &MUIMasterBase;

/* Library version constants for clarity */
#define DOS_MIN_VERSION         37
#define UTILITY_MIN_VERSION     37
#define GRAPHICS_MIN_VERSION    39
#define INTUITION_MIN_VERSION   39
#define ASL_MIN_VERSION         37
#define LAYERS_MIN_VERSION      37
#define COMMODITIES_MIN_VERSION 37
#define REXXSYS_MIN_VERSION     37
#define GADTOOLS_MIN_VERSION    37
#define KEYMAP_MIN_VERSION      37
#define DATATYPES_MIN_VERSION   37
#define IFFPARSE_MIN_VERSION    37
#define DISKFONT_MIN_VERSION    37
#define ICON_MIN_VERSION        37
#define WORKBENCH_MIN_VERSION   37

/****************************************************************************************/

ULONG SAVEDS STDARGS LC_BUILDNAME(L_InitLib)(LC_LIBHEADERTYPEPTR _MUIMasterBase)
{
    /* C89 COMPATIBILITY: All declarations must be at the top of the function block. */
    MUIMasterBase_intern *libBase = (MUIMasterBase_intern *)_MUIMasterBase;

    D(bug("Inside Init func of muimaster.library\n"));

    *MUIMasterBasePtr = (struct Library *)libBase;
    MUIMasterBase = (struct Library *)libBase;

    /*
     * Open required libraries.
     * NOTE: Casts are removed from OpenLibrary() calls to avoid macro expansion
     * conflicts with types like 'UtilityBase', 'GfxBase', etc. The compiler
     * will correctly handle assigning the 'struct Library *' return value.
    */
    if (!(libBase->dosbase = (struct DosLibrary *)OpenLibrary("dos.library", DOS_MIN_VERSION))) goto fail;
    if (!(libBase->utilitybase = (struct UtilityBase *)OpenLibrary("utility.library", UTILITY_MIN_VERSION))) goto fail;
    if (!(libBase->gfxbase = (struct GfxBase *)OpenLibrary("graphics.library", GRAPHICS_MIN_VERSION))) goto fail;
    if (!(libBase->intuibase = (struct IntuitionBase *)OpenLibrary("intuition.library", INTUITION_MIN_VERSION))) goto fail;

    if (!(AslBase = OpenLibrary("asl.library", ASL_MIN_VERSION))) goto fail;
    if (!(LayersBase = OpenLibrary("layers.library", LAYERS_MIN_VERSION))) goto fail;
    if (!(CxBase = OpenLibrary("commodities.library", COMMODITIES_MIN_VERSION))) goto fail;
    if (!(RexxSysBase = (struct RxsLib *)OpenLibrary("rexxsyslib.library", REXXSYS_MIN_VERSION))) goto fail;
    if (!(GadToolsBase = OpenLibrary("gadtools.library", GADTOOLS_MIN_VERSION))) goto fail;
    if (!(KeymapBase = OpenLibrary("keymap.library", KEYMAP_MIN_VERSION))) goto fail;
    if (!(DataTypesBase = OpenLibrary("datatypes.library", DATATYPES_MIN_VERSION))) goto fail;
    if (!(IFFParseBase = OpenLibrary("iffparse.library", IFFPARSE_MIN_VERSION))) goto fail;
    if (!(DiskfontBase = OpenLibrary("diskfont.library", DISKFONT_MIN_VERSION))) goto fail;
    if (!(IconBase = OpenLibrary("icon.library", ICON_MIN_VERSION))) goto fail;
    if (!(WorkbenchBase = OpenLibrary("workbench.library", WORKBENCH_MIN_VERSION))) goto fail;
    if (!(MUIScreenBase = OpenLibrary("muiscreen.library", 0))) goto fail;

    /* Optional libraries: continue even if they are not available */
    CyberGfxBase = OpenLibrary("cybergraphics.library", 0);
#ifdef HAVE_COOLIMAGES
    CoolImagesBase = OpenLibrary("coolimages.library", 0);
#endif

    /* Initialize internal lists and semaphores */
    InitSemaphore(&libBase->ZuneSemaphore);
    NewList((struct List *)&libBase->BuiltinClasses);
    NewList((struct List *)&libBase->Applications);

    return TRUE;

fail:
    D(bug("muimaster.library Init FAILED. Expunging open libraries.\n"));
    LC_BUILDNAME(L_ExpungeLib)(_MUIMasterBase);
    return FALSE;
}

/****************************************************************************************/

ULONG SAVEDS STDARGS LC_BUILDNAME(L_OpenLib)(LC_LIBHEADERTYPEPTR MUIMasterBase)
{
    D(bug("Inside Open func of muimaster.library\n"));
    MUIMasterBase->lib_OpenCnt++;
    return TRUE;
}

/****************************************************************************************/

void SAVEDS STDARGS LC_BUILDNAME(L_CloseLib)(LC_LIBHEADERTYPEPTR MUIMasterBase)
{
    D(bug("Inside Close func of muimaster.library\n"));
    MUIMasterBase->lib_OpenCnt--;
}

/****************************************************************************************/

void SAVEDS STDARGS LC_BUILDNAME(L_ExpungeLib)(LC_LIBHEADERTYPEPTR _MUIMasterBase)
{
    /* C89 COMPATIBILITY: Declaration at top of the block. */
    MUIMasterBase_intern *libBase = (MUIMasterBase_intern *)_MUIMasterBase;

    D(bug("Inside Expunge func of muimaster.library\n"));

    /* Close libraries stored in the internal base struct */
    CloseLibrary((struct Library *)libBase->gfxbase);
    CloseLibrary((struct Library *)libBase->utilitybase);
    CloseLibrary((struct Library *)libBase->dosbase);
    CloseLibrary((struct Library *)libBase->intuibase);

    /* Close libraries stored as global variables */
    CloseLibrary(AslBase);
    CloseLibrary(LayersBase);
    CloseLibrary(CxBase);
    CloseLibrary((struct Library *)RexxSysBase);
    CloseLibrary(GadToolsBase);
    CloseLibrary(KeymapBase);
    CloseLibrary(DataTypesBase);
    CloseLibrary(IFFParseBase);
    CloseLibrary(DiskfontBase);
    CloseLibrary(IconBase);
    CloseLibrary(CyberGfxBase);
    CloseLibrary(WorkbenchBase);
    CloseLibrary(MUIScreenBase);
#ifdef HAVE_COOLIMAGES
    CloseLibrary(CoolImagesBase);
#endif
}
/****************************************************************************************/