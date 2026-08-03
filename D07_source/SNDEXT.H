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
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Original Source: 1996 - Todd Replogle
Prepared for public release: 03/21/2003 - Charlie Wiederhold, 3D Realms
*/
//-------------------------------------------------------------------------

// SNDEXT.H - Double-Buffered GRP audio streaming subsystem.
// Aka "sounds.c extended". Plays long snd files with low RAM consumption.

#ifndef _SNDEXT_STREAM_PUBLIC_
#define _SNDEXT_STREAM_PUBLIC_

#include "types.h"

#pragma pack(push, 1)



//**************************************************************************
// MULTIVOC SAMPLING SPEED AND RATE-SCALE COMPENSATOR RULE [DO's & DON'Ts]
//**************************************************************************
// DON'T: DO NOT pass raw extracted file frequencies (e.g. 11025Hz, 8713Hz)
//        directly into MV_PlayRaw() rate parameters.
//        Multivoc's raw injection pathway lacks a container header parser 
//        and completely bypasses James Dose native Time Constant formula:
//        samplespeed = 256000000L / (65536 - tc);
//        Passing file-specific rates here directly warps the assembly
//        RateScale shift registers, throwing pitch into a chaotic sub-cella
//        or making sounds chipmunk-accelerated, stuttering and broken.
//
// DO:    ALWAYS lock rate argument strictly to the game native hardware
//        mixing frequency: global variable "MixRate" (8,16,22,44kHz etc).
//        By feeding the double-buffering cache frame pages aligned
//        byte-for-byte to the core hardware mixer speed, forcing Multivoc
//        interpolator step size to a surgical 1.0 integer ratio.
//        This eliminates continuous pitch-shifting distortions,
//        locks frequency natively.
//**************************************************************************

// Advanced Dynamic RAW PCM Audio Stream Window Tracker
// Runtime voice configuration structure tracking active hardware slots
typedef struct
{
    long file_handle;               // Active stream file descriptor index
    long remaining_bytes;           // Total unread data ceiling inside track
    long chunk_offsets[128];        // Fixed dimension explicit array allocation bounds
    long total_chunks;              // Total extracted wave blocks inside soundtrack
    long current_chunk;             // Active chunk sequence running pointer
    long chunk_size;                // Base capacity of single cache block (16384 bytes)
    long current_chunk_size;        // Size of the currently processing block
    int voice_handle;               // Returned active voice node token from MV_PlayRaw
    int active_page;                // Active toggle memory window selector (0 or 1)
    char playing;                   // Status flag monitoring state loops
    byte *audio_windows[2];         // Just two pointers - 8 bytes in overall
    unsigned int sample_rate;       // Extracted VOC sample rate (fixed per file)
} sound_stream_t;

#pragma pack(pop)

// Public streaming API prototypes
void FX_PlaySoundStream(const char *filename, char looped_flag);
void FX_ServiceVocStream(void);
void FX_StopVocStream(void);
void soundstream(short num);
void pausesound(int toggle);
void pausesoundticks(long ticks);

// Global stream context pointer shared across the subsystem
extern sound_stream_t *sound_stream_ctx;

#endif // _SNDEXT_STREAM_PUBLIC_
