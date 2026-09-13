#ifndef _KNOB_PRIVATE_H_
#define _KNOB_PRIVATE_H_

/* Internal header, not the PRIV-stripped generated <libraries/mui.h>: both
   use the LIBRARIES_MUI_H guard, so whichever is seen first suppresses the
   other, and the private instance data below needs the full declarations. */
#include "mui.h"

struct Knob_DATA
{
    struct MUI_EventHandlerNode  ehn;
    DOUBLE prevangle;
};

#endif /* _KNOB_PRIVATE_H_ */
