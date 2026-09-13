#ifndef _CRAWLING_PRIVATE_H_
#define _CRAWLING_PRIVATE_H_

#include <exec/types.h>
/* Internal header, not the PRIV-stripped generated <libraries/mui.h>: both
   use the LIBRARIES_MUI_H guard, so whichever is seen first suppresses the
   other, and the private instance data below needs the full declarations. */
#include "mui.h"

#define CRAWLING_INITIAL_DELAY (5 * 10)

struct Crawling_DATA
{
    struct MUI_EventHandlerNode ehn;
    LONG    	    	    	ticker;
};

#endif /* _CRAWLING_PRIVATE_H_ */
