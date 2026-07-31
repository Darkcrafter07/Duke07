//-------------------------------------------------------------------------
/*
Copyright (C) 1996, 2003 - 3D Realms Entertainment

This file is part of Duke Nukem 3D version 1.5 - Atomic Edition

Duke Nukem 3D is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Original Source: 1996 - Todd Replogle
Prepared for public release: 03/21/2003 - Charlie Wiederhold, 3D Realms
*/
//-------------------------------------------------------------------------

// ANIMLIB.H - Public structures and function definitions for Duke ANM Video

#ifndef _animlib_public_
#define _animlib_public_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "types.h"
#include "develop.h"
#include "util_lib.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1)

// Structure declarations for deluxe animate large page files
typedef struct
{
    uint32 id;                 // 4 character ID == "LPF "
    uint16 maxLps;             // max # largePages allowed. 256 FOR NOW.
    uint16 nLps;               // # largePages in this file.
    uint32 nRecords;           // # records in this file.
    uint16 maxRecsPerLp;       // # records permitted in an lp. 256 FOR NOW.
    uint16 lpfTableOffset;     // Absolute Seek position of lpfTable. 1280 FOR NOW.
    uint32 contentType;        // 4 character ID == "ANIM"
    uint16 width;              // Width of screen in pixels.
    uint16 height;             // Height of screen in pixels.
    byte   variant;            // 0==ANIM.
    byte   version;            // 0==18 cycles/sec, 1==70 cycles/sec.
    byte   hasLastDelta;       // 1==Last record is a delta from last-to-first frame.
    byte   lastDeltaValid;     // 0==The last-to-first delta should be ignored.
    byte   pixelType;          // 0==256 color.
    byte   CompressionType;    // 1==(RunSkipDump) Only one used FOR NOW.
    byte   otherRecsPerFrm;    // 0 FOR NOW.
    byte   bitmaptype;         // 1==320x200, 256-color.
    byte   recordTypes[32];    // FIXED: Restored original array dimension boundary!
    uint32 nFrames;            // actual frames count inside the archive
    uint16 framesPerSecond;    // Number of frames to play per second.
    uint16 pad2[29];           // FIXED: Restored original pad buffer alignment!
} lpfileheader;

// Large page structural descriptor entry segment mapping
typedef struct
{
    uint16 baseRecord;         // Number of first record in this large page.
    uint16 nRecords;           // Number of records in lp (bits 15/14 flags).
    uint16 nBytes;             // Total number of bytes of contents, excluding header.
} lp_descriptor;

// Main monolithic memory block for streaming video context assets
typedef struct
{
    uint16        framecount;    // current frame of anim
    lpfileheader  lpheader;      // file header will be loaded into this structure
    lp_descriptor LpArray[256];  // FIXED: Restored full large page arrays block!
    uint16        curlpnum;      // initialize to an invalid Large page number
    lp_descriptor curlp;         // header of large page currently in memory
    uint16        thepage[0x8000]; // buffer where current large page is loaded
    byte          imagebuffer[0x10000]; // buffer where anim frame is decoded
    byte*         buffer;        // Stale pointer for compatibility structures mapping
    byte          pal[768];      // Video palette register array index mapping
    int32         currentframe;  // active timeline pointer frame step count
    int32         file_handle;   // PROTECTED STREAMING: Low-level file handle descriptor link
} anim_t;

#pragma pack(pop)

void  ANIM_LoadAnim(char *filename);
void  ANIM_FreeAnim(void);
int32 ANIM_NumFrames(void);
byte* ANIM_DrawFrame(int32 framenumber);
byte* ANIM_GetPalette(void);

extern anim_t *anim;

#ifdef __cplusplus
};
#endif

#endif // _animlib_public_
