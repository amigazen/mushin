#ifndef _COLORADJUST_PRIVATE_H_
#define _COLORADJUST_PRIVATE_H_

#include "mui.h"
#include "support.h"

/*** Instance data **********************************************************/
struct Coloradjust_DATA
{
    struct Library *colorwheelbase;
    struct Library *gradientsliderbase;
    struct Hook     sliderhook, wheelhook, gradhook;
    Object         *rslider, *gslider, *bslider, *colfield, *wheel, *grad;       
    ULONG           rgb[3];
    UWORD           gradpenarray[3];
    LONG            gradpen;
    BOOL            truecolor;

    struct ColorWheelIFace * icolorwheel;
};

#endif /* _COLORADJUST_PRIVATE_H_ */
