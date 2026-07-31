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
along with this program; if not, write to the Free Softw4:53 20.06.2025are
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Original Source: 1996 - Todd Replogle
Prepared for public release: 03/21/2003 - Charlie Wiederhold, 3D Realms
*/
//-------------------------------------------------------------------------


// ANIMLIB.C - Monolithic Low-Memory Stream Player Implementation for DOS4GW


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "types.h"
#include "develop.h"
#include "util_lib.h"
#include "animlib.h"

// Globals
anim_t *anim = NULL;
static boolean Anim_Started = false;

void CheckAnimStarted(char *funcname)
{
    if (!Anim_Started)
    {
        Error("ANIMLIB_%s: Anim has not been initialized\n", funcname);
    }
}

// Given a frame number return the large page number it resides in
uint16 findpage(uint16 framenumber)
{
    uint16 i;

    CheckAnimStarted("findpage");
    for (i = 0; i < anim->lpheader.nLps; i++)
    {
        if (anim->LpArray[i].baseRecord <= framenumber && anim->LpArray[i].baseRecord + anim->LpArray[i].nRecords > framenumber)
        {
            return i;
        }
    }
    return i;
}

// Seek out and load the specific large page directly from GRP file handle stream
void loadpage(uint16 pagenumber, uint16 *pagepointer)
{
    long seek_pos;
    int32 size;

    CheckAnimStarted("loadpage");

    if (anim->curlpnum != pagenumber)
    {
        anim->curlpnum = pagenumber;
        size = sizeof(lp_descriptor);

        // PROTECTED DISK STREAMING: Re-base pointer calculations strictly to low-level file seeks
        seek_pos = 0xB00L + ((long)pagenumber * 0x10000L);
        
        klseek_stream(anim->file_handle, seek_pos, SEEK_SET);
        kread_stream(anim->file_handle, &anim->curlp, size);

        klseek_stream(anim->file_handle, seek_pos + size + 2, SEEK_SET);
        kread_stream(anim->file_handle, pagepointer, anim->curlp.nBytes + (anim->curlp.nRecords * 2));
    }
}

// Portable 32-Bit Flat Mode Port of James Dose Decompressor Engine
void CPlayRunSkipDump(char *srcP, char *dstP)
{
    uint16 wordCnt;
    signed char cnt;
    byte pixel;

nextOp:
    cnt = (signed char)*srcP++;
    if (cnt > 0)  goto dump;
    if (cnt == 0) goto run;

    cnt -= 0x80;
    if (cnt == 0) goto longOp;

    /* shortSkip processing path */
    dstP += cnt; 
    goto nextOp;

dump:
    // FIXED SAFE BOUNDS: Wrap to safe conditional decrement to break infinite zipping corruptions
    while (cnt--)
    {
        *dstP++ = *srcP++;
    }
    goto nextOp;

run:
    wordCnt = (byte)*srcP++; 
    pixel = *srcP++;
    
    // FIXED SAFE BOUNDS: Wrap to safe loop sequence to handle wordCnt == 0 correctly
    while (wordCnt--)
    {
        *dstP++ = pixel;
    }
    goto nextOp;

longOp:
    wordCnt = *((uint16 *)srcP);
    srcP += sizeof(uint16);
    if ((int16)wordCnt <= 0) goto notLongSkip; 

    /* longSkip processing path */
    dstP += wordCnt;
    goto nextOp;

notLongSkip:
    if (wordCnt == 0) goto stop;

    wordCnt -= 0x8000; 
    if (wordCnt >= 0x4000) goto longRun;

    /* longDump handling loop sequence */
    while (wordCnt--)
    {
        *dstP++ = *srcP++;
    }
    goto nextOp;

longRun:
    wordCnt -= 0x4000;
    pixel = *srcP++;
    while (wordCnt--)
    {
        *dstP++ = pixel;
    }
    goto nextOp;

stop:
    return;
}

// Render the frame specified from the large page currently in memory
void renderframe(uint16 framenumber, uint16 *pagepointer)
{
    byte *ppointer;
    long offset;
    int i, destframe;

    CheckAnimStarted("renderframe");
    offset = 0;
    destframe = framenumber - anim->curlp.baseRecord;

    for (i = 0; i < destframe; i++)
    {
        offset += pagepointer[i];
    }

    ppointer = (byte *)pagepointer;
    ppointer += (anim->curlp.nRecords * 2) + offset;

    if (ppointer[1])
    {
        ppointer += (4 + (((uint16 *)ppointer)[1] + (((uint16 *)ppointer)[1] & 1)));
    }
    else
    {
        ppointer += 4;
    }

    CPlayRunSkipDump((char *)ppointer, (char *)anim->imagebuffer);
}

void drawframe(uint16 framenumber)
{
    CheckAnimStarted("drawframe");
    loadpage(findpage(framenumber), anim->thepage);
    renderframe(framenumber, anim->thepage);
}

// Setup internal streaming anim data structure bounds without buffering entire file data
void ANIM_LoadAnim(char *filename)
{
    long header_size, array_size;
    int i;

    if (!Anim_Started) 
    {
        Anim_Started = true;
    }

    if (anim == NULL)
    {
        // FIX: Pass the address of our global static boolean flag instead of a volatile stack variable!
        // This ensures Ken's memory manager always references a valid operational memory address.
        Anim_Started = 1; 
        allocache((long *)&anim, sizeof(anim_t), (char *)&Anim_Started);
    }

    // Isolate active descriptor through uncached file handle streaming sequence
    anim->file_handle = kopen4group_stream(filename);
    if (anim->file_handle == -1)
    {
        anim->file_handle = kopen4load(filename, 0);
        if (anim->file_handle == -1) return;
    }

    anim->curlpnum = 0xffff;
    anim->currentframe = -1;
    header_size = sizeof(lpfileheader);

    klseek_stream(anim->file_handle, 0L, SEEK_SET);
    kread_stream(anim->file_handle, &anim->lpheader, header_size);

    // Extract palette channels natively from discrete position indexes
    klseek_stream(anim->file_handle, header_size + 128L, SEEK_SET);
    for (i = 0; i < 768; i += 3)
    {
        kread_stream(anim->file_handle, &anim->pal[i + 2], 1);
        kread_stream(anim->file_handle, &anim->pal[i + 1], 1);
        kread_stream(anim->file_handle, &anim->pal[i], 1);
        klseek_stream(anim->file_handle, 1L, SEEK_CUR); // Skip pad byte channel padding
    }

    // Extract large page descriptors array straight from table offset boundary
    array_size = sizeof(anim->LpArray);
    klseek_stream(anim->file_handle, (long)anim->lpheader.lpfTableOffset, SEEK_SET);
    kread_stream(anim->file_handle, &anim->LpArray, array_size);
}

// Call "suckcache(anim);" BEFORE and AFTER this one! (3 calls!)
void ANIM_FreeAnim(void)
{
    // Call "suckcache(anim);" BEFORE and AFTER this one! (3 calls!)
    if (Anim_Started)
    {
        if (anim != NULL)
        {
            if (anim->file_handle != -1)
            {
                kclose_stream(anim->file_handle);
                anim->file_handle = -1;
            }
            
            suckcache(anim);
            anim = NULL;
        }
        Anim_Started = false;
    }
}

int32 ANIM_NumFrames(void)
{
    CheckAnimStarted("NumFrames");
    return anim->lpheader.nRecords;
}

byte * ANIM_DrawFrame(int32 framenumber)
{
    int32 cnt;

    CheckAnimStarted("DrawFrame");
    if ((anim->currentframe != -1) && (anim->currentframe <= framenumber))
    {
        for (cnt = anim->currentframe; cnt < framenumber; cnt++)
        {
            drawframe(cnt);
        }
    }
    else
    {
        for (cnt = 0; cnt < framenumber; cnt++)
        {
            drawframe(cnt);
        }
    }
    anim->currentframe = framenumber;
    return anim->imagebuffer;
}

byte * ANIM_GetPalette(void)
{
    CheckAnimStarted("GetPalette");
    return anim->pal;
}
