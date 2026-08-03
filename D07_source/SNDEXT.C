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

//=========================================================================
// SNDEXT.C - Double-Buffered GRP audio streaming subsystem.
// Aka "sounds.c extended". Plays long snd files with low RAM consumption.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>

#include "types.h"
#include "develop.h"
#include "util_lib.h"
#include "duke3d.h"
#include "sndcards.h"
#include "fx_man.h"
#include "sndext.h"

#define LOUDESTVOLUME 150

// External hardware and mixer config variables from Duke3D
extern int32 FXDevice, MusicDevice;
extern int32 FXVolume, MusicVolume;
extern int32 SoundToggle, MusicToggle;
extern int32 VoiceToggle, AmbianceToggle;
extern int32 NumVoices, NumChannels;
extern int32 NumBits, MixRate, MidiPort, ReverseStereo;

extern short soundm[];

// Globals for streaming pipeline tracker
static int   active_dma_buffer = 0;
static int32 stream_file_handle = -1;
static long  stream_buf_size = 32768;
static long  stream_file_pos = 0;
static char  audio_stream_active = 0;
static char  audio_stream_looped = 0;
static short active_stream_sound_id = -1;
static long  audio_header_offset = 0x20L;

// Static sound stream context (no allocache)
static sound_stream_t stream_ctx_static;
sound_stream_t *sound_stream_ctx = &stream_ctx_static;

// Set up static buffers
static byte static_stream_buffer_0[16384];
static byte static_stream_buffer_1[16384];

//===============================================================================
// FX_ServiceVocStream() - Background streaming service for double-buffered audio
//===============================================================================
void FX_ServiceVocStream(void)
{
    int shadow_page, handle;
    long bytes_to_read, bytes_read;

    if (!audio_stream_active || 
        sound_stream_ctx == NULL || 
        !sound_stream_ctx->playing)
        return;

    handle = active_stream_sound_id;
    
    // SAFETY: Validate handle bounds
    if (handle < 0 || handle >= NUM_SOUNDS) return;

    if (MV_VoicePlaying(sound_stream_ctx->voice_handle) == 0)
    {
        shadow_page = sound_stream_ctx->active_page ^ 1;

        // SAFETY: Check file handle validity
        if (sound_stream_ctx->file_handle == -1)
        {
            FX_StopVocStream();
            return;
        }

        if (sound_stream_ctx->remaining_bytes <= 0)
        {
            if (soundm[handle] & 1) 
            {
                sound_stream_ctx->remaining_bytes = soundsiz[handle] - audio_header_offset;
                klseek_stream(sound_stream_ctx->file_handle, audio_header_offset, SEEK_SET);
            }
            else
            {
                FX_StopVocStream();
                return;
            }
        }

        // SAFETY: Validate chunk size before playing
        if (sound_stream_ctx->current_chunk_size <= 0 || 
            sound_stream_ctx->current_chunk_size > 16384) return;

        sound_stream_ctx->voice_handle = MV_PlayRaw
        (
            (char *)sound_stream_ctx->audio_windows[shadow_page],
            sound_stream_ctx->current_chunk_size,
            MixRate,
            0,
            LOUDESTVOLUME,
            LOUDESTVOLUME,
            LOUDESTVOLUME,
            soundpr[handle], handle
        );

        sound_stream_ctx->active_page = shadow_page;

        bytes_to_read = min(16384L, sound_stream_ctx->remaining_bytes);
        memset((void*)sound_stream_ctx->audio_windows[shadow_page ^ 1], 0, 16384);

        bytes_read = kread_stream
        (
            sound_stream_ctx->file_handle,
            sound_stream_ctx->audio_windows[shadow_page ^ 1],
            bytes_to_read
        );

        if (bytes_read > 0)
        {
            sound_stream_ctx->remaining_bytes -= bytes_read;
            sound_stream_ctx->current_chunk_size = bytes_read;
        }
    }
}

//==========================================================================
// FX_StopVocStream() - Stop audio streaming and clear RAM.
//==========================================================================
void FX_StopVocStream(void)
{
    if (audio_stream_active)
    {
        if (sound_stream_ctx != NULL)
        {
            if (sound_stream_ctx->playing)
            {
                MV_Kill(sound_stream_ctx->voice_handle);
                sound_stream_ctx->playing = 0;
            }
            if (sound_stream_ctx->file_handle != -1)
            {
                kclose_stream(sound_stream_ctx->file_handle);
                sound_stream_ctx->file_handle = -1;
            }
            sound_stream_ctx = NULL; // Reset pointer
        }

        if (active_stream_sound_id != -1)
        {
            Sound[active_stream_sound_id].ptr = NULL;
            Sound[active_stream_sound_id].lock = 0;
            active_stream_sound_id = -1;
        }
        audio_stream_active = 0;
    }
}

//==========================================================================
// soundstream() - Stream a long sound file with low RAM consumption.
//==========================================================================
void soundstream(short num)
{
    int i;
    long bytes_read_1, bytes_read_2;
    long file_len, current_offset;
    short pitch, pitche, pitchs, cx;
    byte file_header_scratch[0x30];
    unsigned char voc_rate_byte;

    // Early exit if sound system is disabled or invalid
    if (FXDevice == NumSoundCards)             return;
    if (SoundToggle == 0)                      return;
    if (VoiceToggle == 0 && (soundm[num] & 4)) return;
    if ((soundm[num] & 8) && ud.lockout)       return;

    // Calculate pitch variation
    pitchs = soundps[num];
    pitche = soundpe[num];
    cx = klabs(pitche - pitchs);

    if (cx)
    {
        if (pitchs < pitche)   pitch = pitchs + (rand() % cx);
        else                   pitch = pitche + (rand() % cx);
    }
    else
    {
        pitch = pitchs;
    }

    // Stop any active stream
    if (audio_stream_active) FX_StopVocStream();

    // Initialize stream context safely
    memset(&stream_ctx_static, 0, sizeof(sound_stream_t));
    sound_stream_ctx = &stream_ctx_static;

    sound_stream_ctx->audio_windows[0] = static_stream_buffer_0;
    sound_stream_ctx->audio_windows[1] = static_stream_buffer_1;

    // Open the sound file
    sound_stream_ctx->file_handle = kopen4group_stream(sounds[num]);
    if (sound_stream_ctx->file_handle == -1) return;

    // Get file length and validate header
    file_len = klseek_stream(sound_stream_ctx->file_handle, 0L, SEEK_END);
    if (file_len <= audio_header_offset) {
        kclose_stream(sound_stream_ctx->file_handle);
        return;
    }

    klseek_stream(sound_stream_ctx->file_handle, 0L, SEEK_SET);
    kread_stream(sound_stream_ctx->file_handle, file_header_scratch, 0x2C);

    // UNIVERSAL CONTAINER DETECTOR RESOLUTION PATHWAY
    if (file_header_scratch[0] == 'R' && file_header_scratch[1] == 'I' &&
          file_header_scratch[2] == 'F' && file_header_scratch[3] == 'F')
    {
        // TARGET: STANDARD UNCOMPRESSED WAVE AUDIO (.WAV)
        sound_stream_ctx->sample_rate = *(unsigned long *)&file_header_scratch[0x18];
        audio_header_offset = 0x2CL; // WAV headers take up exactly 44 bytes
    }
    else if (file_header_scratch[0x14] == 1)
    {
        // TARGET: CREATIVE VOICE AUDIO (.VOC)
        voc_rate_byte = file_header_scratch[0x1A];
        if (voc_rate_byte != 256)
        {
            sound_stream_ctx->sample_rate = 1000000 / (256 - voc_rate_byte);
        }
        else
        {
            sound_stream_ctx->sample_rate = 11025;
        }
        audio_header_offset = 0x20L; // VOC standard audio blocks start past 32 bytes
    }
    else
    {
        // FALLBACK: Safe default resolution metrics pass
        sound_stream_ctx->sample_rate = 11025;
        audio_header_offset = 0x20L;
    }

    // Align file descriptors pointer strictly past the detected container header block boundary
    klseek_stream(sound_stream_ctx->file_handle, audio_header_offset, SEEK_SET);
    sound_stream_ctx->remaining_bytes = file_len - audio_header_offset;
    sound_stream_ctx->chunk_size = 16384L;
    sound_stream_ctx->current_chunk = 0;

    sound_stream_ctx->total_chunks =
      (sound_stream_ctx->remaining_bytes + sound_stream_ctx->chunk_size - 1) / sound_stream_ctx->chunk_size;

    if (sound_stream_ctx->total_chunks > 128) sound_stream_ctx->total_chunks = 128;

    current_offset = audio_header_offset;
    for (i = 0; i < sound_stream_ctx->total_chunks; i++)
    {
        sound_stream_ctx->chunk_offsets[i] = current_offset;
        current_offset += sound_stream_ctx->chunk_size;
    }

    // Pre-fill startup double buffering page layouts safely
    memset(sound_stream_ctx->audio_windows[0], 0, 16384);
    memset(sound_stream_ctx->audio_windows[1], 0, 16384);

    bytes_read_1 = kread_stream(sound_stream_ctx->file_handle,
                               sound_stream_ctx->audio_windows[0], 16384);
    bytes_read_2 = kread_stream(sound_stream_ctx->file_handle,
                               sound_stream_ctx->audio_windows[1], 16384);

    if (bytes_read_1 <= 0) {
        kclose_stream(sound_stream_ctx->file_handle);
        audio_stream_active = 0;
        return;
    }

    sound_stream_ctx->remaining_bytes -= (bytes_read_1 + bytes_read_2);
    sound_stream_ctx->current_chunk_size = bytes_read_1;

    sound_stream_ctx->active_page = 0;
    sound_stream_ctx->playing = 1;
    active_stream_sound_id = num;
    audio_stream_active = 1;

    // FIRE DUAL CHANNEL RAW OUTPUT ALIGNED DIRECTLY TO HARDWARE MIXRATE
    sound_stream_ctx->voice_handle = MV_PlayRaw
    (
        (char *)sound_stream_ctx->audio_windows[0],
        sound_stream_ctx->current_chunk_size,
        MixRate,
        pitch,
        LOUDESTVOLUME,
        LOUDESTVOLUME,
        LOUDESTVOLUME,
        soundpr[num], num
    );
}

//=============================================================================
// Low-level hardware DSP control for Sound Blaster with precise config routing
// Allows to pause playing sounds without stopping them by hacking SB internals
//=============================================================================
void pausesound(int toggle) // unused yet but it works
{
    word dsp_base;
    word dsp_write_port;

    if (FXDevice == NumSoundCards || SoundToggle == 0) 
    {
        return;
    }

    // Dynamic Hardware Binding: Pull the true active base address from BlasterConfig struct
    dsp_base = (word)BlasterConfig.Address; 
    dsp_write_port = dsp_base + 0xC; // DSP Write Buffer / Status register port

    // toggle == 1: HARDWARE DMA PAUSE (Freeze dynamic DSP sample stream progression)
    if (toggle == 1)
    {
        // Drop master volume via software mixer first to prevent any direct hardware clicks
        FX_SetVolume(0);

        // Wait for DSP write buffer to be ready
        while (inp(dsp_write_port) & 0x80);
        
        // Dynamic Hardware Fork based on config.c NumBits definition
        if (NumBits == 16)
        {
            outp(dsp_write_port, 0xD5); // Send command 0xD5 - Pause 16-bit DMA playback instantly
        }
        else
        {
            outp(dsp_write_port, 0xD0); // Send command 0xD0 - Pause 8-bit DMA playback instantly
        }
    }
    // toggle == 0: HARDWARE DMA RESUME (Unfreeze dynamic DSP sample stream progression)
    else
    {
        // Restore player's original configuration volume level from config.c variable
        FX_SetVolume(FXVolume);

        // Wait for DSP write buffer to be ready
        while (inp(dsp_write_port) & 0x80);
        
        // Dynamic Hardware Fork based on config.c NumBits definition
        if (NumBits == 16)
        {
            outp(dsp_write_port, 0xD6); // Send command 0xD6 - Continue 16-bit DMA playback seamlessly
        }
        else
        {
            outp(dsp_write_port, 0xD4); // Send command 0xD4 - Continue 8-bit DMA playback seamlessly
        }
    }
}

// Call like "pausesoundticks(120)" to make a 1s pause
void pausesoundticks(long ticks)
{
    long startclock;
    
    if (FXDevice == NumSoundCards || SoundToggle == 0) 
    {
        return;
    }
    
    pausesound(1);  // Pause immediately
    startclock = totalclock;
    
    // Wait for specified ticks (120 ticks = 1 second in Duke3D)
    while ((totalclock - startclock) < ticks)
    {
        // Small delay to prevent 100% CPU usage
        if ((totalclock - startclock) > 0)
        {
            // Just wait for time to pass
        }
    }
    
    pausesound(0);  // Resume after delay
}

