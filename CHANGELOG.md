# Changelog

All notable changes to mushin since the fork of AROS Zune are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

The baseline is [`ab805da`](https://github.com/amigazen/mushin/commit/ab805da00d0b1bbc42af1da14cfd6c4c9ba659dc) (21 October 2025), the initial import of the AROS Zune components. That import contained `src/workbench/libs/muimaster`, `src/workbench/classes`, and `src/workbench/prefs`. 

## [2026-10-12]

### Fixed

- Voyager locked up on a page change and crashed after a resize. `RecalcDisplay` lays the group out in place, and `AddEventHandler` unlinks a node that is still on the window list before `Insert`. `WindowLimits` runs only for a `NEWSIZE` whose limits differ from the current ones. A simple-refresh redraw drains `LAYERREFRESH` before the transfer-animation blit. Input handlers with 32-bit method ids go through `DoMethodA`. Family insert detaches a node only when both neighbours still point at it. Group attribute forwarding stops at a bounded walk. A virtgroup scroll that layout has already clamped is not applied again on the next `Update`.
- `MUIM_GetConfigItem` stored the dataspace record address. Layout hooks read that address as a coordinate, which placed toolbars and the HTML group outside the window. Numeric items now store the `ULONG`. String items still store the pointer.
- `Virtgroup` `Draw` returned without `MUIM_Virtgroup_BeginPaint`, so the HTML class never received a paint for the document. `Draw` calls `BeginPaint` after the background and `EndPaint` when the pass finishes, and forwards the draw flags on `MUIM_Draw`.
- `Dispose` left a child whose neighbour no longer pointed back. The next `OM_GET` followed that pointer into freed memory on a Voyager tab change. Unlink drops a one-sided link and clears the node's own pointers. It does not splice a stale successor onto a live list. Family sort builds the replacement list with `NewList` and `AddTail`.
- `MUIM_Window_Setup` ran before the screen render info existed. A subclass that reads `MUIA_Window_Screen` during `Setup` deadlocked on the public screen lock. Open fills in the screen render info first. `RecalcDisplay` lays out in place.
- IBrowse opened with a blank HTML page. HTMLview paints from `MUIM_CustomBackfill`, and only when the rectangle is the virtual group's inner box. A full draw sends that method after the clip is installed. `DrawBackground` forwards a custom backfill and leaves the document pixels alone.
- The IBrowse paint fix made Voyager page changes hang. `DrawBackground` forwarded `MUIM_CustomBackfill` to every object that carried the attribute, so Voyager's page groups ran `Backfill` while the old document was still being torn down and layout never left `AskMinMax`. The callback is limited to a custom Virtgroup that has a layout hook and no children, which is HTMLview. A size event under the transfer animation skips the extra `WindowLimits` call. A redraw waits until `LAYERREFRESH` is clear before the blit.
- String fields filled orange. Textinput reads the string background setting as a pen, and that setting is the image spec `2:m2`. The leading `2` was parsed as an RGB colour, `ObtainPen` failed, and the fill used pen `0xFFFF`. A `2:` prefix is skipped and the remainder is taken as the MUI pen.
- The MUI demos failed drag-and-drop, virtual scrolling, window sizing, and requesters. A list starts its own drag and keeps receiving mouse moves for the duration. A virtual group moves its children through the C `MUI_Offset` call and repaints the strip that scrolls into view. `SizeWindow` is followed by a fresh `WindowLimits`, so the window cannot be dragged smaller than its contents. A proportional gadget inside a virtual group tracks the scrollbar instead of refreshing at the pre-scroll position.
- Apdf left the page view stale when a scrollbar moved, and About disposed the application out from under the requester. A Virtgroup subclass that has children is redrawn after the scroll. `MUI_Request` keeps the application and the requester window allocated across the input loop.

## [2026-10-10]

### Added

- `Copyright (C) 2026, amigazen project` on every file added in this fork and on every file whose substance changed relative to the AROS import. Existing AROS and third-party copyright lines were left in place.

## [2026-10-06]

### Added

- C string and memory routines in `support_amigaos.c`: `memcpy`, `memmove`, `memset`, `memcmp`, `strcpy`, `strncpy`, `strcat`, `strcmp`, `strncmp`, `strlen`, `strchr`, `strrchr`, `strstr`, `stricmp`, `toupper`, `isdigit`, and `strtoul`. `memcpy` and `memmove` call `CopyMem`. `memset` fills on the CPU, because `BltClear` only applies to chip memory. `stricmp` and `toupper` call `utility.library`. The remaining routines have no exec entry and are plain C.
- `ZuneSin`, `ZuneCos`, and `ZuneAtan2`, implemented on `mathieeedoubtrans.library`. The division inside `atan2` uses `mathieeedoubbas.library`. Both libraries are opened in `LibInit` and closed in expunge. If either open fails, the three calls return 0. On AROS the same names remain libm macros.

### Changed

- Datatype integer parsing calls `dos.library` `StrToLong`.
- Dirlist, Poplist, and Popscreen comparisons call `Stricmp`.
- Rexx port name folding calls `ToUpper`.
- Knob, level meter, and gradient drawing call `ZuneSin`, `ZuneCos`, and `ZuneAtan2`.

### Removed

- `#include <string.h>`, `<stdio.h>`, `<stdlib.h>`, and `<ctype.h>` from the library sources and from `headers.c`. `all.gst` was picking up the SAS/C libc prototypes from those includes and the library was linking against routines it does not ship. `buildincludes`, `opentest`, `test`, and `voyagertest` still include libc. `sc.lib` remains on the link line for the compiler helpers.

## [2026-10-05]

### Added

- MUI 3.8 `AreaData` fields on `mad_Flags2`: weights, disappear sizes, short help, `FixWidth`, `FixHeight`, and `VirtualTop`.
- `_vleft`, `_vtop`, `_vright`, and `_vbottom`, the virtual-origin counterparts of the existing edge macros.
- `MADF_GROUP`, `MADF_CUSTOMBACKFILL`, `MADF_PARTIAL`, `MADF_VISIBLE`, `MADF_KNOWSACTIVE`, and the draw-flag bits. Flags that exist only in Zune moved onto `mad_Flags2` so the low flag word matches MUI 3.8.
- `MUIA_CustomBackfill`. An object that sets it paints its own background.
- A clip argument on `MUIM_Show`, used by Virtgroup `ShowClipped`.
- Virtgroup instance data laid out as MUI 3.8 `VirtgroupData38`, with `InstallClip`, `RemoveClip`, and `Update`.
- `MUI_Offset`. Virtgroup scrolling moves the children by that call. The previous path ran `Hide`, `Layout`, and `Show` for every scroll step.
- A nest count on Group `InitChange` and `ExitChange`. An inner `ExitChange` during an outer child swap returns without `RecalcDisplay`.
- An ancestor walk that uses the Group class itself. A subclass `ExitChange` finds a parent that is still exchanging children.
- Private vectors `ObtainImage`, `ReleaseImage`, `Show`, `Hide`, `LayoutObj`, `Offset`, and `GetVirtualRect`.
- `ZUNE_FreeClass`, callable with a stack argument. `CreateCustomClass`, `DeleteCustomClass`, `NewObject`, and `DisposeObject` free classes through it.
- `LIBS:mui/` and `LIBS:MUI/` searched before the Zune directories when an external MCC is opened.
- Floattext registered in the SAS/C builtin class list. The class was compiled and then omitted from `builtins[]`.
- An overlay of `ENV:sys/reaction.prefs` onto the shared look settings. The ReAction font name strings are copied into library-owned storage.
- `MUI_Layout` updates `mad_VirtualTop` when it places an object.
- A `voyagertest` link rule that pulls in the same objects Voyager does. The `demos` target builds `tutorial/examples`.
- Tutorial examples that call `OpenLibrary` on the installed master. The command line may name a different library.
- `muiscreen_lib.c` for SAS/C. Init opens dos, utility, graphics, intuition, and iffparse into real globals, starts the public-screen close task, and expunge releases those resources.
- SAS/C register prototypes for the muiscreen LVOs, and `MAKE_ID` in `libraries/muiscreen.h`.
- `lib_Version` 19, `lib_Revision` 75, version date 25.09.2026. `LibOpen` prints one line so a resident copy older than the file on `LIBS:` is visible in the log.
- `.gitignore` entries for object files, UAE maps, `output.txt`, `amiga_compiler.h`, and the generated cybergraph files.
- `src/workbench/libs/muiscreen/smakefile` and `SCOPTIONS`. The smakefile builds `muiscreen.library`, `opentest`, and `muiscreentest` with `LIBRARYCODE` unset. `SCOPTIONS` selects no debug, no stack check, and small code and data.
- `muiscreen_lib.fd` and `muiscreen_lib.sfd` for the ten public calls from `MUIS_AllocPubScreenDesc` through `MUIS_RemInfoClient`, bias 30. The same two files are under `include/fd`.
- `clib/compiler-specific.h` from the NDK as the register-argument header for muimaster and muiscreen. `__ASM__`, `__REG__`, `__SAVE_DS__`, and `__STDARGS__` cover library entries, hooks, and dispatchers. `amiga_compiler.h` is checked in beside each library so the build does not depend on a particular NDK install.
- `sysiclass` images for the built-in checkmark, radio button, and arrows. Cycle, popup, file, drawer, and tape arrows use `glyph.image` when that class is present. Pictures with no system glyph, and machines without `glyph.image`, keep the vector drawing.
- ReAction bevel types mapped onto MUI frames. Gadget glyphs are drawn from `glyph.image` rather than from the window-chrome `FILLPEN` pictures. `3DLook` leaves the separator frame alone. Xen uses the zin31 bevel. Thin Xen is a 1-pixel half-shine bevel. The checkmark is the glyph tick, separate from the menu check.

### Changed

- The preferences program, its catalog name, and its Workbench icon are `mushin`. Window titles, the About menu, version strings, and installer paths use that name. `Zune.lnk` was renamed to `mushin.lnk`.
- The top-level GNU makefile is `makefile.gcc`, which is the name the ToolKit SDK documents.

### Fixed

- `AskMinMax` with a `NULL` or non-memory `MinMaxInfo` reads the object's own `mad_MinMax`.
- `ShowMe` during a parent Group `InitChange` returns without `RecalcDisplay`. The parent finishes the child exchange first.
- Virtgroup `InitChange` subtracts the current scroll offset before it clears `Top` and `Left`.
- Group `DisconnectParent` leaves `MUIA_Parent` set while the child is still in the family.
- Window open chooses width and height independently. A saved position that recorded only the window chrome is discarded.
- Window dispose skips `FreeVec` on a title the application owns, and closes the window before the child objects are freed.
- Window `RecalcDisplay` lays out before any `Hide`. A nested recalc takes the layer lock once.
- Removing a window event handler checks that the node was inserted. `Remove` on a node that was never added corrupted the handler list.
- `MUI_Layout` on an object with a `NULL` parent returns failure. The previous code wrote the origin through that pointer.
- `MUI_CreateCustomClass` looks up the superclass with a normal C call. The LVO form read the library base from A0, so every subclass create failed.
- Each external MCC `OpenLibrary` is paired with `CloseLibrary`. The module open count can reach zero and the class can expunge.
- Builtin classes are created without an `OpenLibrary` of muimaster for each class. Expunge of those classes does not `CloseLibrary` the library that contains them.
- The last `LibClose` expunges the library. The previous path left the old segment resident, so the next open reused it.
- A standard group title uses the text pen. Hilite and 3D titles still use shine and shadow.
- muiscreen rejects a missing public-screen description, a `NULL` name, a missing Workbench screen, and a `NULL` info client.
- The public-screen close task deletes its message port, and expunge `Wait`s for the task to exit.
- Scrollgroup failed to compile after `layout_hook` was deleted. The field is back on the instance data and the SAS/C build uses it.
- The window cycle chain tests `MADF2_SETUP` on `mad_Flags2`. `RecalcDisplay` references to the old flag-word locals were removed with the flag move.
- Application `InputBuffered` drains one window message and returns. The merged path had called back into `NewInput` and re-entered the same handler.
- Notify delivery, List sorting, Bitmap pen allocation, and the `MUI_Redraw` clip for a virtual group are the pre-merge implementations. The merge had dropped those four paths.
- `MUI_RemoveClipRegion` pops the MUI clip stack and installs the region underneath. The page-clip stand-in pointer had been passed to `InstallClipRegion`, which left the HTML rectangle on the window and clipped Voyager's footer.
- The last close calls `L_ExpungeLib` directly. The `ZUNE_FreeBuiltinClasses` gate that wrapped expunge was removed, together with its prototype.
- The `snprintf` declaration in the prefs header takes `int` for the size argument, matching `support_amigaos.c`. SAS/C rejected the translation unit when the two prototypes disagreed.

### Removed

- The builtin-class `OpenLibrary("muimaster.library")` that held `OpenCnt` above zero for the life of the process.
- `CloseLibrary` of muimaster from the expunge of its own builtin classes.
- The in-process fake library base in the tutorial examples. Each example opens the installed library.
- SAS/C `DEBUG=LINE` and `OPTIMIZERINLINELOCAL` from `SCOPTIONS`, and `small.lib` from the link line.
- The second Floattext entry in the builtin list.
- `register __a0` and `__asm` parameter syntax from the muimaster and muiscreen sources touched by the compiler-specific.h change. Those files use `__ASM__` and `__REG__`.

## [2026-09-14]

### Added

- A SAS/C build of the preferences program: `src/workbench/prefs/Zune/smakefile`, `SCOPTIONS`, and `amigasupport.c`. `amigasupport.c` supplies `XGET`, `DoSuperNewTags`, and `snprintf`, which the AROS prefs sources call and which SAS/C does not provide.
- Headers the prefs sources include and which the initial import did not contain: `include/aros/debug.h`, `include/devices/rawkeycodes.h`, `include/mui/Rawimage_mcc.h`, and `include/zune/customclasses.h`.
- CatComp input for the preferences catalog: `catalogs/zune.cd`, `catalogs/catalog_version.h`, `strings.h`, `catalogs/mmakefile.src`, and the czech, deutsch, español, français, italiano, and russian `.ct` files.

### Changed

- Prefs pages whose MCP class is absent from this tree are omitted from the page list at startup.
- Image buttons that loaded their picture from `THEME:` use the built-in imagery. The `THEME:` assign is an AROS path and is not present on a classic install.
- The default preferences window uses a shorter layout so the full window fits on a 640x256 screen.

## [2026-09-13]

### Added

- SAS/C `SCOPTIONS` and smakefile targets that build `muimaster.library` as a drop-in and `zunemaster.library` side by side from the same sources. The targets ship FD, SFD, and protos, and stub the CyberGraphX calls the AROS sources make unconditionally.
- `ZuneNextObject` and a `DoMethodA` helper. Clipping and redraw have C bodies for in-library calls. Signal masks are passed as 32-bit values.
- `opentest` coverage that builds a window, sets and gets attributes, and runs `MUIM_Application_NewInput` until the close gadget or Ctrl-C.
- `mui_private.c` and `muimaster_lib.fd`.

### Fixed

- String input faulted because `LocaleBase` was never opened. Slider and Gauge never saw mouse moves because the window lacked `WFLG_REPORTMOUSE`. Dead keys were dropped because `RAWKEY` events were not passed through `MapRawKey`. `FindAreaObject` and `GoActive` called the method as a C function and read the registers wrong; both go through `DoMethodA`. Smart-refresh windows painted outside the damage region. Register tabs failed to redraw the newly selected page.
- `LibInit` installed the default pens, opened `topaz.font` at size 8, and opened the required library bases before returning the library. Dispose and child walks followed the AROS list macros into the wrong link field under SAS/C. LVO entries and 32-bit method ids use the SAS/C register convention.

### Removed

- Panel and PanelGroup from the OS3 build: `panel.c`, `panel.h`, `panel_private.h`, `panel.conf`, `panelgroup.c`, `panelgroup.h`, `panelgroup_private.h`, `panelgroup.conf`, and `tutorial/examples/PanelGroup.c`. Those classes are not part of the MUI 3.8 set this library implements.
- `OpenLibrary` of `muiscreen.library` and `cybergraphics.library` from `LibInit`. Opening muiscreen from inside muimaster re-entered `LoadSeg` on the same load and hung.

## [2026-07-05]

### Fixed

- Regenerated the SDK muimaster headers from the corrected SFD: `clib/muimaster_protos.h`, `inline/muimaster.h`, `inline/muimaster_protos.h`, `pragma/muimaster_lib.h`, `pragmas/muimaster_pragmas.h`, `Include_i/lvo/muimaster_lib.i`, and `emodules/muimaster.m`. `MUI_Error` was pragma `00`, which passes no register; it is `801` (D0). `MUI_Priv1` through `MUI_Priv4` are declared at offsets `0x84` through `0x96`, so `MUI_ObtainPen` at `0x9C` through `MUI_EndRefresh` at `0xC6` line up with `LibVectors[]`. The MUI 3.8 developer slots after that (Show, Hide, LayoutObj, Offset) stay out of the public header until those vectors exist.

## [2025-10-26]

### Added

- `L_InitLib` for the Amiga library vector table, opening the bases `muimaster.library` needs and returning failure if a required open fails.
- `opentest.c`, which calls `OpenLibrary` on the built master, checks the returned base, and closes it.

### Changed

- The linked binary is named `muimaster.library`. The file previously checked in as `zunemaster.library` was renamed to that.

## [2025-10-25]

### Added

- `README.md`, describing the fork, the AmigaOS 3 target, and where the license terms are.

### Fixed

- A clean `smake` failed. `mui.h` included `prefs.h`, so every translation unit compiled the prefs declarations. `CFLAGS` lacked `NOCHECKABORT`, and SAS/C pulled in the check-abort stub. `WritePixelArrayAlpha` and `WriteLUTPixelArray` had no Amiga definitions, so frame and image objects did not link. `support_amigaos.c` now provides both CyberGraphX calls. `HexToLong` and `HexToIPTR` are `__saveds`. The `prefs.h` include was removed from `mui.h`, and the smakefile adds `NOCHECKABORT`.

## [2025-10-22]

### Added

- `include/SDI/SDI_lib.h`. The AROS `AROS_LH`/`AROS_LHA` library macros do not expand under SAS/C. The SDI header supplies the Amiga `LIBFUNC`/`LIBPROTO` forms used by the public entry points.
- `test.lnk` and `wb.lnk`, the SAS/C link responses for the test program and the Workbench-launched build.
- `zunemaster.library` and `zunemaster.library.info`, the first library that linked.

### Changed

- `L_InitLib`, `L_OpenLib`, `L_CloseLib`, and `L_ExpungeLib` in `muimaster_init.c` are `__asm __saveds` with the library base in A0. The AROS `LC_BUILDNAME` wrappers passed the base on the stack, so exec called the wrong entry.
- Each public LVO (`mui_addclipping.c`, `mui_addclipregion.c`, `mui_allocaslrequest.c`, `mui_aslrequest.c`, `mui_beginrefresh.c`, `mui_createcustomclass.c`, `mui_deletecustomclass.c`, `mui_disposeobject.c`, `mui_endrefresh.c`, `mui_error.c`, `mui_freeaslrequest.c`, `mui_freeclass.c`, `mui_getclass.c`, `mui_layout.c`, `mui_makeobject.c`, `mui_newobject.c`, `mui_obtainpen.c`, `mui_redraw.c`, `mui_rejectidcmp.c`, `mui_releasepen.c`, `mui_removeclipping.c`, `mui_removeclipregion.c`, `mui_request.c`, `mui_requestidcmp.c`) uses the same Amiga register entry. The class files that call those routines were updated with them.

### Fixed

- `locale.c` called `ADD2INIT` and `ADD2EXIT`, which SAS/C does not provide, and included `catalogs/catalog_version.h`, which is not in this tree. Locale init is explicit, and `CATALOG_VERSION` is `3`.
- `muimaster_init.c` declares the global bases the objects reference: asl, layers, commodities, rexxsyslib, gadtools, keymap, datatypes, iffparse, diskfont, icon, workbench, coolimages, and cybergraphics. Without the declarations the objects compiled and the link reported them undefined.
- The library reported version 1.0. Callers and the SFD are version 19. `zunemaster_lib.c` reports version 19.50. The smakefile compiles `mui_makeobject.c` and `mui_newobject.c` from the library root. It had been compiling the copies under `staticlib/`, which did not match the versioned prototypes. `muiRenderInfo()` is defined when `MUI_AREADATA_DEFINED` is set. Setting that guard had hidden the macro and broken every object that used it.

### Removed

- `temp_smk.tmp`, a scratch smakefile left in the library directory.

## [2025-10-21]

`ab805da` imported `muimaster.library`, the Zune classes, and the Zune preferences program from AROS. The commits the same day put back the files that import was missing and made the Amiga objects compile.

### Added

- `LICENSE.md`, the AROS Public License. Exhibit A names the AROS Development Team as the Initial Developer and the amigazen project as the copyright holder of portions created in this fork.
- `muimaster_lib.fd` and `muimaster_lib.sfd`. They were added under `include/sfd/` and moved to `include/fd/` when muiscreen gained its own FD pair.
- Headers generated by LibDescConverter from that SFD, both next to the library and under `SDK/`: `clib/muimaster_protos.h`, `inline/muimaster.h`, `inline/muimaster_protos.h`, `pragma/muimaster_lib.h`, `pragmas/muimaster_pragmas.h`, `proto/muimaster.h`, `libraries/mui.h`, `SDK/Include_i/lvo/muimaster_lib.i`, and `SDK/emodules/muimaster.m`. The AROS import shipped the internal `mui.h` and had no SAS/C prototypes, pragmas, or FD.
- `catalogs/muimaster.cd` and the generated `muimaster_strings.h`. `locale.c` included the catalog and the import had no catalog source. Builtin class descriptors are declared in `support_classes.h`, which is where `support_classes.c` looks them up.
- `SDI_compiler.h` and `area_macros.h`. Class sources include both, and neither file was in the import.
- `muiscreen.library`, absent from the import. Sources: `addinfoclient.c`, `allocpubscreendesc.c`, `closepubfile.c`, `closepubscreen.c`, `freepubscreendesc.c`, `openpubfile.c`, `openpubscreen.c`, `readpubfile.c`, `reminfoclient.c`, `writepubfile.c`, `fileformat.h`, `muiscreen_init.c`, `muiscreen_intern.h`, `muiscreen.conf`, `mmakefile.src`, and `include/libraries/muiscreen.h`.
- `include/fd/muiscreen_lib.fd`, `include/fd/muiscreen_lib.sfd`, and the generated `clib/muiscreen_protos.h`, `pragmas/muiscreen_pragmas.h`, and `proto/muiscreen.h`, with the same set under `SDK/Include_h/`, plus `SDK/Include_i/lvo/muiscreen_lib.i` and `SDK/emodules/muiscreen.m`.
- `.gitignore`, the SAS/C `all.gst` global symbol table, `buildincludes`, and `zunemaster.lnk`.

### Fixed

- Declarations that followed statements, which SAS/C rejects under C89. Each of those functions now declares its locals at the start of the block.
- Class and library files that called an API with no prototype in the includes they had. The missing `#include` lines were added, and the objects that failed on that error compile.
- AROS-only types, attributes, and control flow in `application.c`, `area.c`, `boopsi.c`, `configdata.c`, `framedisplay.c`, `group.c`, `menuitem.c`, `notify.c`, `numeric.c`, `penadjust.c`, `prop.c`, `register.c`, `window.c`, `datatypescache.c`, `dragndrop.c`, `font.c`, `frame.c`, `macros.h`, `mui_disposeobject.c`, `mui_layout.c`, `mui_redraw.c`, `mui_request.c`, `muimaster_intern.h`, `support_amigaos.h`, `support_classes.c`, and `zunemaster_lib.c`. Those edits had used AROS headers and AROS list macros. Each site now uses the Amiga equivalent that SAS/C can see.
- `support_classes.c` included the wrong workbench protos header, so the builtin `_desc` symbols did not match the declarations. It includes the header that defines them.

[2026-10-12]: https://github.com/amigazen/mushin/compare/9028d4339838f6b8fa87a79a3516de15476e184e...483031b4d0325c19f973b156eb0b3cab30f1602d
[2026-10-10]: https://github.com/amigazen/mushin/compare/ab8fa50ebc13bcde6a84d41202234c3f7e528619...9028d4339838f6b8fa87a79a3516de15476e184e
[2026-10-06]: https://github.com/amigazen/mushin/compare/ed68fa81d3348f7a86371ca45af55f130462596d...ab8fa50ebc13bcde6a84d41202234c3f7e528619
[2026-10-05]: https://github.com/amigazen/mushin/compare/978e1dbf7b5276932ed0a3dcc241d5cb48d75789...ed68fa81d3348f7a86371ca45af55f130462596d
[2026-09-14]: https://github.com/amigazen/mushin/compare/2276b96541ee442dce85f4da2114146143b87ed2...978e1dbf7b5276932ed0a3dcc241d5cb48d75789
[2026-09-13]: https://github.com/amigazen/mushin/compare/b74f51f26faee7391ea415db905301fc68965284...2276b96541ee442dce85f4da2114146143b87ed2
[2026-07-05]: https://github.com/amigazen/mushin/commit/b74f51f26faee7391ea415db905301fc68965284
[2025-10-26]: https://github.com/amigazen/mushin/commit/f75f0d9da7ef975f865b339c27edc1697b6d0444
[2025-10-25]: https://github.com/amigazen/mushin/compare/09429ea6372398f94c448ca54528e3227cc75d9b...8f27d350c3cd5b2e4e3ad444cec5a3de07b9bd7a
[2025-10-22]: https://github.com/amigazen/mushin/compare/30a99965de1f5febd564c29ef83fed183b5c00ee...09429ea6372398f94c448ca54528e3227cc75d9b
[2025-10-21]: https://github.com/amigazen/mushin/compare/ab805da00d0b1bbc42af1da14cfd6c4c9ba659dc...30a99965de1f5febd564c29ef83fed183b5c00ee
