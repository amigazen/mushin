/*
    Copyright (C) 2002, The AROS Development Team.
    All rights reserved.
*/

#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/memory.h>
#include <rexx/storage.h>
#include <intuition/screens.h>
#include <graphics/text.h>

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
#include <proto/asl.h>
#include <proto/icon.h>
#include <proto/wb.h>
#include <proto/muiscreen.h>
#include <proto/cybergraphics.h>

#include <clib/alib_protos.h>

#include "muimaster_intern.h"
#include "mui.h"

/* Typedef for cleaner casting */
typedef struct MUIMasterBase_intern MUIMasterBase_intern;

/*
 * Library base globals.
 *
 * These MUST exist as real global variables, not as macros redirecting into
 * struct MUIMasterBase_intern the way the AROS/GCC build does it.
 *
 * Reason: on SAS/C every call into another library is generated from a
 * "#pragma libcall <base> <name> <offset> <regmask>" line.  The base operand
 * of that pragma is recorded as an *identifier* when all.gst is built from
 * headers.c, long before any of this library's own headers are seen.  At the
 * call site the compiler resolves that identifier through its normal symbol
 * table - the preprocessor has already finished, so a "#define GfxBase
 * (mb->gfxbase)" macro is invisible to it and the emitted code still does
 * "move.l _GfxBase,a6".  A macro therefore only rewrites explicit source-level
 * dereferences and silently leaves every pragma-generated call pointing at the
 * (NULL) global, which is why those macros were useless here and had to be
 * replaced by real globals that we assign in L_InitLib below.
 *
 * The types must match the declarations the SAS/C proto headers already put
 * into all.gst, otherwise the definition collides with the extern (Error 72).
 * Note UtilityBase is "struct Library *" on SAS/C, unlike AROS/GCC where it is
 * "struct UtilityBase *".
 */
struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct DosLibrary *DOSBase;
struct Library *UtilityBase;

struct Library *MUIScreenBase;
struct Library *AslBase;
struct Library *LayersBase;
struct Library *CxBase;
struct RxsLib *RexxSysBase;
struct Library *GadToolsBase;
struct Library *KeymapBase;
struct Library *LocaleBase;
struct Library *DataTypesBase;
struct Library *IFFParseBase;
struct Library *DiskfontBase;
struct Library *IconBase;
struct Library *WorkbenchBase;
struct Library *CoolImagesBase;

#define LC_LIBHEADERTYPEPTR struct Library *

void SAVEDS STDARGS LC_BUILDNAME(L_ExpungeLib)(LC_LIBHEADERTYPEPTR _MUIMasterBase);

/****************************************************************************************/

/* #define MYDEBUG 1 */
#include "debug.h"

struct Library *MUIMasterBase;
struct Library **MUIMasterBasePtr = &MUIMasterBase;
struct Library *CyberGfxBase;

#ifdef MUSHIN_GCC_NATIVE
BOOL ZuneGccInit(void);
void ZuneGccCleanup(void);
#endif

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
#define LOCALE_MIN_VERSION      38
#define DATATYPES_MIN_VERSION   37
#define IFFPARSE_MIN_VERSION    37
#define DISKFONT_MIN_VERSION    37
#define ICON_MIN_VERSION        37
#define WORKBENCH_MIN_VERSION   37

static const struct TextAttr topaz8Attr =
    { "topaz.font", 8, FS_NORMAL, FPF_ROMFONT };

/****************************************************************************************/

/*
 * Build a PST_SYS pen spec ("s" followed by a decimal DrawInfo pen number)
 * straight into spec->buf.
 *
 * Done by hand rather than with snprintf() because the pen specs have to be
 * exactly the ascii form that MUI_ObtainPen and third-party classes such as
 * NList expect, and because RawDoFmt-based formatting treats %d as 16-bit.
 */
static void init_syspen(struct MUI_PenSpec *spec, LONG dripen)
{
    UBYTE digits[8];
    LONG value = dripen;
    int n = 0;
    int i = 0;

    spec->buf[i++] = (UBYTE) PST_SYS;

    if (value <= 0)
    {
        digits[n++] = (UBYTE) '0';
    }
    else
    {
        while ((value > 0) && (n < (int)sizeof(digits)))
        {
            digits[n++] = (UBYTE) ('0' + (value % 10));
            value /= 10;
        }
    }

    while (n > 0)
        spec->buf[i++] = digits[--n];

    spec->buf[i] = (UBYTE) '\0';
}

/****************************************************************************************/

/* Called from LibInit in zunemaster_lib.c via normal C calling convention
   (STDARGS).  Do not use register __a0 here; the base arrives on the stack. */
ULONG SAVEDS STDARGS LC_BUILDNAME(L_InitLib)(LC_LIBHEADERTYPEPTR _MUIMasterBase)
{
    /* C89 COMPATIBILITY: All declarations must be at the top of the function block. */
    MUIMasterBase_intern *libBase = (MUIMasterBase_intern *)_MUIMasterBase;
    struct MUI_PenSpec *pens;

    D(bug("Inside Init func of muimaster.library\n"));

    *MUIMasterBasePtr = (struct Library *)libBase;
    MUIMasterBase = (struct Library *)libBase;

    /*
     * Each base is stored twice on purpose: into the global that the SAS/C
     * pragmas generate their "move.l _XxxBase,a6" against, and into the
     * matching struct MUIMasterBase_intern field.  The struct copies are what
     * MUI_CreateCustomClass() hands to third-party classes in
     * mcc_IntuitionBase/mcc_GfxBase/mcc_DOSBase/mcc_UtilityBase, and what
     * L_ExpungeLib() closes; the globals are what this library's own code
     * actually calls through.  Keeping only one of the two was the cause of
     * every intuition/graphics call in the library jumping through a NULL A6.
     */
    if (!(DOSBase = (struct DosLibrary *)OpenLibrary("dos.library", DOS_MIN_VERSION)))
        goto fail;
    libBase->dosbase = DOSBase;

    /*
     * utility.library must come up early: SCOPTIONS selects UTILITYLIBRARY, so
     * the compiler routes 32-bit multiply/divide through _UtilityBase.
     */
    if (!(UtilityBase = OpenLibrary("utility.library", UTILITY_MIN_VERSION)))
        goto fail;
    libBase->utilitybase = (struct UtilityBase *)UtilityBase;

    if (!(GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", GRAPHICS_MIN_VERSION)))
        goto fail;
    libBase->gfxbase = GfxBase;

    if (!(IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", INTUITION_MIN_VERSION)))
        goto fail;
    libBase->intuibase = IntuitionBase;

    if (!(AslBase = OpenLibrary("asl.library", ASL_MIN_VERSION)))
        goto fail;
    libBase->aslbase = AslBase;

    if (!(LayersBase = OpenLibrary("layers.library", LAYERS_MIN_VERSION)))
        goto fail;
    libBase->layersbase = LayersBase;

    if (!(CxBase = OpenLibrary("commodities.library", COMMODITIES_MIN_VERSION)))
        goto fail;
    libBase->cxbase = CxBase;

    if (!(RexxSysBase = (struct RxsLib *)OpenLibrary("rexxsyslib.library", REXXSYS_MIN_VERSION)))
        goto fail;
    libBase->rxsbase = RexxSysBase;

    if (!(GadToolsBase = OpenLibrary("gadtools.library", GADTOOLS_MIN_VERSION)))
        goto fail;
    libBase->gadtoolsbase = GadToolsBase;

    if (!(KeymapBase = OpenLibrary("keymap.library", KEYMAP_MIN_VERSION)))
        goto fail;
    libBase->keymapbase = KeymapBase;

    /* String.mui OpenLocale/IsPrint; without this LocaleBase is NULL and
     * the first typed character jumps through a NULL libcall and locks. */
    if (!(LocaleBase = OpenLibrary("locale.library", LOCALE_MIN_VERSION)))
        goto fail;

    if (!(DataTypesBase = OpenLibrary("datatypes.library", DATATYPES_MIN_VERSION)))
        goto fail;

    if (!(IFFParseBase = OpenLibrary("iffparse.library", IFFPARSE_MIN_VERSION)))
        goto fail;
    libBase->iffparsebase = IFFParseBase;

    if (!(DiskfontBase = OpenLibrary("diskfont.library", DISKFONT_MIN_VERSION)))
        goto fail;
    libBase->diskfontbase = DiskfontBase;

    if (!(IconBase = OpenLibrary("icon.library", ICON_MIN_VERSION)))
        goto fail;
    libBase->iconbase = IconBase;

    if (!(WorkbenchBase = OpenLibrary("workbench.library", WORKBENCH_MIN_VERSION)))
        goto fail;
    libBase->workbenchbase = WorkbenchBase;

    /*
     * Do not OpenLibrary("muiscreen.library") here.  MakeLibrary has not yet
     * AddLibrary()'d us, so a commercial muiscreen that opens muimaster.library
     * from its own LibInit/LibOpen will LoadSeg this file again and recurse
     * until the stack runs out - which is exactly "opentest muimaster.library
     * hangs; opentest zunemaster.library does not".  configdata.c and window.c
     * already treat a NULL MUIScreenBase as "no pubscreen helper".
     */

    /*
     * Classic AmigaOS native build never opens cybergraphics.library.
     * Proto stubs (ZUNE_NO_CYBERGRAPHICS) make every CGX call a no-op, and
     * MUIMRI_TRUECOLOR stays clear so paint uses planar graphics.library.
     */
#ifdef ZUNE_NO_CYBERGRAPHICS
    CyberGfxBase = NULL;
    libBase->cybergfxbase = NULL;
#else
    /* Optional: continue even if cybergraphics.library is not available */
    CyberGfxBase = OpenLibrary("cybergraphics.library", 0);
    libBase->cybergfxbase = CyberGfxBase;
#endif
#ifdef HAVE_COOLIMAGES
    CoolImagesBase = libBase->coolimagesbase =
        OpenLibrary("coolimages.library", 0);
#endif

    /* Initialize internal lists and semaphores */
    InitSemaphore(&libBase->ZuneSemaphore);
    NewList((struct List *)&libBase->BuiltinClasses);
    NewList((struct List *)&libBase->Applications);

    /*
     * The following three allocations live in muimaster_init-aros.c upstream,
     * which is not part of this build.  They are not optional: mui_makeobject.c
     * uses topaz8font as the fallback font, and window.c dereferences
     * defaultPens[] for all eight MPEN_* slots on every MUIM_Window_Setup, so
     * leaving defaultPens NULL makes render-info setup read from low memory.
     */
    libBase->topaz8font = OpenFont((struct TextAttr *)&topaz8Attr);

    /* Reserve the address that Notify class uses as its special trigger value
       so that no real object can ever be allocated there. May legitimately
       fail if something else already owns that page. */
    libBase->SpecialMemory = AllocAbs(4, (APTR)MUIV_TriggerValue);

    if (!(libBase->defaultPens = AllocMem(sizeof(struct MUI_PenSpec) * MPEN_COUNT,
        MEMF_ANY | MEMF_CLEAR)))
        goto fail;

    pens = libBase->defaultPens;
    init_syspen(&pens[MPEN_SHINE], SHINEPEN);
    init_syspen(&pens[MPEN_BACKGROUND], BACKGROUNDPEN);
    init_syspen(&pens[MPEN_SHADOW], SHADOWPEN);
    init_syspen(&pens[MPEN_TEXT], TEXTPEN);
    init_syspen(&pens[MPEN_FILL], FILLPEN);
    init_syspen(&pens[MPEN_MARK], HIGHLIGHTTEXTPEN);

    /* An empty spec tells window.c to derive these two itself from the
       screen's real pen set rather than pinning them to a system pen. */
    pens[MPEN_HALFSHINE].buf[0] = (UBYTE) '\0';
    pens[MPEN_HALFSHADOW].buf[0] = (UBYTE) '\0';

#ifdef MUSHIN_GCC_NATIVE
    if (!ZuneGccInit())
        goto fail;
#endif

    return TRUE;

fail:
    D(bug("muimaster.library Init FAILED. Expunging open libraries.\n"));
    L_ExpungeLib(_MUIMasterBase);
    return FALSE;
}

/****************************************************************************************/

ULONG SAVEDS STDARGS LC_BUILDNAME(L_OpenLib)(LC_LIBHEADERTYPEPTR MUIMasterBase)
{
    D(bug("Inside Open func of muimaster.library\n"));
    return TRUE;
}

/****************************************************************************************/

void SAVEDS STDARGS LC_BUILDNAME(L_CloseLib)(LC_LIBHEADERTYPEPTR MUIMasterBase)
{
    D(bug("Inside Close func of muimaster.library\n"));
}

/****************************************************************************************/

void SAVEDS STDARGS LC_BUILDNAME(L_ExpungeLib)(LC_LIBHEADERTYPEPTR _MUIMasterBase)
{
    /* C89 COMPATIBILITY: Declaration at top of the block. */
    MUIMasterBase_intern *libBase = (MUIMasterBase_intern *)_MUIMasterBase;

    D(bug("Inside Expunge func of muimaster.library\n"));

    /*
     * Builtin IClasses used to be FreeClass()'d from MUI_FreeClass when
     * the last NewObject ref dropped.  That destroyed Notify while
     * Application was still in OM_DISPOSE.  Tear them down here, newest
     * first, before Intuition is closed.  Each MakeClass OpenLibrary is
     * balanced by CloseLibrary.
     */
    {
        Class *cl;

        while ((cl = (Class *) RemTail((struct List *)
            &libBase->BuiltinClasses)) != NULL)
        {
            cl->cl_Flags &= ~CLF_INLIST;
            if (FreeClass(cl))
                CloseLibrary((struct Library *)_MUIMasterBase);
        }
    }

#ifdef MUSHIN_GCC_NATIVE
    ZuneGccCleanup();
#endif

    if (libBase->defaultPens)
        FreeMem(libBase->defaultPens,
            sizeof(struct MUI_PenSpec) * MPEN_COUNT);
    libBase->defaultPens = NULL;

    if (libBase->SpecialMemory)
        FreeMem(libBase->SpecialMemory, 4);
    libBase->SpecialMemory = NULL;

    /* CloseFont needs GfxBase, so this has to happen before graphics.library
       is closed below. */
    if (libBase->topaz8font)
        CloseFont(libBase->topaz8font);
    libBase->topaz8font = NULL;

    if (libBase->gfxbase)
        CloseLibrary((struct Library *)libBase->gfxbase);
    libBase->gfxbase = NULL;
    GfxBase = NULL;

    if (libBase->utilitybase)
        CloseLibrary((struct Library *)libBase->utilitybase);
    libBase->utilitybase = NULL;
    UtilityBase = NULL;

    if (libBase->dosbase)
        CloseLibrary((struct Library *)libBase->dosbase);
    libBase->dosbase = NULL;
    DOSBase = NULL;

    if (libBase->intuibase)
        CloseLibrary((struct Library *)libBase->intuibase);
    libBase->intuibase = NULL;
    IntuitionBase = NULL;

    if (AslBase)
        CloseLibrary(AslBase);
    AslBase = NULL;
    libBase->aslbase = NULL;

    if (LayersBase)
        CloseLibrary(LayersBase);
    LayersBase = NULL;
    libBase->layersbase = NULL;

    if (CxBase)
        CloseLibrary(CxBase);
    CxBase = NULL;
    libBase->cxbase = NULL;

    if (RexxSysBase)
        CloseLibrary((struct Library *)RexxSysBase);
    RexxSysBase = NULL;
    libBase->rxsbase = NULL;

    if (GadToolsBase)
        CloseLibrary(GadToolsBase);
    GadToolsBase = NULL;
    libBase->gadtoolsbase = NULL;

    if (KeymapBase)
        CloseLibrary(KeymapBase);
    KeymapBase = NULL;
    libBase->keymapbase = NULL;

    if (LocaleBase)
        CloseLibrary(LocaleBase);
    LocaleBase = NULL;

    if (DataTypesBase)
        CloseLibrary(DataTypesBase);
    DataTypesBase = NULL;

    if (IFFParseBase)
        CloseLibrary(IFFParseBase);
    IFFParseBase = NULL;
    libBase->iffparsebase = NULL;

    if (DiskfontBase)
        CloseLibrary(DiskfontBase);
    DiskfontBase = NULL;
    libBase->diskfontbase = NULL;

    if (IconBase)
        CloseLibrary(IconBase);
    IconBase = NULL;
    libBase->iconbase = NULL;

    if (CyberGfxBase)
        CloseLibrary(CyberGfxBase);
    CyberGfxBase = NULL;
    libBase->cybergfxbase = NULL;

    if (WorkbenchBase)
        CloseLibrary(WorkbenchBase);
    WorkbenchBase = NULL;
    libBase->workbenchbase = NULL;

    if (MUIScreenBase)
        CloseLibrary(MUIScreenBase);
    MUIScreenBase = NULL;
#ifdef HAVE_COOLIMAGES
    if (libBase->coolimagesbase)
        CloseLibrary(libBase->coolimagesbase);
    libBase->coolimagesbase = NULL;
    CoolImagesBase = NULL;
#endif
}
/****************************************************************************************/
