//-------------------------------------------------------------------------
/*
Copyright (C) 1996, 2003 - 3D Realms Entertainment
Copyright (C) 2026 - Darkcrafter07

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

// ANIMLIB.C - Deluxe Paint 2 animation ANM files player.
// Features entire file RAM load and low-RAM stream (64KB RAM) modes.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "types.h"
#include "develop.h"
#include "util_lib.h"
#include "animlib.h"

extern char fullanmramload; // declared in menues.c

// Globals
anim_t *anim = NULL;
static char Anim_Started = 0; 

// Isolated mem seg for stream mode 2 to prevent structures distortion
static uint16 static_streampage[0x8000];

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
        if (anim->LpArray[i].baseRecord <= framenumber && 
            anim->LpArray[i].baseRecord + 
            (anim->LpArray[i].nRecords & 0x3FFF) > framenumber)
        {
            return i;
        }
    }
    return i;
}

// Portable 32-bit flat mode decompression engine
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

    // shortSkip processing path
    dstP += cnt; 
    goto nextOp;

dump:
    while (cnt--)
    {
        *dstP++ = *srcP++;
    }
    goto nextOp;

run:
    wordCnt = (byte)*srcP++; 
    pixel = *srcP++;
    
    while (wordCnt--)
    {
        *dstP++ = pixel;
    }
    goto nextOp;

longOp:
    wordCnt = *((uint16 *)srcP);
    srcP += sizeof(uint16);
    if ((int16)wordCnt <= 0) goto notLongSkip; 

    // longSkip processing path
    dstP += wordCnt;
    goto nextOp;

notLongSkip:
    if (wordCnt == 0) goto stop;

    wordCnt -= 0x8000; 
    if (wordCnt >= 0x4000) goto longRun;

    // longDump handling loop sequence
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
    int i, destframe;
    long offset;
    byte *ppointer;

    CheckAnimStarted("renderframe");
    offset = 0;
    destframe = framenumber - anim->curlp.baseRecord;

    for (i = 0; i < destframe; i++)
    {
        offset += pagepointer[i];
    }

    ppointer = (byte *)pagepointer;
    ppointer += (anim->curlp.nRecords * 2) + offset;

    if (ppointer)
    {
        ppointer += (4 + (*((uint16 *)ppointer) & 1));
    }
    else
    {
        ppointer += 4;
    }

    CPlayRunSkipDump((char *)ppointer, (char *)anim->imagebuffer);
}

// HIGH-COMPATIBILITY PIPELINE DISPATCHER:
// Execute both modes cleanly without overlapping memory fields
void drawframe(uint16 framenumber)
{
    int32 size;
    long page_offset;
    uint16 pagenum;
    byte *file_ptr;

    CheckAnimStarted("drawframe");
    pagenum = findpage(framenumber);
    size = sizeof(lp_descriptor);

    if (fullanmramload)
    {
        // --- MODE 1: PURE MONOLITHIC RAM TRACKING ---
        page_offset = 0xB00L + ((long)pagenum * 0x10000L);
        file_ptr = anim->buffer + page_offset;
        
        memcpy(&anim->curlp, file_ptr, size);
        file_ptr = anim->buffer + page_offset + size + 2;
        anim->curlpnum = pagenum;

        renderframe(framenumber, (uint16 *)file_ptr);
    }
    else
    {
        // --- MODE 2: ULTRA LOW-RAM DISK STREAMING ---
        if (anim->curlpnum != pagenum)
        {
            anim->curlpnum = pagenum;
            page_offset = 0xB00L + ((long)pagenum * 0x10000L);

            klseek_stream(anim->file_handle, page_offset, SEEK_SET);
            kread_stream(anim->file_handle, &anim->curlp, size);

            klseek_stream(anim->file_handle, page_offset + size + 2, SEEK_SET);
            kread_stream
            (
                anim->file_handle, static_streampage, 
                anim->curlp.nBytes + (anim->curlp.nRecords * 2)
            );
        }

        renderframe(framenumber, static_streampage);
    }
}

// Forward declarations of isolated parsers
void ANIM_LoadAnimFullram(char *buffer);
void ANIM_LoadAnimStream(char *filename);

// Unified Entry Wrapper called straight from playanm context hooks
void ANIM_LoadAnim(char *buffer_or_filename)
{
    Anim_Started = 1;
    if (fullanmramload) ANIM_LoadAnimFullram(buffer_or_filename);
    else                ANIM_LoadAnimStream(buffer_or_filename);
}

// Pre-assigned buffer pipeline mapping (MODE 1)
void ANIM_LoadAnimFullram(char *buffer)
{
    int i;
    long header_size, array_size;
    byte *pal_source_with_mandatory_padding;

    anim->buffer = (byte *)buffer; 
    anim->file_handle = -1;
    anim->curlpnum = 0xffff;
    anim->currentframe = -1;

    header_size = sizeof(lpfileheader);
    memcpy(&anim->lpheader, anim->buffer, header_size);

    pal_source_with_mandatory_padding = anim->buffer + header_size + 128L;
    for (i = 0; i < 768; i += 3)
    {
        anim->pal[i + 2] = *pal_source_with_mandatory_padding++;
        anim->pal[i + 1] = *pal_source_with_mandatory_padding++;
        anim->pal[i]     = *pal_source_with_mandatory_padding++;
        
        // Explicitly SKIP mandatory 4th RGBA byte from RAM layout
        pal_source_with_mandatory_padding++; 
    }

    array_size = sizeof(anim->LpArray);
    memcpy(&anim->LpArray, anim->buffer + (long)anim->lpheader.lpfTableOffset, array_size);
}

// High performance runtime hardware storage seeker initialization (MODE 2)
void ANIM_LoadAnimStream(char *filename)
{
    int i;
    long header_size, array_size;

    if (anim == NULL)
    {
        allocache((long *)&anim, sizeof(anim_t), (char *)&Anim_Started);
    }

    anim->file_handle = kopen4group_stream(filename);
    if (anim->file_handle == -1)
    {
        anim->file_handle = kopen4load(filename, 0);
        if (anim->file_handle == -1) return;
    }

    anim->buffer = NULL;
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
        
        // SELF-DOCUMENTING SYSTEM CALL: Mandatory 4-byte stream alignment skip
        klseek_stream(anim->file_handle, 1L, SEEK_CUR); 
    }

    array_size = sizeof(anim->LpArray);
    klseek_stream(anim->file_handle, (long)anim->lpheader.lpfTableOffset, SEEK_SET);
    kread_stream(anim->file_handle, &anim->LpArray, array_size);
}

void ANIM_FreeAnim(void)
{
    if (anim != NULL)
    {
        if (anim->file_handle != -1)
        {
            kclose_stream(anim->file_handle);
            anim->file_handle = -1;
        }
        anim->buffer = NULL;
        anim = NULL;
    }
    Anim_Started = 0;
}

int32 ANIM_NumFrames(void)
{
    CheckAnimStarted("NumFrames");
    return anim->lpheader.nRecords;
}

byte *ANIM_DrawFrame(int32 framenumber)
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

byte *ANIM_GetPalette(void)
{
    CheckAnimStarted("GetPalette");
    return anim->pal;
}
