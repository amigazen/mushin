/*
    Copyright (C) 2002, The AROS Development Team.
    All rights reserved.
    
*/

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/commodities.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/iffparse.h>
#include <proto/timer.h>
#include <proto/utility.h>
#include <proto/asl.h>

/* for the generation of the all.gst file */

/*
 * The library's own MUI declarations have to be in the GST, not just the
 * system ones above.
 *
 * "mui.h" and the generated <libraries/mui.h> both guard themselves with
 * LIBRARIES_MUI_H, and the generated one additionally defines the
 * _MUI_CLASSES_*_H and _MUI_MACROS_H guards of every header buildincludes
 * inlined into it.  Whichever of the two a translation unit sees first
 * therefore silences the other completely.
 *
 * Every class source includes <proto/muimaster.h> (for the libcall pragmas)
 * before it includes "mui.h", and <proto/muimaster.h> pulls in
 * <clib/muimaster_protos.h>, which pulls in <libraries/mui.h>.  The public
 * header thus always won the race, and because buildincludes.c drops every
 * line containing "PRIV" when it generates that header, the whole library was
 * being compiled against the cut-down public API: struct MUI_GlobalInfo
 * without mgi_Prefs/mgi_Configdata/mgi_AppPort/mgi_WindowsPort/
 * mgi_CustomScreen, no MUIM_Window_RecalcDisplay, no MUI_EHF_HANDLEINPUT, no
 * MUIM_FindAreaObject, and so on.  Sources that happened not to touch a
 * private field still compiled, which is why this stayed hidden.
 *
 * Including "mui.h" here puts the internal declarations - and LIBRARIES_MUI_H
 * itself - into all.gst, so they are already in scope before any source file
 * starts.  The later <libraries/mui.h> then finds its own guard set and
 * expands to nothing, which is exactly what is wanted inside the library.
 *
 * Consequence for MUIMASTER_NAME: mui.h derives it from MUIMASTER_DROPIN, so
 * the value baked into the GST is the non-drop-in one.  Nothing inside the
 * library reads MUIMASTER_NAME (zunemaster_lib.c tests MUIMASTER_DROPIN
 * directly), and opentest.c is compiled without the GST so that it really
 * does validate the installed SDK header.
 */
#include "mui.h"
