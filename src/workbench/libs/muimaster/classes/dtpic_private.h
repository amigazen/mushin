#ifndef _DTPIC_PRIVATE_H_
#define _DTPIC_PRIVATE_H_

#include <exec/types.h>
/* Internal header, not the PRIV-stripped generated <libraries/mui.h>: both
   use the LIBRARIES_MUI_H guard, so whichever is seen first suppresses the
   other, and the private instance data below needs the full declarations. */
#include "mui.h"

/*** Instance data **********************************************************/
struct Dtpic_DATA
{
    struct Library *datatypesbase;

    STRPTR name;
    APTR dto;
    struct BitMapHeader *bmhd;
    struct BitMap *bm;
    struct BitMap *bm_highlighted;
    struct BitMap *bm_selected;
    struct MUI_EventHandlerNode ehn;

    BOOL highlighted;   // mouse pointer is within object
    BOOL selected;      // gadget is selected
    LONG deltaalpha;    // increment/decrement for each tick
    LONG currentalpha;  // the actual alpha for rendering
    BOOL eh_active;     // TRUE after MUIM_Window_AddEventHandler

    LONG alpha;
    BOOL darkenselstate;
    LONG fade;
    BOOL lightenonmouse;

};

#endif /* _DTPIC_PRIVATE_H_ */
