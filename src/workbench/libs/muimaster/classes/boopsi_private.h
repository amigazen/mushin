#ifndef _BOOPSI_PRIVATE_H_
#define _BOOPSI_PRIVATE_H_

#include <exec/types.h>
#include <intuition/classusr.h>
/* Internal header, not the PRIV-stripped generated <libraries/mui.h>: both
   use the LIBRARIES_MUI_H guard, so whichever is seen first suppresses the
   other, and the private instance data below needs the full declarations. */
#include "mui.h"

/*** Instance data **********************************************************/
struct Boopsi_DATA
{
    struct TagItem *remember;
    LONG remember_len;

    struct IClass *boopsi_class;
    char *boopsi_classid;
    int boopsi_minwidth,boopsi_minheight;
    int boopsi_maxwidth,boopsi_maxheight;
    Object *boopsi_object;
    ULONG boopsi_tagdrawinfo;
    ULONG boopsi_tagscreen;
    ULONG boopsi_tagwindow;
    ULONG boopsi_smart;

    struct TagItem *boopsi_taglist;
    struct MUI_EventHandlerNode ehn;
};

#endif /* _BOOPSI_PRIVATE_H_ */