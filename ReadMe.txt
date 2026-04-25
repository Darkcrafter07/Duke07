===========================================================================
 ---===|DUKE07 SOURCE PORT OF DUKE NUKEM3D ATOMIC BY DARKCRAFTER07|===---
===========================================================================
Vogons thread: https://www.vogons.org/viewtopic.php?t=107057
===========================================================================
darkcrafter774@gmail.com
===========================================================================

1. Run Windows 95
2. Install Watcom 11.0
3. Run Windows Commander
4. Navigate to D07_SRC
5. Delete all *.obj files if found
6. To build for 386, run file "MakeLQ2.bat"
7. Don't worry about linking errors
8. Copy files alq2x.obj, k.obj, eng386.obj, cash386.obj, mm386ti.obj
   to D07_source
9. Navigate to D07_source
10. Copy file "Prag386.h" as "Pragmas.h" (replace)
11. Run file "MakeLQ2.bat"
12. Delete all *.obj files in D07_source to build for 486
*** if you want to disable low detail mode, edit eng386.c as such:
    set "use_lqmode" variable to 0 and use A.asm (NOT ALQ2X-ALQ3X!!!).
---------------------------
13. To build for Pentium, navigate to D07_SRC again
14. Delete all *.obj files if found
15. Run file "Make.bat"
16. Copy files a.obj, k.obj, engine.obj, cache1d.obj, mmulti.obj
   to D07_source
17. Navigate to D07_source again.
18. Copy file "Prag586.h" as "Pragmas.h" (replace)
19. Run file "Make.bat"
20. Run file BG.bat
21. Copy Duke07.exe and D07LQ2X.exe to the folder where your 
    legally obtained copy of Duke Nukem 3D Atomic Edition 1.4
    or 1.5 is placed

If there's an esoteric error like "this line is cursed", inspect
ALL of your source files so that the final line MUST be EMPTY,
put your keyboard cursor on a last line "short something = -1",
press ENTER, then press ENTER AGAIN and SAVE.

You may run it with a /nofpu command to degrade slopes rendering
quality to run on very old and slow 486 and even 386 without FPUs.

Duke07.exe /nofpu

You can go even further and run it with low-detail mode like it's
done in Doom, make sure it's EXPERIMENTAL and looks BAD:
D07LQ2X.exe

D07LQ2X is compiled for 386 including all C and asm refactored
code, so that should run.
===========================================================================
v0.22 features (April 26, 2026):
- major sprite portals improvements:
  we're still dealing with just 1 portal pair (PORTAL0-PORTAL1) but this
  time, there's a better parallax portal-sprite projection made in
  engine.c, eng386.c in "drawsprite, drawsprite_LQ2X" function.
  So there's less seams, image is way more stable and moves around less.
  On the other hand, XY camera movement, height tracking and left-right-
  -up-down reverse motion capture is introduced, tweaked and aligned
   between each portal and its toolset sprites. Portals framerate increased
   twice allowing to introduce less seams.

  Portal-teleportation now depends on your sprite size so be careful as
  larger tend to teleport earlier (place telepdest sprites further from
  your portals!) and smaller ones teleport later so you need to get closer
  to them.
===========================================================================
v0.21 features (April 22, 2026):
- down to 384000 bytes MIDI playback from 768000 because it's too much RAM.
  If only there's a way to optimize MIDI playback and use a smaller buffer.
- first attempt to implement wall aligned sprite portals.
  They're like 1-sided only but allow to connect any part of a map,
  in any place, via sprite portal, forming a window passage. It was
  developed really throughly to make sure they don't bog even systems
  as old as 486SX too much and not cause errors and crashes.

  HOW to create them: select a tile #4096 (PORTAL0), put it anywhere in map,
  make it flat (recommended) by placing a cursor at them and pressing "R",
  you may align them to walls by pressing "O", hold shift and hold left
  moust button and pull them slightly away from the wall so that it doesn't
  get swolen by it. Then place a tile #621 (CAMERA1) in any place of a map
  where you want this portal to show image from. Duplicate CAMERA1 and give
  it a tile #5062 (PRTLTELEPDEST) - that's your teleport destination. The
  engine makes both CAMERA1 and PRTLTELEPDEST that ARE LINKED to a portal
  INVISIBLE.

  Give your portal a hitag that corresponds to a lotag of CAMERA1 and 
  PRTLTELEPDEST (yeah CAMERA1 and PRTLTELEPDEST are configured SAME WAY!).
  For example, PORTAL(hitag:9970,lotag:0), CAMERA1(hitag:0,lotag9970),
  PRTLTELEPDEST(hitag:0,lotag9970).
  E.g, they're set just like security cameras and don't interfere with them
  but make sure NOT to use a CAMERA1 sprite that is already connected to
  any security camera. Get in 2D mode (NumPad ENTER), get cursor
  over a sprite, press ALT+H to assign a hitag, enter a value, press reg-
  -ular ENTER, then press ALT+T to assign a lotag, enter a value, press reg
  -ular ENTER again.
  At this time you can only create 1 PORTAL PAIR (PORTAL0 and PORTAL1), do
  NOT use the same tile PORTAL0 for another side! Use PORTAL1 like that
  PORTAL1(hitag:9971,lotag:0), CAMERA1(hitag:0,lotag9971),
  PRTLTELEPDEST(hitag:0,lotag9971).

  Known limitations:
     - there's just ONE pair PORTAL0, PORTAL1, more to come;
     - low detail mode doesn't update picture in bottom left corner,
       I don't know how to fix this yet. At least I could fix tilt!
     - D07LQ2X still draws scanline tutti-frutti inside portals because
       the asm routines are designed to draw 320x200 image, not 128x128.
       The way to fix that would be externing "long ydim" in asm, load
       that in a register and use instead of hardcoded "200" or whatever
       value is there...
     - parallax inside portals suck, it's a subject to improve.
     - PORTAL tiles MUST BE SQUARE dimensions like 128x128 (low quality),
       192x192, 256x256 etc to work properly, there is NO way to fix that
       because there is NO need to do that as you may still resize sprite
       as you wish and it's not going to stretch contents inside.
     - it's best to have the same sizes of sprites for one pair like
       PORTAL0 and PORTAL1. In case it goes crazy, you'd delete and
       create them again without copy pasting one another for best
       stability.

            --- implementation details or how it works start ---
  ***global.c***
  add new variables below "short camsprite":
     short portalsprite0 = -1; short portalsprite1 = -1;
  ***duke3d.h***
  extern those two new variables below "extern char env_music_fn[4][13];"
    "extern short camsprite, portalsprite0, portalsprite1;"
  ***actors.c:***
  for each PORTAL#x tile create a function that's gonna act like a trigger
  to activate this particular PORTAL#x tile. movestandablesportal0(ID#128)
  for PORTAL0 and movestandablesportal1(ID#129) for PORTAL1.
  ***game.c***
  include "portals.c" file before displayrooms function, modify it as well
  in order to call particular portal drawing functions as se40codeportal#x
  and se40_DrawPortal#x and restore screen after them. Make a function
  before "short spawn" - startspriteportal and call movestandablesportal0,
  movestandablesportal1 inside. Modify function "domovethings" in order 
  to call "startspriteportal" function after "movefta();" call.
  Modify function "short spawn" in order to include "PORTAL0 and PORTAL1"
  actors to be included in this line "if( PN != SPEAKER && PN != LETTER..."
  Place new cases on the new line like if(PN!=PORTAL0&&PN!=PORTAL1 etc),
    if not done, portals are not going to activate drawing automatically.
  In the same function "short spawn" extend T temp_data from 6 to 12...
  like "T1=T2=T3=T4=T5=T6=T7=T8=T9=T10=T11=T12=0;" In the same function,
    add "case PORTAL0, case PORTAL1" before or after "case VIEWSCREEN".
  Modify "case CAMERA1-CAMERA4-CAMERAPOLE" to identify cameras linked to
  portals and hide them. Also add "case PRTLTELEPDEST" to hide them too.
  ***portals.c*** contains code to draw portals to their respectve tiles.
  you can add it right in game.c before "displayrooms" but it was decided
  to put them out in a separate file for an ease of coding. Make sure that
  if you changed portals.c, you must delete "game.obj" before recompile,
  otherwise the changes are not going to become.
  ***sector.c***. Modify function "void checksectors", the very beginning
  of that function contains a code of portals to teleport player in 3D
  space. That means it will not teleport you if you jump above it (made
  specially to allow for better 3D like windows and doors place on facade
  of multistory buildings). That makes it possible to stack many different
  portals in the same X,Y space but different height so you can have
  PORTAL0-1 pair on story #3 and PORTAL2-3 pair on story #4 for example
  (I didn't implement more than 1 pair though). Make sure that this version
  only teleports from a CENTER of a sprite, so that's a disatvantage I'm
  looking forward to fix...
  ***names.h, soundefs.h, user.con*** include new cases PORTAL0 4960,
  PORTAL1 4961, PRTLTELEPDEST 5062.
  ***engine.c, eng386.c***. Modify "drawsprite and drawsprite_LQ2X" functs
  in order to depict texture contents on flats sprites like parascan does
  to floors and ceilings "parallax textures" but inside a trapezoid frame,
  also known as a "wall aligned flat sprite". I'd like to do the same to
  ground aligned sprites so that we could have holes in floors, ceilings.
            --- implementation details or how it works finish ---

===========================================================================
v0.20 features (April 11, 2026):
- first, initial GitHub upload, forked from "videogamepreservation" archive
  https://github.com/videogamepreservation/dukenukem3d;
- more stable and a bit faster integer slopes due to increased reciptables
  size and newton-raphons krecip iterations instead of idiv to prevent
  division by zero errors;
===========================================================================
v0.19 features (March 17, 2026, 01:30AM):
- integer slopes optimization via adaptive step size (32px for closeup,
  16px for medium distances, 8px for distant slopes);
===========================================================================
v0.18 features (March 17, 2026, 01:30AM):
- much better integer slopes for all the versions included;
===========================================================================
v0.17 features:
- "distance-slow" optimization. Turn on by /distslow command-line parameter
   allows to speed-up rendering on big open areas by rendering distant
   areas 8 times less often. Causes rendering artifacts since simple
   "drawalls" modification in engine. Both regular and 386 version are
   bundled with it.
===========================================================================
v0.16 features:
- further enhancements of low detail mode, especially "drawsprite" function
  doesn't introduce unwanted bending to the farthest bounds of a sprite,
  e.g. no more crooked-mirror effect. Depth sorting issues and infinitely
  tall repeating on sprites are now fixed, thus "trasmach4" pixel
  duplicating asm function is now on. Transparent wall sprites aren't yet
  rendered.
- mact386.bin diagonal mouse movement slowdown fixed. File mact386.lib
  was patched with "Build Engine Mouse Fix v1.1" exe tool. No more need
  to patch it when exe's are built because the mact386.lib itself changed.
  The tool is made by "CTPAX" (fear, RUS.):
  _CTPAX_@MAIL.RU, ctpax-cheater.losthost.org
===========================================================================
v0.15 features:
- the original game comes with its own y-parallax scrolling defined as
  "parallaxyscale" in engine.c in "parascan" function and premap.c
  "setupbackdrop" function. Setting it to 65535 does the trick, thus
   I set my approach parallax scroll speed to 0 to disable it. So we have
   nice original smooth parallax effect and height tracking parallax
   per player. Also, my approach wasn't properly reset per level so that
   I added "g_skyycuroffs" variable, initialized it to 0 and assigned 
   all sky cases offsets in premap.c. Each time a level is loaded, the
   offset resets to those defined in premap.c per "g_skyycuroffs" value.
- an attempt to implement LQ3X mode was made. It looks even worse and
  there's no speed gain unlike LQ2X mode, thus not compiled. Set "pxlzefctr"
  variable to "3" in eng386.c, run "MakeLQ3.bat".
===========================================================================
v0.14 features:
- further enhancements on LQ2X mode, a lot has been done but there's
  a room for improvements. Now that it works just as fast as ingame low
  detail mode it looks crisper and much faster when window size is shrunk.
  Unfortunately, the old default ingame mode is ineffective on little
  windows size because it always renders a larger 160x100 view and then
  stretches up or down the rendered view onto 320x200 screen and does
  double job which makes the game perform slower. 
- correct "bswap" 386 asm instructions by Tronix, so mirrors are ok
- the way to fill unprocessed fields in asm has been found and unlike
  left (-1, -2 etc.) or right (+1, +2 etc.) writes, there's a need to know
  screen width in pixels. It could be done by loading "_xdim" variable in
  asm functions but it's out of scope for the moment and "320" constant is
  hardcoded. Read ALQ2X.ASM for details. Enabling transmach4 pixel doubling
  logic brings the light on "drawsprite_LQ2X" function issues in eng386.c.
  There's a need to polish it further.
- flat vertical sprites with transparency don't render yet.
===========================================================================
v0.13 features:
- an attempt to compile the game entirely for 386 with all code
  and assembly refactored to do so (mostly bswap instruction replaced and
  a different compilation path). Also, all occurences of "Nice try" in
  eng386.c were commented out (disabled).
- an attempt to implement low detail like it's done in Doom. Duke3D does it
  by rendering a 160x100 tile and then stretching it up to 320x200 by using
  the same drawing functions that high detail mode does. It's pretty
  ineffective. Now we want to do it like in doom, e.g. call to process
  stuff like walls, floors, ceilings, sprites and sky twice as less but
  interpolate the unprocessed fields. Only walls and skies are done correct
  yet (skies use wallscan function to draw).
- it looks bad yet
===========================================================================
v0.12 features:
- finally an "interpolated" way to do slopes with /nofpu flag.
  Ken's unfinished "slopevlin2" approach now renamed to #0.
  "slopevlin" duplicated and now is #2 approach, has its own
  precision parameters, and most importantly, simulateneous pixel writes
  to the sides, allowing to "interpolate" only those columns that were
  requested in grouscan_nonfpu function in engine.c, frame parity removed
  for a while but the file is still there if you will - engaltgs.c.
  The a.asm changes:

        ; we now add this thing above
        mov [ebp-1], al ; Pixel to the left - 1
        ;the original line below:
        mov [ebp],   al ; Center Pixel
        ; we now add this thing beneath
	mov [ebp+1], al ; Pixel to the right + 1
        
        you can iterate more to fill in the gaps
===========================================================================
v0.11 features:
- up to 768000 bytes MIDI playback.

- up to 10 custom 4-tile 256px width panoramic skies (defined
  in DEFS.CON). Make sure for the names SKYCUSTOM1-SKYCUSTOM10
  to stay the same. If you use a tile #3840, make sure you have
  3 more tiles space left in your art file, e.g. #3841,3842,3843
  must be free to fit remainig 3 parts of your sky.
===========================================================================
v0.1 features:
- y-sky parallax shearing based on player's look up/down and slopes if 
  mouse aims off. Player's height relative to the world is also tracked.
  Integer math only version turned on by default as floats are useless but
  the code without integer optimization is commented and included.
  Must be done really wrong and is not very smooth.

- /nofpu command line parameter to speed up rendering on really old and
  slow 486 or even 386 CPUs without FPU. At this moment I couldn't
  implement anything better than interleaved ticker dependent scanline
  alternation. In engine.c, basically only grouscan and drawalls functions
  are modified to handle different rendering paths. I can't modify this
  thing any further at the moment unfortunately.

- playanm function enhanced and now correctly removes played ANM files 
  from the RAM (cache) via "suckcache(anim_t_internal);" command. Turns out
  original Duke didn't do that at all because "free animation" command
  referenced to the one defined in animlib.c and it was commented out
  (disabled) for a good reason - it used John Carmack's zone.c hidden
  in "mact386.lib" memory handler which isn't compatible with Duke3D's
  cache1d.c system. All sounds are now stopped after animation is played.

- playanm_split function is present in the code, tested but unused. Can be
  used to split a large ANM file and play in one go, one after another and
  skipped by pressing Enter, Space or Escape just once. Messes up the
  palette a little-bit after the first animation was played. Unloads
  a played ANM file from RAM (cache) before playing another one in the
  sequence. Useful to play large anm files with as much lower RAM
  consumption as possible.

- animlib modifications to untie it from "mact386.lib" stuff with the
  same namespace. "_internal" suffix added to each of the functions.

- correct stop all sounds after a bonus screen ( finish level screen ).
===========================================================================



Terry allowed to publish DOS4g binder?
There must be more than that - Terry
allowed to publish sources on some
git project I can't remember but
they're NOT included here anyway


by Darkcrafter07
darkcrafter774@gmail.com

===========================================================================






===================================================================
Duke Nukem 3D (v1.5 CD Version) Source Code Release - April 1, 2003
===================================================================

LEGAL STUFF
-----------

"Duke Nukem" is a registered trademark of Apogee Software, Ltd. (a.k.a. 3D Realms).
"Duke Nukem 3D" copyright 1996 - 2003 3D Realms.  All trademarks and copyrights reserved.

This is the complete source code for Duke Nukem 3D version 1.5, buildable as detailed in the next section.

The code is licensed under the terms of the GPL (gnu public license).  You should read the entire license (filename "GNU.TXT" in this archive), so you understand your rights and what you can and cannot do with the source code from this release.

All of the Duke Nukem 3D data files remain copyrighted and licensed by 3D Realms under the original terms.  You cannot redistribute our data or data files from the original game.  You can use our data for personal entertainment or educational purposes.  If you need the data, you can order a Duke Nukem 3D CD from our store on our website.

Please do not contact us for possible commercial exploitation of Duke Nukem 3D -- we will not be interested.

Please note that this is being released without any kind of support from Apogee Software, Ltd / 3D Realms Entertainment.  We cannot help in getting this running and we do not guarantee that you will be able to get it to work, nor do we guarantee that it won't blow up your computer if you do try and use it.  Use at your own risk.

SPECIAL THANKS
--------------

First, we'd like to thank all the fans of 3D Realms and Duke Nukem.  We wouldn't be where we are without you and without your support.

Second, we'd like to thank Charlie Wiederhold for putting this release together.  We had decided some time ago to release the source code, but people are busy and it always seemed to be low priority.  A couple of weeks ago, Charlie got a source archive from Scott, and came back a few days later with it all compiling.

Thanks also to Ken Silverman and Jim Dose for allowing us to include some of their source in this build, so we have it all in one archive.

Finally, we'd like to thank the whole Duke Nukem Forever team.  These guys are all working incredibly hard on the next installment of Duke Nukem.

Enjoy the source code.  We can't wait to see what people do with it.  And we really look forward to being able to play the game under XP, with sound, internet play, hardware acceleration and whatever else you're clever enough to put in :)

George Broussard & Scott Miller
3D Realms

======================================================
HOW TO COMPILE THE SOURCE CODE (by Charlie Wiederhold)
======================================================

This source code release was compiled on March 20th, 2003 using the materials in this archive.

Duke was originally written with the Watcom 10.0 C/C++ compiler. This code has been updated to allow it to compile under the free version of the Watcom compiler available from their webpage (http://www.openwatcom.org).

It was compiled under both Open Watcom 1.0 and Watcom 11.0c for Windows.  Both are free to download, and I didn't notice much of a difference between the two when using them. This means, thankfully, that anybody can work with this code right away without trying to find an out of production compiler or wait for someone to port it to other modern compilers.

Step 1) Install Watcom C/C++ onto your system.

Step 2) When you install, make sure that you select DOS 32 Bit as one of your target environments to build for.

Step 3) Choose a place you want to work from and unzip the contents of this .ZIP file into that directory.

From here you have two choices on how to work. You can use the command line compiler that comes with Watcom, or you can use the IDE (basically the development studio that manages your files, options for compiling, editing files, debugger, etc). Whichever one you choose depends on what you are comfortable with.

IF YOU USE THE COMMAND LINE COMPILER
------------------------------------

- In a Command Prompt, go into the Source directory where you should find a MAKEFILE. and a MAKEFILE.LNK file.
- At the Command Prompt type "wmake" or "wmake makefile" without the quotes.
- This should compile and create a DUKE3D.EXE file which you can then copy into the directory with your Duke 3D data and run.

IF YOU USE THE IDE
------------------

- Start up the Watcom IDE and go to File -> Open Project.
- Find the directory where you've unzipped the Source files into, and you should see a DUKE3D.WPJ, select this and hit "OK".
- Click the "Make Current Target" button, or press "F4" or go to the menu Targets -> Make. You'll see lots of Warnings as it compiles, that's normal.
- This will create a DUKE3D.EXE file in the same directory where the DUKE3D.WPJ was located, which can then be copied in the directory with your Duke 3D data and run.

ONCE YOU HAVE DUKE3D.EXE COMPILED
---------------------------------

- If you own Duke 3D version 1.5, you are set... simply copy your new .EXE into the directory and run it.
- If you own Duke 3D version 1.4 or 1.3d, follow the same steps below except don't download the shareware data.
- If you own neither versions: Download the shareware version of Duke 3D from http://www.3drealms.com (go to Downloads).
- In the directory you have Duke 3D installed now, copy the four files in the TESTDATA directory into your Duke 3D directory. Now you should be able to play the game well enough to test, though unfortunately there will still be some minor issues. Your best results will come from owning a copy of Duke 3D version 1.5, which can still be purchased from the 3D Realms website.

This should be enough to get you started. Unfortunately nobody at 3D Realms will be able to devote the time to answer any support questions if you do encounter problems. However, go to http://www.3drealms.com/forums.html and you will find people discussing the source code in the Duke 3D Source category and able to answer questions. I will try to answer extremely common questions there shortly after the release, but I promise, within a very short time the community will outgrow my knowledge/understanding of this source and be better suited to answer any questions itself.

MISC NOTES ABOUT THIS RELEASE
-----------------------------

- All changes I made from the original are indicated by a "// CTW" style comment. In all but one case I commented out the original code so that it would still be there. I made as few changes as possible since the fun for people is working on the Duke 3D original code, not my personal rewrite/interpretation.

- Unfortunately the source to the SETUP.EXE and SETMAIN.EXE programs used for creating setup configuration files for Duke 3D is nowhere to be found, so if you need a setup utility to create custom .CFG files you'll need to write your own. If we find the source code we'll update this release to include that as well. The code to read .CFG files is within the Duke 3D CONFIG.C source itself, so it gives you something to start with.

- This source includes the Build Engine data (.OBJ files) needed for compiling. The Build Engine is a seperate entity from Duke 3D, though Duke 3D is built upon the Build Engine.

- The BUILDENGINE.ZIP file contains all of the data you need from Ken Silverman's released version to compile the Build Engine itself. Instructions for doing this are contained within that file. I have tested this with the free versions of Watcom and it works. More information about this code at Ken's webpage here: http://www.advsys.net/ken/buildsrc/default.htm

- The AUDIOLIB.ZIP file contains all of the data you need from Jim Dose's sound engine that was used in both Rise of the Triad and Duke 3D to compile the actual Audio Library itself. Instructions for doing this are contained in that file. I have not tested this personally to see if it compiles under the free version of Watcom.

- Sound will be sketchy if you are on any modern system. It does work, but only as well as the original Duke 3D's does.

- Networking works. Scott Miller and I fired up a quick game and it worked fine. The only catch is you need to disable sound for this to work in modern systems. No testing over a modem was done.

- The original .DMO demo files do not work. However recording new ones and playing those back does work, sometimes. I've disabled playback of demos in the source for stability when people first try to run their compiled version. It shouldn't be too hard to find how to re-enable it for those who want to go in and fix it.

- If you would like to play Duke 3D in high res Vesa modes instead of 320x200, download Ken's utility here: http://www.advsys.net/ken/build.htm

- The files in the EXTRAS folder are there purely for curiosity and educational purposes. Things like Allen Blum's experiments with room over room can be found in there, as well as older versions of the source files that ultimately made it into the game. None of these are necessary and are purely, well, extra.

- Duke 3D used DOS/4GW for it's DOS Extender. Watcom 1.0/11.0c comes with a couple free DOS Extenders, however you will need to bind it to the extender in order to distribute the EXE you create to other computers where it wasn't compiled. How you do this depends on which extender you choose to use. I trust that once you get to the point of distributing an EXE you can figure out how to bind it to the extender. Since DOS/4GW was a commercial licensed product, we can't distribute the resources we used to do this.

- All references to TEN (Total Entertainment Network) have been commented out of the version that will be compiled, but left in for the curious to look at.

- This page was an invaluable learning tool for me when working on preparing the source code for release: http://www.clipx.net/ ... The actual information I used can be found here: http://www.clipx.net/ng/zcpp/index.php

- I started working on this having no knowledge of Watcom, DOS based C programming, etc. and got it up and going within a night (and a half...ish) after work hours. So if you have a basic understanding of programming, you shouldn't have much trouble getting this up and running.

- The first things I think everyone would like to see done is an update to the sound engine and an update to the networking to allow play over the internet. But there is no end to the changes people can make, so everyone at 3D Realms looks forward to what people will do now that the source code is finally out there. It's been a long time coming (too long probably), but here you go, have fun!

Charlie Wiederhold
3D Realms