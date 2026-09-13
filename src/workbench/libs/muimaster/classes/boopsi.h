/*
    Copyright (C) 2002-2003, The AROS Development Team. All rights reserved.
*/

#ifndef _MUI_CLASSES_BOOPSI_H
#define _MUI_CLASSES_BOOPSI_H

/* Internal header.  buildincludes.c inlines quoted includes and already has
   "mui.h" on its list, so this line simply disappears from the generated
   <libraries/mui.h>; the angle form used to be copied through verbatim and
   left the generated header including itself. */
#include "mui.h"

/*** Name *******************************************************************/
#define MUIC_Boopsi             "Boopsi.mui"

/*** Identifier base (for Zune extensions) **********************************/
/*
 * This was a comment claiming the base lived in libraries/mui.h, which was
 * only true of the generated copy of that header: a stale revision of it still
 * carries a definition that no file in this tree produces any more.  Restored
 * here, where every other class keeps its own base and where AROS keeps this
 * one, so that MUIA_Boopsi_OnlyTrigger below resolves.
 */
#define MUIB_Boopsi             (MUIB_ZUNE | 0x00000600)

/*** Attributes *************************************************************/
#define MUIA_Boopsi_Class \
    (MUIB_MUI | 0x00426999) /* V4  isg struct IClass * */
#define MUIA_Boopsi_ClassID \
    (MUIB_MUI | 0x0042bfa3) /* V4  isg char *          */
#define MUIA_Boopsi_MaxHeight \
    (MUIB_MUI | 0x0042757f) /* V4  isg ULONG           */
#define MUIA_Boopsi_MaxWidth \
    (MUIB_MUI | 0x0042bcb1) /* V4  isg ULONG           */
#define MUIA_Boopsi_MinHeight \
    (MUIB_MUI | 0x00422c93) /* V4  isg ULONG           */
#define MUIA_Boopsi_MinWidth \
    (MUIB_MUI | 0x00428fb2) /* V4  isg ULONG           */
#define MUIA_Boopsi_Object \
    (MUIB_MUI | 0x00420178) /* V4  ..g Object *        */
#define MUIA_Boopsi_Remember \
    (MUIB_MUI | 0x0042f4bd) /* V4  i.. ULONG           */
#define MUIA_Boopsi_Smart \
    (MUIB_MUI | 0x0042b8d7) /* V9  i.. BOOL            */
#define MUIA_Boopsi_TagDrawInfo \
    (MUIB_MUI | 0x0042bae7) /* V4  isg ULONG           */
#define MUIA_Boopsi_TagScreen \
    (MUIB_MUI | 0x0042bc71) /* V4  isg ULONG           */
#define MUIA_Boopsi_TagWindow \
    (MUIB_MUI | 0x0042e11d) /* V4  isg ULONG           */

#define MUIA_Boopsi_OnlyTrigger /* PRIV */ \
    (MUIB_Boopsi | 0x00000000) /* ZV1 .s. BOOL  PRIV (for notification) */


extern const struct __MUIBuiltinClass _MUI_Boopsi_desc; /* PRIV */

#endif /* _MUI_CLASSES_BOOPSI_H */
