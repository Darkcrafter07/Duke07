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

// Supported sound stream container formats
#define FMT_UNKNOWN    0
#define FMT_VOC        1
#define FMT_WAV_PCM    2
#define FMT_WAV_ADPCM  3

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
    long next_chunk_size;           // Size of the next processing block
    int voice_handle;               // Returned active voice node token from MV_PlayRaw
    int active_page;                // Active toggle memory window selector (0 or 1)
    char playing;                   // Status flag monitoring state loops
    byte *audio_windows[2];         // Just two pointers - 8 bytes in overall
    unsigned long sample_rate;      // Extracted VOC sample rate (fixed per file)
    short current_stream_num;       // Current playing file ID (-1 if silence)
    int is_adpcm;                   // Flags: 1 - ADPCM, 0 - PCM
    int stream_format;              // Current detected format from the "FMT" defines
    int adpcm_predictor;            // Current predicted PCM16 sound level
    int adpcm_step_index;           // Current step table index (0..88)
    int adpcm_resample_acc;         // 
    int adpcm_bits;                 // Stream bit-depth: 2, 3 or 4 bits
    int adpcm_block_align;          // WAV block_align (bytes per IMA block)
    int adpcm_block_pos;            // Bytes consumed inside current block payload
    int adpcm_block_payload_size;   // Payload bytes per block (block_align - 4)
    long data_start_offset;         // Offset of first ADPCM block in file
    unsigned int adpcm_bit_buf;     // LSB-first bit-stream accumulator for 2/3/4/5-bit ADPCM
    int adpcm_bit_cnt;              // Valid unread bits currently held in adpcm_bit_buf
} sound_stream_t;

#pragma pack(pop)

// Public streaming API prototypes
void FX_PlaySoundStream(const char *filename, char looped_flag);
void FX_ServiceVocStream(void);
void FX_StopVocStream(void);
void soundstream(short num);
void pausesound(int toggle);
void pausesoundticks(long ticks);

char FX_FullRAMLoad_ADPCM_DVI_WAV(long fp, long file_len, char **out_ptr, unsigned char *lock_ptr, long *out_size);

void FX_BatchConvertSoundsToADPCM(int target_bits);
void FX_BatchConvertADPCMToPCM(int out_wav_mode); // 0 = VOC, 1 = PCM WAV

// Global stream context pointer shared across the subsystem
extern sound_stream_t *sound_stream_ctx;

#endif // _SNDEXT_STREAM_PUBLIC_
