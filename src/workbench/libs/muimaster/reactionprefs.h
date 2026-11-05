/*
    Copyright (C) 2026, amigazen project. All rights reserved.
*/

/*
 * Load ENV:sys/reaction.prefs (IFF FORM PREF / RACT) and map overlapping
 * ReAction look settings onto ZunePrefsNew.
 *
 * Struct layout matches NDK3.2R4 <prefs/reaction.h>.
 */

#ifndef ZUNE_REACTIONPREFS_H
#define ZUNE_REACTIONPREFS_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

struct ZunePrefsNew;

#define ZUNE_REACTION_PREFS_ENV    "ENV:sys/reaction.prefs"
#define ZUNE_REACTION_PREFS_ENVARC "ENVARC:sys/reaction.prefs"

/*
 * Apply reaction.prefs onto prefs.  On success, *font_normal and/or
 * *font_button may be AllocVec'd font name strings the caller owns and
 * must FreeVec (pass existing pointers if re-applying).  *pattern_spec
 * may receive an AllocVec'd imagespec ("5:path" / "3:path") when
 * rp_Pattern holds a backfill name; caller owns and must FreeVec.
 * Returns TRUE if a RACT chunk was read and applied.
 */
BOOL Zune_ApplyReactionPrefs(struct ZunePrefsNew *prefs,
    STRPTR *font_normal, STRPTR *font_button, STRPTR *pattern_spec);

#endif /* ZUNE_REACTIONPREFS_H */
