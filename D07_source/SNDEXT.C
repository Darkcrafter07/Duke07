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
//                           IMA ADPCM STUFF - START
//===============================================================================

// Low-level high-speed Intel/DVI IMA ADPCM Lookup Tables
static const int stepSizeTable[89] = 
{
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
    50, 55, 60, 66, 73, 80, 88, 97, 107, 118, 130, 143, 157, 173, 190, 209, 230,
    253, 279, 307, 337, 371, 408, 449, 494, 544, 599, 658, 724, 796, 876, 963,
    1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024, 3327,
    3660, 4026, 4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487,
    12635, 13899, 15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};

static const int indexAdjustTable2b[2] = { -1, 2 };
static const int indexAdjustTable3b[4] = { -1, -1, 2, 4 };
static const int indexAdjustTable4b[8] = { -1, -1, -1, -1, 2, 4, 6, 8 };

// Fixed-point i486 decoder for Intel/DVI ADPCM standard (2, 3, 4 bits)
static short DecodeDviSample(int code, int bits, int *predictor, int *step_index) 
{
    int i, step, vpdiff, sign_mask, mag_mask, sign, magnitude;

    sign_mask = 1 << (bits - 1);       // MSB is the sign bit
    mag_mask  = sign_mask - 1;         // Remaining bits are magnitude

    step = stepSizeTable[*step_index];
    sign = code & sign_mask;
    magnitude = code & mag_mask;

    // Pure Intel/DVI algorithmic quantization formula from SpanDSP standard
    vpdiff = 0;
    for (i = bits - 2; i >= 0; i--)
    {
        if (magnitude & (1 << i))
        {
            vpdiff += (step >> ((bits - 2) - i));
        }
    }
    vpdiff += (step >> (bits - 1));    // Add 0.5 rounding step fraction

    // Apply delta signed value directly to history predictor
    if (sign) 
    {
        *predictor -= vpdiff;
        if (*predictor < -32768) 
        {
            *predictor = -32768;
        }
    } 
    else 
    {
        *predictor += vpdiff;
        if (*predictor > 32767) 
        {
            *predictor = 32767;
        }
    }

    // Adjust step table index limits based on current bit depth
    if (bits == 2) 
    {
        *step_index += indexAdjustTable2b[magnitude & 1];
    }
    else if (bits == 3)
    {
        *step_index += indexAdjustTable3b[magnitude & 3];
    }
    else
    {
        *step_index += indexAdjustTable4b[magnitude & 7];
    }

    if (*step_index < 0)  *step_index = 0;
    if (*step_index > 88) *step_index = 88;

    return (short)(*predictor);
}

//===============================================================================
//                           IMA ADPCM STUFF - FINISH
//===============================================================================



//===============================================================================
// Container and format detector - execute it strictly ONCE on track start
//===============================================================================
// Encapsulated safe container analyzer - executes strictly ONCE on track start
static void MV_AnalyzeAndSetupStream(sound_stream_t *ctx, long file_len, long __far *out_header_offset)
{
    int scan_pos;
    unsigned char header_buffer[512];
    unsigned char voc_rate_byte;

    // Default fallbacks to prevent hardware locking
    ctx->stream_format    = FMT_UNKNOWN;
    ctx->is_adpcm         = 0;
    ctx->adpcm_bits       = 8;
    ctx->sample_rate      = 11025;
    ctx->adpcm_predictor  = 0;
    ctx->adpcm_step_index = 0;
    *out_header_offset    = 0x2C;

    if (ctx->file_handle == -1)
    {
        return;
    }

    // Pre-load file baseline header chunk securely into RAM block
    klseek_stream(ctx->file_handle, 0L, SEEK_SET);
    if (kread_stream(ctx->file_handle, header_buffer, 512) < 32)
    {
        return;
    }

    // 1. EVALUATE CREATIVE VOC CONTAINER SIGNATURE
    if (header_buffer[0] == 'C' && header_buffer[1] == 'r' && header_buffer[2] == 'e' && header_buffer[3] == 'a')
    {
        ctx->stream_format = FMT_VOC;
        *out_header_offset = (long)(*(unsigned short *)&header_buffer[0x14]);
        if (*out_header_offset <= 0)
        {
            *out_header_offset = 32;
        }
        
        if (header_buffer[*out_header_offset] == 0x01) 
        {
            char voc_rate_byte = header_buffer[*out_header_offset + 4];
            ctx->sample_rate = 1000000L / (256 - voc_rate_byte);
            *out_header_offset += 6; 
        }
        
        ctx->remaining_bytes = file_len - *out_header_offset;
        return;
    }

    // 2. EVALUATE MICROSOFT RIFF/WAVE CONTAINER SIGNATURE
    if (header_buffer[0] == 'R' && header_buffer[1] == 'I' && header_buffer[2] == 'F' && header_buffer[3] == 'F')
    {
        if (header_buffer[8] == 'W' && header_buffer[9] == 'A' && header_buffer[10] == 'V' && header_buffer[11] == 'E')
        {
            ctx->sample_rate = *(unsigned long *)&header_buffer[0x18];

            // Evaluate Audio Format field at offset 0x14 (0x0011 is Microsoft/Intel DVI ADPCM)
            if (*(unsigned short *)&header_buffer[0x14] == 0x0011)
            {
                ctx->stream_format = FMT_WAV_ADPCM;
                ctx->is_adpcm      = 1;
                ctx->adpcm_bits    = (int)(*(unsigned short *)&header_buffer[0x22]);

                // Intel/DVI Spec: Extract global baseline predictor from expanded format metadata chunks
                ctx->adpcm_predictor = (int)(header_buffer[0x2A] | (header_buffer[0x2B] << 8));
                if (ctx->adpcm_predictor & 0x8000) 
                {
                    ctx->adpcm_predictor |= ~0xFFFF; // Sign extension layout
                }
                ctx->adpcm_step_index = (int)header_buffer[0x2C];
                if (ctx->adpcm_step_index > 88) 
                {
                    ctx->adpcm_step_index = 88;
                }

                // SAFE IN-MEMORY SCANNER:
                // Find 'data' chunk header footprint to completely bypass Audition List/fact metadata tags.
                for (scan_pos = 12; scan_pos < 500; scan_pos++)
                {
                    if (header_buffer[scan_pos] == 'd' && 
                        header_buffer[scan_pos + 1] == 'a' && 
                        header_buffer[scan_pos + 2] == 't' && 
                        header_buffer[scan_pos + 3] == 'a')
                    {
                        *out_header_offset = (long)(scan_pos + 8); 
                        ctx->remaining_bytes = file_len - *out_header_offset;
                        return;
                    }
                }
                
                *out_header_offset = 0x3C; 
                ctx->remaining_bytes = file_len - *out_header_offset;
                return;
            }
            else
            {
                // Standard uncompressed PCM WAV configuration
                ctx->stream_format = FMT_WAV_PCM;
                *out_header_offset = 0x2C; 
                ctx->remaining_bytes = file_len - *out_header_offset;
                return;
            }
        }
    }
}

//====================================================================================
// A helper decompression function called from "soundstream" and "FX_ServiceVocStream"
//====================================================================================
static int MV_DecompressPage(sound_stream_t *ctx, unsigned char *output_buffer8, int bytes_to_read)
{
    int out_samples, p_pos, shift, code;
    int bytes_to_fetch, actual_file_read, target_samples;
    signed short sample_calced;        // Strictly signed short to force SAR sign-extension
    unsigned char current_byte;
    unsigned char packed_buffer[4096]; // Array size layout brackets explicitly locked!

    out_samples = 0;
    p_pos = 0;

    // CRITICAL PHASE SYNC FIX: 
    // Force reset the Bresenham timing accumulator strictly at the start of EVERY page
    // This eliminates micro-shifts between double-buffered chunks, reducing DC-offset and clicks.
    ctx->adpcm_resample_acc = 0;

    // Calculate approximate compressed bytes required for this hardware frame pass
    target_samples = (int)((bytes_to_read * ctx->sample_rate) / MixRate);
    if (target_samples > 16384) 
    {
        target_samples = 16384;
    }

    bytes_to_fetch = (target_samples * ctx->adpcm_bits) >> 3;
    if (bytes_to_fetch > 4096) 
    {
        bytes_to_fetch = 4096;
    }
    if (bytes_to_fetch < 128)
    {
        bytes_to_fetch = 128;
    }

    actual_file_read = kread_stream(ctx->file_handle, packed_buffer, bytes_to_fetch);
    if (actual_file_read <= 0) 
    {
        return 0;
    }

    sample_calced = (short)ctx->adpcm_predictor;

    // HIGH-SPEED SYNCHRONOUS BRESENHAM UNPACK LOOP: 
    while (out_samples < bytes_to_read)
    {
        // If we consumed the fetched disk buffer, reload the next piece seamlessly
        if (p_pos >= actual_file_read)
        {
            actual_file_read = kread_stream(ctx->file_handle, packed_buffer, bytes_to_fetch);
            if (actual_file_read <= 0)
            {
                break; // End of file reached safely
            }
            p_pos = 0;
        }

        current_byte = packed_buffer[p_pos++];

        // Extract DVI samples from Left to Right (MSB to LSB) matching Audition container format
        for (shift = 8 - ctx->adpcm_bits; shift >= 0 && out_samples < bytes_to_read; shift -= ctx->adpcm_bits)
        {
            code = (current_byte >> shift) & ((1 << ctx->adpcm_bits) - 1);

            sample_calced = (short)DecodeDviSample
            (
                code, 
                ctx->adpcm_bits, 
                &ctx->adpcm_predictor, 
                &ctx->adpcm_step_index
            );

            // Timeline accumulator steps forward by hardware mixer frequency scale ratio
            ctx->adpcm_resample_acc += MixRate;
            while (ctx->adpcm_resample_acc >= ctx->sample_rate && out_samples < bytes_to_read)
            {
                // Scale signed 16-bit into unsigned PCM8 (0..255) safely without integer overflow
                output_buffer8[out_samples++] = (unsigned char)((sample_calced >> 8) + 128);
                ctx->adpcm_resample_acc -= ctx->sample_rate;
            }
        }
    }

    // Security pass: If file ended prematurely, fill the rest with strict hardware PCM8 silence midpoint (128)
    while (out_samples < bytes_to_read)
    {
        output_buffer8[out_samples++] = 128; 
    }

    return out_samples;
}

//===============================================================================
// FX_ServiceVocStream() - Background streaming service for double-buffered audio
//===============================================================================
void FX_ServiceVocStream(void)
{
    int shadow_page, handle, next_page, bytes_to_fetch;
    int actual_file_read, out_samples, p_pos, code;
    int target_samples, shift, sample_calced;
    long bytes_to_read, bytes_read;
    unsigned char current_byte;
    unsigned char packed_buffer[4096];
    unsigned char *output_buffer8;

    if (!audio_stream_active || sound_stream_ctx == NULL || !sound_stream_ctx->playing)
    {
        return;
    }

    handle = active_stream_sound_id;
    if (handle < 0 || handle >= NUM_SOUNDS)
    {
        return;
    }

    // BRANCH 1: NEW PERFECT SCHEME FOR REGULAR UNCOMPRESSED PCM / VOC TRACKS
    if (sound_stream_ctx->is_adpcm == 0)
    {
        // Hardware sync check for regular PCM: Trigger when Multivoc drops the current page
        if (MV_VoicePlaying(sound_stream_ctx->voice_handle) == 0)
        {
            shadow_page = sound_stream_ctx->active_page ^ 1;

            if (sound_stream_ctx->file_handle == -1)
            {
                FX_StopVocStream();
                return;
            }

            // Verify if look-ahead buffer has valid chunks pre-buffered from previous frame pass
            if (sound_stream_ctx->next_chunk_size <= 0)
            {
                if (soundm[handle] & 1) // Loop handling check pass
                {
                    sound_stream_ctx->remaining_bytes = soundsiz[handle] - audio_header_offset;
                    klseek_stream(sound_stream_ctx->file_handle, audio_header_offset, SEEK_SET);
                    sound_stream_ctx->next_chunk_size = 16384; 
                }
                else
                {
                    FX_StopVocStream();
                    return;
                }
            }

            // ZERO-LATENCY INJECTION: Instantly play the completely pre-buffered layout
            sound_stream_ctx->voice_handle = MV_PlayRaw
            (
                (char *)sound_stream_ctx->audio_windows[shadow_page],
                sound_stream_ctx->next_chunk_size, 
                MixRate,
                0,
                LOUDESTVOLUME, LOUDESTVOLUME, LOUDESTVOLUME,
                soundpr[handle], 
                handle
            );

            sound_stream_ctx->active_page = shadow_page;

            // --- LOOK-AHEAD PRE-FILL BACKGROUND PHASE FOR NEXT FRAME ---
            next_page = shadow_page ^ 1; 
            bytes_to_read = min(16384L, sound_stream_ctx->remaining_bytes);

            memset((void*)sound_stream_ctx->audio_windows[next_page], 0, 16384);
            bytes_read = kread_stream(sound_stream_ctx->file_handle, sound_stream_ctx->audio_windows[next_page], bytes_to_read);

            if (bytes_read > 0)
            {
                sound_stream_ctx->remaining_bytes -= bytes_read;
                sound_stream_ctx->next_chunk_size = bytes_read;
            }
            else
            {
                sound_stream_ctx->next_chunk_size = 0;
            }
        }
    }
    // BRANCH 2: ORIGINAL OLD PATHWAY ROLLBACK STRICTLY FOR WAV IMA ADPCM TRACKS
    else
    {
        // Hardware sync check for ADPCM: Trigger pass strictly when active page is done
        if (MV_VoicePlaying(sound_stream_ctx->voice_handle) == 0)
        {
            shadow_page = sound_stream_ctx->active_page ^ 1;

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

            if (sound_stream_ctx->file_handle == -1)
            {
                FX_StopVocStream();
                return;
            }

            if (sound_stream_ctx->current_chunk_size <= 0 || sound_stream_ctx->current_chunk_size > 16384)
            {
                return;
            }

            // OLD ADPCM IMMEDIATELY PLAYS EXECUTED CHUNK BEFORE disk-read BLOCK
            sound_stream_ctx->voice_handle = MV_PlayRaw
            (
                (char *)sound_stream_ctx->audio_windows[shadow_page],
                sound_stream_ctx->current_chunk_size,
                MixRate,
                0,
                LOUDESTVOLUME, LOUDESTVOLUME, LOUDESTVOLUME,
                soundpr[handle],
                handle
            );

            sound_stream_ctx->active_page = shadow_page;

            bytes_to_read = min(16384L, sound_stream_ctx->remaining_bytes);
            memset((void*)sound_stream_ctx->audio_windows[shadow_page ^ 1], 0, 16384);

            // RUN THE ORIGINAL DECODER SEQUENCES ON THE FLY
            sound_stream_ctx->adpcm_resample_acc = 0; 
            output_buffer8 = (unsigned char *)sound_stream_ctx->audio_windows[shadow_page ^ 1];
            out_samples = 0;

            target_samples = (int)((bytes_to_read * sound_stream_ctx->sample_rate) / MixRate);
            if (target_samples > 16384)
            {
                target_samples = 16384;
            }

            bytes_to_fetch = ((target_samples * sound_stream_ctx->adpcm_bits) + 7) >> 3;
            if (bytes_to_fetch > 4096)
            {
                bytes_to_fetch = 4096;
            }

            actual_file_read = kread_stream(sound_stream_ctx->file_handle, packed_buffer, bytes_to_fetch);
            if (actual_file_read > 0)
            {
                p_pos = 0;
                sample_calced = sound_stream_ctx->adpcm_predictor;

                while (p_pos < actual_file_read && out_samples < target_samples)
                {
                    current_byte = packed_buffer[p_pos++];

                    for (shift = 8 - sound_stream_ctx->adpcm_bits; shift >= 0 && out_samples < target_samples; shift -= sound_stream_ctx->adpcm_bits)
                    {
                        code = (current_byte >> shift) & ((1 << sound_stream_ctx->adpcm_bits) - 1);

                        sample_calced = (int)DecodeDviSample(
                            code,
                            sound_stream_ctx->adpcm_bits,
                            &sound_stream_ctx->adpcm_predictor,
                            &sound_stream_ctx->adpcm_step_index
                        );

                        if (sample_calced > 32000)
                        {
                            sample_calced = 32000;
                        }
                        if (sample_calced < -32000)
                        {
                            sample_calced = -32000;
                        }

                        output_buffer8[out_samples++] = (unsigned char)((sample_calced + 32768) / 256);
                    }
                }

                while (out_samples < bytes_to_read)
                {
                    output_buffer8[out_samples++] = 128;
                }

                bytes_read = bytes_to_read;
                sound_stream_ctx->current_chunk_size = out_samples;
            }
            else
            {
                bytes_read = 0;
            }

            if (bytes_read > 0)
            {
                sound_stream_ctx->remaining_bytes -= bytes_read;
            }
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
            
            // Clear current stream index cache on stop release
            sound_stream_ctx->current_stream_num = -1;
            sound_stream_ctx = NULL; // Reset pointer
        }
        
        active_stream_sound_id = -1;
        audio_stream_active = 0;
    }
}

//==========================================================================
// soundstream() - Stream a long sound file with low RAM consumption.
//==========================================================================
void soundstream(short num)
{
    // COMPACT C89/C99 VARIABLES ALLOCATION BLOCK AT THE TOP
    int i, bytes_read_1, bytes_read_2, pitch, next_page, scan_pos;
    long real_data_start_offset, file_len;
    long current_offset, bytes_to_read, bytes_read;
    unsigned char file_header_scratch[48]; // 0x30 bytes array
    unsigned char header_buffer[512];

    // Check if the current audio streaming context is already playing this specific sound ID
    if (audio_stream_active && sound_stream_ctx != NULL && sound_stream_ctx->playing && active_stream_sound_id == num)
    {
        return;
    }

    // Hardware toggle and state check safety macros
    if (FXDevice == -1 || NumSoundCards == 0) 
    {
        return;
    }
    if (SoundToggle == 0) 
    {
        return;
    }
    if (VoiceToggle == 0 && (soundm[num] & 4)) 
    {
        return;
    }

    // Stop active streams to prevent dual hardware thread allocation racing
    if (audio_stream_active)
    {
        FX_StopVocStream();
    }

    // Reset static context structure parameters clean
    memset(&stream_ctx_static, 0, sizeof(sound_stream_t));
    sound_stream_ctx = &stream_ctx_static;
    active_stream_sound_id = num;

    // Direct buffer pointer link setup layout brackets
    sound_stream_ctx->audio_windows[0] = static_stream_buffer_0;
    sound_stream_ctx->audio_windows[1] = static_stream_buffer_1;

    // Initialize physical streaming handle index from engine group files
    sound_stream_ctx->file_handle = kopen4group_stream(sounds[num]);
    if (sound_stream_ctx->file_handle == -1)
    {
        return;
    }

    // Read target stream length boundaries
    file_len = klseek_stream(sound_stream_ctx->file_handle, 0L, SEEK_END);

    // === NEW ENCAPSULATED ANALYSIS PIPELINE ===
    klseek_stream(sound_stream_ctx->file_handle, 0L, SEEK_SET);
    kread_stream(sound_stream_ctx->file_handle, header_buffer, 512);
    
    // Extract base target sample frequency scaling layout
    sound_stream_ctx->sample_rate = *(unsigned long *)&header_buffer[0x18];

    // Evaluate container signature bounds (0x0011 indicates DVI IMA ADPCM variant)
    if (*(unsigned short *)&header_buffer[0x14] == 0x0011)
    {
        sound_stream_ctx->is_adpcm = 1;
        sound_stream_ctx->adpcm_bits = (int)(*(unsigned short *)&header_buffer[0x22]);
        sound_stream_ctx->adpcm_predictor = 0;
        sound_stream_ctx->adpcm_step_index = 0;
        audio_header_offset = 0x3C;

        for (scan_pos = 12; scan_pos < 500; scan_pos++)
        {
            if (header_buffer[scan_pos] == 'd' &&
                header_buffer[scan_pos + 1] == 'a' &&
                header_buffer[scan_pos + 2] == 't' &&
                header_buffer[scan_pos + 3] == 'a')
            {
                audio_header_offset = (long)(scan_pos + 8);
                break;
            }
        }
    }
    else
    {
        sound_stream_ctx->is_adpcm = 0;
        audio_header_offset = 0x2C;
    }
    // === END OF DETECTION PIPELINE ===

    // BRANCH 1: ORIGINAL CODE EXECUTION FOR WAV IMA ADPCM TRACKS ONLY
    if (sound_stream_ctx->is_adpcm == 1)
    {
        // Extract global baseline predictor from file metadata header once on start
        sound_stream_ctx->adpcm_predictor = (int)(file_header_scratch[0x2A] | (file_header_scratch[0x28] << 8));
        if (sound_stream_ctx->adpcm_predictor & 0x8000)
        {
            sound_stream_ctx->adpcm_predictor |= 0xFFFF;
        }

        sound_stream_ctx->adpcm_step_index = (int)file_header_scratch[0x2C];
        if (sound_stream_ctx->adpcm_step_index > 88)
        {
            sound_stream_ctx->adpcm_step_index = 88;
        }

        klseek_stream(sound_stream_ctx->file_handle, audio_header_offset, SEEK_SET);
        sound_stream_ctx->remaining_bytes = file_len - audio_header_offset;
        sound_stream_ctx->chunk_size = 16384L;
        sound_stream_ctx->current_chunk = 0;

        sound_stream_ctx->total_chunks = (sound_stream_ctx->remaining_bytes + sound_stream_ctx->chunk_size - 1) / sound_stream_ctx->chunk_size;
        if (sound_stream_ctx->total_chunks > 128)
        {
            sound_stream_ctx->total_chunks = 128;
        }

        current_offset = audio_header_offset;
        for (i = 0; i < sound_stream_ctx->total_chunks; i++)
        {
            sound_stream_ctx->chunk_offsets[i] = current_offset;
            current_offset += sound_stream_ctx->chunk_size;
        }

        // Replace 0 to 128, to remove the starting clicks and prevent from DC-offset accumulation
        memset(sound_stream_ctx->audio_windows[0], 128, 16384);
        memset(sound_stream_ctx->audio_windows[1], 128, 16384);

        // CRITICAL ADAPTIVE INITIAL PRE-FILL (OLD ORIGINAL PATHWAY)
        // Pre-fill ONLY the first buffer before starting playback
        bytes_read_1 = MV_DecompressPage(sound_stream_ctx, sound_stream_ctx->audio_windows[0], 16384);
        // Second buffer will fill during playback frame processing inside service loop
        sound_stream_ctx->current_chunk_size = bytes_read_1;

        sound_stream_ctx->active_page = 0;
        sound_stream_ctx->playing = 1;
        audio_stream_active = 1;

        pitch = soundps[num];
        sound_stream_ctx->voice_handle = MV_PlayRaw
        (
            (char *)sound_stream_ctx->audio_windows[0],
            sound_stream_ctx->current_chunk_size,
            MixRate, 
            pitch, 
            LOUDESTVOLUME, LOUDESTVOLUME, LOUDESTVOLUME,
            soundpr[num], 
            num
        );
    }
    // BRANCH 2: NEW STABLE TRUE PRE-BUFFERING SCHEME FOR UNCOMPRESSED PCM / VOC
    else
    {
        klseek_stream(sound_stream_ctx->file_handle, audio_header_offset, SEEK_SET);
        sound_stream_ctx->remaining_bytes = file_len - audio_header_offset;
        sound_stream_ctx->chunk_size = 16384L;
        sound_stream_ctx->current_chunk = 0;

        sound_stream_ctx->total_chunks = (sound_stream_ctx->remaining_bytes + sound_stream_ctx->chunk_size - 1) / sound_stream_ctx->chunk_size;
        if (sound_stream_ctx->total_chunks > 128)
        {
            sound_stream_ctx->total_chunks = 128;
        }

        current_offset = audio_header_offset;
        for (i = 0; i < sound_stream_ctx->total_chunks; i++)
        {
            sound_stream_ctx->chunk_offsets[i] = current_offset;
            current_offset += sound_stream_ctx->chunk_size;
        }

        memset(sound_stream_ctx->audio_windows[0], 128, 16384);
        memset(sound_stream_ctx->audio_windows[1], 128, 16384);

        // Pre-buffering start setup targeting strictly active index 0
        sound_stream_ctx->active_page = 0;

        // Load the FIRST page completely with native uncompressed wave forms
        bytes_to_read = min(16384L, sound_stream_ctx->remaining_bytes);
        bytes_read = kread_stream(sound_stream_ctx->file_handle, sound_stream_ctx->audio_windows[0], bytes_to_read);

        if (bytes_read > 0)
        {
            sound_stream_ctx->remaining_bytes -= bytes_read;
            sound_stream_ctx->current_chunk_size = bytes_read;
        }

        pitch = soundps[num];

        // Hardware channel instantly fires using completely filled page 0 array payload
        sound_stream_ctx->voice_handle = MV_PlayRaw
        (
            (char *)sound_stream_ctx->audio_windows[0],
            sound_stream_ctx->current_chunk_size,
            MixRate, 
            pitch, 
            LOUDESTVOLUME, LOUDESTVOLUME, LOUDESTVOLUME,
            soundpr[num], 
            num
        );

        // Look-ahead background load step: Pre-fill page 1 right now to hide disk-read latencies
        next_page = 1;
        bytes_to_read = min(16384L, sound_stream_ctx->remaining_bytes);
        bytes_read = kread_stream(sound_stream_ctx->file_handle, sound_stream_ctx->audio_windows[next_page], bytes_to_read);

        if (bytes_read > 0)
        {
            sound_stream_ctx->remaining_bytes -= bytes_read;
            sound_stream_ctx->next_chunk_size = bytes_read; // Cache size data ready for instant sync swaps
        }
        else
        {
            sound_stream_ctx->next_chunk_size = 0;
        }

        sound_stream_ctx->playing = 1;
        audio_stream_active = 1;
    }
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
