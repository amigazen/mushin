#ifndef _AREA_MACROS_H
#define _AREA_MACROS_H

#include "classes/area.h"

#ifndef _MUI_MACROS_H
#include "macros.h"
#endif

/*
 * macros.h normally declares this struct, but a translation unit that pulled
 * in the generated <libraries/mui.h> first already has _MUI_MACROS_H set, so
 * the include above is skipped and the struct has to be declared here instead.
 * MUI_AREADATA_DEFINED keeps the two declarations from colliding.
 */
#ifndef MUI_AREADATA_DEFINED
#define MUI_AREADATA_DEFINED
struct __dummyAreaData__
{
    struct MUI_NotifyData mnd;
    struct MUI_AreaData   mad CLASS_INSTANCE_ALIGN;
};
#endif

#endif /* _AREA_MACROS_H */
