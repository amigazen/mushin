/*
    Copyright (C) 1995-2020, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <proto/locale.h>
#include "SDI_compiler.h"

#define CATCOMP_ARRAY
#include "muimaster_strings.h"

#define CATALOG_NAME     "System/Libs/muimaster.catalog"
#define CATALOG_VERSION 3

/*** Variables **************************************************************/
struct Catalog *catalog;


/*** Functions **************************************************************/
/* Main *********************************************************************/
CONST_STRPTR _(ULONG id)
{
    if (LocaleBase != NULL && catalog != NULL)
    {
        return GetCatalogStr(catalog, id, CatCompArray[id].cca_Str);
    }
    else
    {
        return CatCompArray[id].cca_Str;
    }
}

/* Setup ********************************************************************/
BOOL Locale_Initialize(VOID)
{
    if (LocaleBase != NULL)
    {
        catalog = OpenCatalog
        (
            NULL, CATALOG_NAME, OC_Version, CATALOG_VERSION, TAG_DONE
        );
    }
    else
    {
        catalog = NULL;
    }
    return TRUE;
}

VOID Locale_Deinitialize(VOID)
{
    if(LocaleBase != NULL && catalog != NULL) CloseCatalog(catalog);
}


/* ADD2INIT and ADD2EXIT macros removed - not compatible with SAS/C */
