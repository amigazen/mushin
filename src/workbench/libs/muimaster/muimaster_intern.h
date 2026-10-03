/*
    Copyright (C) 2002-2025, The AROS Development Team. All rights reserved.
*/

#ifndef MUIMASTER_INTERN_H
#define MUIMASTER_INTERN_H

#ifndef EXEC_TYPES_H
#   include <exec/types.h>
#endif
#ifndef EXEC_LIBRARIES_H
#   include <exec/libraries.h>
#endif
#ifndef EXEC_MEMORY_H
#   include <exec/memory.h>
#endif
#ifndef INTUITION_CLASSES_H
#   include <intuition/classes.h>
#endif
#ifndef INTUITION_INTUITIONBASE_H
#   include <intuition/intuitionbase.h>
#endif
#ifndef GRAPHICS_GFXBASE_H
#   include <graphics/gfxbase.h>
#endif
#ifndef DOS_DOS_H
#   include <dos/dos.h>
#endif
#ifndef UTILITY_UTILITY_H
#   include <utility/utility.h>
#endif
#ifndef EXEC_SEMAPHORES_H
#   include <exec/semaphores.h>
#endif

#ifdef __AROS__
#   ifndef AROS_ASMCALL_H
#       include <aros/asmcall.h>
#   endif
#else
#   include "support_amigaos.h"
#endif

#include "mui.h"
#include "textengine.h"
#include "prefs.h"
#include "penspec.h"

/* Include prefs.h to get ZunePrefsNew definition */
#ifndef __ZUNE_PREFS_H__
#include "prefs.h"
#endif

/* Convenience alias still used by imspec.c */
typedef struct ZunePrefsNew MUI_Prefs;

/*
 * There is deliberately no "struct MUI_GlobalInfo_Private" here.
 *
 * classes/application.h (reached via mui.h above) already declares
 * struct MUI_GlobalInfo with all seven members - priv0,
 * mgi_ApplicationObject, mgi_WindowsPort, mgi_AppPort, mgi_Configdata,
 * mgi_Prefs and mgi_CustomScreen - so the private fields need no separate
 * view.  Only the *public* copy in the generated <libraries/mui.h> stops at
 * two members, and that header is shadowed inside the library by the
 * shared LIBRARIES_MUI_H guard.
 *
 * A wrapper struct that embedded the public one as its first member and then
 * repeated the private fields used to exist here.  Because the embedded
 * member was in fact the full 28-byte struct, every field it declared landed
 * 28 bytes past its real home - directly on top of app_IHList and
 * app_MethodQueue in struct MUI_ApplicationData - so Application OM_NEW
 * destroyed both lists immediately after initialising them, while readers
 * that used muiGlobalInfo() directly (macros.h _app(), area.c mgi_Prefs)
 * disagreed with every writer.  Always access these fields through
 * muiGlobalInfo(obj) or the struct itself.
 */

#ifndef BNULL
#define BNULL ((BPTR)0)
#endif


struct MUIMasterBase_intern
{
    struct Library              library;
#ifndef __AROS__
    /* On AROS these fields are handled by the system */
    struct ExecBase             *sysbase;
    BPTR                        seglist;

    /* On AROS autoopened libraries are used */
    struct DosLibrary           *dosbase;
    struct UtilityBase          *utilitybase;
    struct Library              *aslbase;
    struct GfxBase              *gfxbase;
    struct Library              *layersbase;
    struct IntuitionBase        *intuibase;
    struct Library              *cxbase;
    struct RxsLib               *rxsbase;
    struct Library              *keymapbase;
    struct Library              *gadtoolsbase;
    struct Library              *iffparsebase;
    struct Library              *diskfontbase;
    struct Library              *iconbase;
    struct Library              *cybergfxbase;
    struct Library              *workbenchbase;
#ifdef HAVE_COOLIMAGES
    struct Library              *coolimagesbase;
#endif
    
/*  struct Library              *datatypesbase; */
#endif /* __AROS__ */

    struct TextFont             *topaz8font;
    struct SignalSemaphore      ZuneSemaphore; /* Used when accessing global data */
    APTR                        SpecialMemory;

    struct MinList              BuiltinClasses;
    struct MinList              Applications;
    struct MUI_PenSpec          *defaultPens;
};

/*
 * Kickstart NextObject is an intuition register LVO. A missing prototype
 * (IGNORE=63) or lost pragma turns the call into a stack C call and the
 * return value is garbage. Every child walk in this library goes through
 * ZuneNextObject instead (support.c).
 */
Object *ZuneNextObject(APTR stateptr);
#ifdef NextObject
#undef NextObject
#endif
#define NextObject ZuneNextObject

#endif /* MUIMASTER_INTERN_H */
