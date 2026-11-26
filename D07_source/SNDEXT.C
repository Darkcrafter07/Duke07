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
// SNDEXT.C - Double-Buffered GRP audio streaming & ADPCM subsystem.
// Aka "sounds.c extended". Plays long snd files with low RAM consumption,
// and decodes compressed DVI/IMA ADPCM WAV SFX into Build cache1d memory.
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
//
//**************************************************************************
// DVI / IMA ADPCM (FORMAT 0x0011) DECODER & CONTAINER RULES [DO's & DON'Ts]
//**************************************************************************
// 1. RIFF CHUNK PARSER IS MANDATORY (NO HARDCODED 0x2C OFFSETS):
//    - WAV files saved by Adobe Audition / Cool Edit contain variable-size
//      extended 'fmt ' chunks plus 'fact', 'LIST', 'DISP' metadata tags.
//    - Always scan chunks dynamically to extract real nBlockAlign,
//      wBitsPerSample, nSamplesPerSec, and exact 'data' payload offset/size.
//    - In Full-RAM loader, read sequentially forward after the 24-byte
//      header probe; do NOT use klseek(..., SEEK_SET) backward jumps on GRP
//      handles.
//
// 2. BLOCK HEADER, BIT-STREAM ACCUMULATOR & STEP TABLES (2, 3, 4, 5 BITS):
//    - Every IMA ADPCM block starts with a 4-byte header:
//      [0..1] signed int16 initial predictor (MUST also be emitted as the 1st
//             PCM sample of the block, or 1 sample per block is lost!),
//      [2]    uint8 initial step_index (0..88), [3] reserved (0).
//    - Never feed those 4 header bytes into the sample decoder!
//    - Samples are packed LSB-to-MSB across continuous bytes via a register
//      bit-stream accumulator (bit_buf / bit_cnt), cleanly handling
//      byte-boundary crossings for 3-bit and 5-bit modes.
//    - Cool Edit / Audition step-index adjustment tables per bit-depth:
//      * 2-bit: { -1, 2 }
//      * 3-bit: { -1, -1, 1, 2 }
//      * 4-bit: { -1, -1, -1, -1, 2, 4, 6, 8 }
//      * 5-bit: { -1, -1, -1, -1, -1, -1, -1, -1, 1, 2, 4, 6, 8, 10, 12, 14 }
//
// 3. ZERO-DROP PAGE BOUNDARIES & 1-TAP PASS-THROUGH FOR i486SX-25:
//    - In double-buffered streaming, never discard unread tail bytes or
//      remaining sub-byte bits when the 16KB PCM window fills mid-block;
//      preserve adpcm_bit_buf / adpcm_bit_cnt across page swaps to prevent
//      audio skipping ahead of video/animations.
//    - Use 1-tap direct pass-through (unfiltered) with nearest-sample rate
//      accumulation to MixRate. Avoids double-averaging muddiness with
//      Multivoc/Sound Blaster hardware filters, preserves high-frequency
//      clarity, and consumes near-zero CPU cycles on a 486SX-25.
//
// 4. TWO-PATHWAY SFX LOADING & EXTERNAL FILE PRIORITY:
//    - Small SFX (< 12288 bytes) are pre-cached at level load inside
//      premap.c -> getsound(), bypassing sounds.c -> loadsound().
//    - Larger SFX (>= 12288 bytes) are loaded on demand inside loadsound().
//    - Pass 0 instead of loadfromgrouponly to kopen4load() so loose WAV/VOC
//      files in the game directory override GRP archive entries cleanly.
//    - Both pathways intercept format 0x0011 and call
//      FX_FullRAMLoad_ADPCM_DVI_WAV(), reconstructing a clean 44-byte RIFF
//      PCM8 Mono WAV header in cache1d for native 3D panning and CON pitch.
//**************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <direct.h>
#include <dos.h>
#include <io.h>

#include "types.h"
#include "develop.h"
#include "util_lib.h"
#include "duke3d.h"
#include "sndcards.h"
#include "fx_man.h"
#include "sndext.h"

#define LOUDESTVOLUME 150
#define WAV_HDR_SZ 8
#define WAV_FMT_SZ 32
#define ADPCM_ERR_LEN 128
#define ADPCM_PACKED_BUFSZ 2048

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

char s_adpcm_last_err[ADPCM_ERR_LEN] = "NONE"; // track last ADPCM error



//===============================================================================
//                           IMA ADPCM STUFF - START
//===============================================================================

// Low-level high-speed Intel/DVI IMA ADPCM Lookup Tables
static const int stepSizADPCMTabDVI[89] =
{
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
    50, 55, 60, 66, 73, 80, 88, 97, 107, 118, 130, 143, 157, 173, 190, 209, 230,
    253, 279, 307, 337, 371, 408, 449, 494, 544, 599, 658, 724, 796, 876, 963,
    1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024, 3327,
    3660, 4026, 4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487,
    12635, 13899, 15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};

static const int idxAdjADPCMTabDVI2b[] = { -1, 2 };
static const int idxAdjADPCMTabDVI3b[] = { -1, -1, 1, 2 };
static const int idxAdjADPCMTabDVI4b[] = { -1, -1, -1, -1, 2, 4, 6, 8 };
static const int idxAdjADPCMTabDVI5b[] = { -1, -1, -1, -1, -1, -1, -1, -1,
                                            1,  2,  4,  6,  8, 10, 12, 14 };

//===============================================================================
// Decode_ADPCM_DVI_smp() - Decode one 2, 3, 4 or 5-bit IMA ADPCM sample
//===============================================================================
static short Decode_ADPCM_DVI_smp(int code, int bits, int *predictor, int *step_index)
{
    int i, step, vpdiff, sign_mask, mag_mask, sign, magnitude;

    sign_mask = 1 << (bits - 1);
    mag_mask  = sign_mask - 1;

    step      = stepSizADPCMTabDVI[*step_index];
    sign      = code & sign_mask;
    magnitude = code & mag_mask;

    vpdiff = 0;
    for (i = bits - 2; i >= 0; i--)
    {
        if (magnitude & (1 << i))
        {
            vpdiff += (step >> ((bits - 2) - i));
        }
    }
    vpdiff += (step >> (bits - 1));

    if (sign)
    {
        *predictor -= vpdiff;
        if (*predictor < -32768) *predictor = -32768;
    }
    else
    {
        *predictor += vpdiff;
        if (*predictor > 32767) *predictor = 32767;
    }

    if (bits == 2)        *step_index += idxAdjADPCMTabDVI2b[magnitude & 1];
    else if (bits == 3)   *step_index += idxAdjADPCMTabDVI3b[magnitude & 3];
    else if (bits == 4)   *step_index += idxAdjADPCMTabDVI4b[magnitude & 7];
    else                  *step_index += idxAdjADPCMTabDVI5b[magnitude & 15];

    if (*step_index < 0)  *step_index = 0;
    if (*step_index > 88) *step_index = 88;

    return (short)(*predictor);
}

// MV_ReadADPCMBlockHeader() - Read predictor/step from start of IMA block
static void MV_ReadADPCMBlockHeader(sound_stream_t *ctx)
{
    unsigned char hdr[4];

    // Each IMA block starts with 4 header bytes:
    //   int16 predictor (LE), uint8 step index, uint8 reserved
    if (kread_stream(ctx->file_handle, hdr, 4) == 4)
    {
        ctx->adpcm_predictor = (int)(short)(hdr[0] | (hdr[1] << 8));
        ctx->adpcm_step_index = (int)hdr[2];
        if (ctx->adpcm_step_index > 88)
        {
            ctx->adpcm_step_index = 88;
        }
    }
}

static void MV_EmitResampledADPCMSample(sound_stream_t *ctx, int raw_sample,
                                        unsigned char *output_buffer8,
                                        int *out_samples, int bytes_to_write)
{
    unsigned char pcm8_val;

    // 1-tap direct pass-through (unfiltered, preserves full high-frequency brightness)
    if (raw_sample > 32512)  raw_sample = 32512;
    if (raw_sample < -32768) raw_sample = -32768;
    pcm8_val = (unsigned char)(((raw_sample + 128) >> 8) + 128);

    ctx->adpcm_resample_acc += MixRate;
    while (ctx->adpcm_resample_acc >= (int)ctx->sample_rate && (*out_samples) < bytes_to_write)
    {
        output_buffer8[(*out_samples)++] = pcm8_val;
        ctx->adpcm_resample_acc -= (int)ctx->sample_rate;
    }
}

// Decode up to bytes_to_write PCM8 samples from the current block.
// Supports 2, 3, 4, and 5 bits/sample DVI/IMA ADPCM via a bit-stream accumulator.
// Returns number of PCM8 samples written. Sets *compressed_read to the
// number of compressed bytes consumed from disk.
static int MV_Decomp_ADPCM_DVI_block(sound_stream_t *ctx, unsigned char *output_buffer8,
                                     int bytes_to_write, long *compressed_read)
{
    int out_samples, block_payload, code;
    int bits, mask, fetched, byte_idx;
    int bytes_to_fetch, rem_out;
    short sample_calced;
    unsigned char packed_buffer[ADPCM_PACKED_BUFSZ];

    out_samples = 0;
    *compressed_read = 0;

    if (ctx->sample_rate <= 0) ctx->sample_rate = 11025;
    bits = ctx->adpcm_bits;
    if (bits < 2 || bits > 5) bits = 4;
    mask = (1 << bits) - 1;

    block_payload = ctx->adpcm_block_align - 4;
    if (block_payload <= 0 || block_payload > ADPCM_PACKED_BUFSZ)
    {
        return 0;
    }

    // 1. Flush any leftover bits from the previous page's accumulator first
    while (ctx->adpcm_bit_cnt >= bits && out_samples < bytes_to_write)
    {
        code = (int)(ctx->adpcm_bit_buf & mask);
        ctx->adpcm_bit_buf >>= bits;
        ctx->adpcm_bit_cnt -= bits;

        sample_calced = Decode_ADPCM_DVI_smp(code, bits, &ctx->adpcm_predictor, &ctx->adpcm_step_index);
        MV_EmitResampledADPCMSample(ctx, (int)sample_calced, output_buffer8, &out_samples, bytes_to_write);
    }

    if (out_samples >= bytes_to_write)
    {
        return out_samples;
    }

    // 2. Start of a new block: read its 4-byte header and emit its initial predictor sample
    if (ctx->adpcm_block_pos >= ctx->adpcm_block_payload_size && ctx->adpcm_bit_cnt < bits)
    {
        if (ctx->remaining_bytes < ctx->adpcm_block_align)
        {
            return out_samples;
        }
        MV_ReadADPCMBlockHeader(ctx);
        *compressed_read += 4;
        ctx->remaining_bytes -= 4;
        ctx->adpcm_block_pos = 0;
        ctx->adpcm_block_payload_size = block_payload;

        // Reset bit-stream accumulator at the start of each byte-aligned block payload
        ctx->adpcm_bit_buf = 0;
        ctx->adpcm_bit_cnt = 0;

        // Emit the 1st sample stored directly in the block header so no sample is dropped per block
        MV_EmitResampledADPCMSample(ctx, ctx->adpcm_predictor, output_buffer8, &out_samples, bytes_to_write);
        if (out_samples >= bytes_to_write)
        {
            return out_samples;
        }
    }

    // 3. Decode bytes from current block without ever discarding unread tail bytes or sub-byte bits
    while (out_samples < bytes_to_write &&
           ctx->adpcm_block_pos < ctx->adpcm_block_payload_size)
    {
        rem_out = bytes_to_write - out_samples;

        // Estimate safe number of compressed bytes that fit completely into rem_out (8 bits per byte)
        bytes_to_fetch = (rem_out * (int)ctx->sample_rate * bits) / (MixRate * 8);
        if (bytes_to_fetch < 1)
        {
            bytes_to_fetch = 1;
        }
        if (bytes_to_fetch > (ctx->adpcm_block_payload_size - ctx->adpcm_block_pos))
        {
            bytes_to_fetch = ctx->adpcm_block_payload_size - ctx->adpcm_block_pos;
        }
        if (bytes_to_fetch > ADPCM_PACKED_BUFSZ)
        {
            bytes_to_fetch = ADPCM_PACKED_BUFSZ;
        }
        if (bytes_to_fetch > ctx->remaining_bytes)
        {
            bytes_to_fetch = (int)ctx->remaining_bytes;
        }
        if (bytes_to_fetch <= 0)
        {
            break;
        }

        fetched = kread_stream(ctx->file_handle, packed_buffer, bytes_to_fetch);
        if (fetched <= 0)
        {
            break;
        }

        *compressed_read += fetched;
        ctx->remaining_bytes -= fetched;
        ctx->adpcm_block_pos += fetched;

        for (byte_idx = 0; byte_idx < fetched; byte_idx++)
        {
            // Push next 8 compressed bits into the LSB-first accumulator
            ctx->adpcm_bit_buf |= ((unsigned int)packed_buffer[byte_idx]) << ctx->adpcm_bit_cnt;
            ctx->adpcm_bit_cnt += 8;

            while (ctx->adpcm_bit_cnt >= bits && out_samples < bytes_to_write)
            {
                code = (int)(ctx->adpcm_bit_buf & mask);
                ctx->adpcm_bit_buf >>= bits;
                ctx->adpcm_bit_cnt -= bits;

                sample_calced = Decode_ADPCM_DVI_smp
                (
                    code,
                    bits,
                    &ctx->adpcm_predictor,
                    &ctx->adpcm_step_index
                );
                MV_EmitResampledADPCMSample(ctx, (int)sample_calced, output_buffer8, &out_samples, bytes_to_write);
            }

            // If output buffer filled up mid-byte, stop immediately; remaining bits stay in ctx
            if (out_samples >= bytes_to_write)
            {
                break;
            }
        }
    }

    return out_samples;
}

// MV_Parse_ADPCM_DVI_WAV() - Read block_align and locate 'data' chunk for IMA WAV, call in soundstream
static int MV_Parse_ADPCM_DVI_WAV(sound_stream_t *ctx, long *data_offset, long *data_size)
{
    int  fmt_found;
    long pos, chunk_size;
    unsigned char chunk_hdr[WAV_HDR_SZ];
    unsigned char fmt_buf[WAV_FMT_SZ];

    fmt_found = 0;
    pos = 12;

    klseek_stream(ctx->file_handle, pos, SEEK_SET);

    while (kread_stream(ctx->file_handle, chunk_hdr, 8) == 8)
    {
        chunk_size = (long)(*(chunk_hdr + 4) | (*(chunk_hdr + 5) << 8) |
                            (*(chunk_hdr + 6) << 16) | ((long)*(chunk_hdr + 7) << 24));

        if (*(chunk_hdr + 0) == 'f' && *(chunk_hdr + 1) == 'm' &&
            *(chunk_hdr + 2) == 't' && *(chunk_hdr + 3) == ' ')
        {
            if (kread_stream(ctx->file_handle, fmt_buf, 16) != 16)
            {
                return 0;
            }
            // fmt_buf[4..7] = sample_rate, [12..13] = block_align, [14..15] = bits per sample
            ctx->sample_rate = (unsigned long)(*(fmt_buf + 4) | (*(fmt_buf + 5) << 8) |
                               ((unsigned long)*(fmt_buf + 6) << 16) |
                               ((unsigned long)*(fmt_buf + 7) << 24));
            ctx->adpcm_block_align = (int)(*(fmt_buf + 12) | (*(fmt_buf + 13) << 8));
            ctx->adpcm_bits        = (int)(*(fmt_buf + 14) | (*(fmt_buf + 15) << 8));
            if (ctx->adpcm_bits < 2 || ctx->adpcm_bits > 5)
            {
                ctx->adpcm_bits = 4;
            }
            fmt_found = 1;

            pos += 8 + chunk_size + (chunk_size & 1);
            klseek_stream(ctx->file_handle, pos, SEEK_SET);
            continue;
        }

        if (*(chunk_hdr + 0) == 'd' && *(chunk_hdr + 1) == 'a' &&
            *(chunk_hdr + 2) == 't' && *(chunk_hdr + 3) == 'a')
        {
            if (!fmt_found)
            {
                return 0;
            }
            *data_offset = pos + 8;
            *data_size   = chunk_size;
            return 1;
        }

        pos += 8 + chunk_size + (chunk_size & 1);
        klseek_stream(ctx->file_handle, pos, SEEK_SET);
    }

    return 0;
}

// soundstream_adpcm_init() - Initialize IMA ADPCM streaming (called from soundstream)
static void soundstream_adpcm_init(short num, long file_len)
{
    int i, next_page, page_size;
    long data_offset, data_size;
    long bytes_decoded, compressed_read;
    unsigned char hdr_scratch[3];

    data_offset = 0;
    data_size = 0;
    compressed_read = 0;
    bytes_decoded = 0;
    next_page = 1;
    page_size = 16384;

    if (!MV_Parse_ADPCM_DVI_WAV(sound_stream_ctx, &data_offset, &data_size))
    {
        FX_StopVocStream();
        return;
    }

    audio_header_offset = data_offset;
    sound_stream_ctx->data_start_offset = data_offset;
    sound_stream_ctx->remaining_bytes = data_size;

    if (sound_stream_ctx->adpcm_block_align <= 4)
    {
        FX_StopVocStream();
        return;
    }

    sound_stream_ctx->adpcm_resample_acc = 0;
    sound_stream_ctx->adpcm_block_pos = 0;
    sound_stream_ctx->adpcm_block_payload_size = 0;

    klseek_stream(sound_stream_ctx->file_handle, data_offset, SEEK_SET);

    memset(sound_stream_ctx->audio_windows[0], 128, page_size);
    memset(sound_stream_ctx->audio_windows[1], 128, page_size);

    // Pre-decode page 0 (plays first)
    bytes_decoded = MV_DecompressPage
    (
        sound_stream_ctx,
        sound_stream_ctx->audio_windows[0],
        page_size,
        &compressed_read
    );
    sound_stream_ctx->current_chunk_size = (int)bytes_decoded;

    // Pre-decode page 1 (lookahead, will play next)
    bytes_decoded = MV_DecompressPage
    (
        sound_stream_ctx,
        sound_stream_ctx->audio_windows[next_page],
        page_size,
        &compressed_read
    );
    sound_stream_ctx->next_chunk_size = (int)bytes_decoded;

    sound_stream_ctx->active_page = 0;
    sound_stream_ctx->playing = 1;
    audio_stream_active = 1;

    sound_stream_ctx->voice_handle = MV_PlayRaw
    (
        (char *)sound_stream_ctx->audio_windows[0],
        sound_stream_ctx->current_chunk_size,
        MixRate,
        soundps[num],
        LOUDESTVOLUME, LOUDESTVOLUME, LOUDESTVOLUME,
        soundpr[num],
        num
    );

    (void)i;
    (void)hdr_scratch;
}

static void FX_ServiceVocStream_ADPCM(int handle)
{
    int play_page, decode_page, decoded;
    long compressed_read;

    compressed_read = 0;
    decoded = 0;

    if (MV_VoicePlaying(sound_stream_ctx->voice_handle) != 0)
    {
        return;
    }

    // Page that is going to be played now
    play_page = sound_stream_ctx->active_page ^ 1;
    // Page that is NOT playing and may be overwritten
    decode_page = play_page ^ 1;

    if (sound_stream_ctx->next_chunk_size <= 0)
    {
        // Nothing decoded ahead: loop or stop
        if (soundm[handle] & 1)
        {
            sound_stream_ctx->adpcm_bit_buf = 0;
            sound_stream_ctx->adpcm_bit_cnt = 0;
            sound_stream_ctx->adpcm_resample_acc = 0;
            klseek_stream(sound_stream_ctx->file_handle,
                          sound_stream_ctx->data_start_offset, SEEK_SET);
            sound_stream_ctx->remaining_bytes = soundsiz[handle] -
                                                sound_stream_ctx->data_start_offset;
            sound_stream_ctx->adpcm_block_pos = 0;
            sound_stream_ctx->adpcm_block_payload_size = 0;
            sound_stream_ctx->next_chunk_size = 0;
        }
        else
        {
            FX_StopVocStream();
            return;
        }
    }

    // Start the already-decoded page
    sound_stream_ctx->current_chunk_size = sound_stream_ctx->next_chunk_size;
    sound_stream_ctx->voice_handle = MV_PlayRaw
    (
        (char *)sound_stream_ctx->audio_windows[play_page],
        sound_stream_ctx->current_chunk_size,
        MixRate,
        0,
        LOUDESTVOLUME, LOUDESTVOLUME, LOUDESTVOLUME,
        soundpr[handle],
        handle
    );
    sound_stream_ctx->active_page = play_page;

    // Decode the next page into the NON-playing buffer
    memset(sound_stream_ctx->audio_windows[decode_page], 128, 16384);
    decoded = MV_DecompressPage
    (
        sound_stream_ctx,
        sound_stream_ctx->audio_windows[decode_page],
        16384,
        &compressed_read
    );
    sound_stream_ctx->next_chunk_size = decoded;
}

//===============================================================================
//                           IMA ADPCM STUFF - FINISH
//===============================================================================



//===============================================================================
// Container and format detector - execute it strictly ONCE on track start
//===============================================================================
// Encapsulated safe container analyzer - executes strictly ONCE on track start
static void MV_AnalyzeAndSetupStream(sound_stream_t *ctx, long file_len, long *out_header_offset)
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
            voc_rate_byte = header_buffer[*out_header_offset + 4];
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
// Keeps decoding across block boundaries. Returns PCM8 samples written.
// *compressed_read receives the total compressed bytes consumed.
//====================================================================================
static int MV_DecompressPage(sound_stream_t *ctx, unsigned char *output_buffer8, int bytes_to_read, long *compressed_read)
{
    int total_out, got;
    long consumed;

    total_out = 0;
    *compressed_read = 0;

    while (total_out < bytes_to_read && ctx->remaining_bytes > 0)
    {
        got = MV_Decomp_ADPCM_DVI_block
        (
            ctx,
            output_buffer8 + total_out,
            bytes_to_read - total_out,
            &consumed
        );

        *compressed_read += consumed;

        if (got <= 0)
        {
            break;
        }
        total_out += got;
    }

    // Pad remainder with PCM8 silence
    while (total_out < bytes_to_read)
    {
        output_buffer8[total_out++] = 128;
    }

    return total_out;
}

//===============================================================================
// FX_ServiceVocStream() - Background streaming service for double-buffered audio
//===============================================================================
void FX_ServiceVocStream(void)
{
    // COMPACT C89/C99 VARIABLES ALLOCATION BLOCK AT THE TOP
    int shadow_page, handle, next_page;
    long bytes_to_read, bytes_read;

    if (!audio_stream_active || sound_stream_ctx == NULL || !sound_stream_ctx->playing)
    {
        return;
    }

    handle = active_stream_sound_id;
    if (handle < 0 || handle >= NUM_SOUNDS)
    {
        return;
    }

    if (sound_stream_ctx->is_adpcm == 0)
    {
        // BRANCH 1: REGULAR UNCOMPRESSED PCM / VOC TRACKS
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
    else
    {
        // BRANCH 2: IMA ADPCM WAV - page-swap service (decodes into the NON-playing page)
        FX_ServiceVocStream_ADPCM(handle);
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
    if (audio_stream_active && sound_stream_ctx != NULL && 
        sound_stream_ctx->playing && active_stream_sound_id == num)
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

    if (sound_stream_ctx->is_adpcm == 1)
    {
        // ---=== BRANCH 1: IMA ADPCM WAV - full init (parses fmt/data chunks, pre-decodes 2 pages)
        soundstream_adpcm_init(num, file_len);
    }

    else
    {
        // ---=== BRANCH 2: TRUE PRE-BUFFERING SCHEME FOR UNCOMPRESSED PCM / VOC
        klseek_stream(sound_stream_ctx->file_handle, audio_header_offset, SEEK_SET);
        sound_stream_ctx->remaining_bytes = file_len - audio_header_offset;
        sound_stream_ctx->chunk_size = 16384L;
        sound_stream_ctx->current_chunk = 0;

        sound_stream_ctx->total_chunks = (sound_stream_ctx->remaining_bytes + 
                                          sound_stream_ctx->chunk_size - 1) / 
                                                sound_stream_ctx->chunk_size;

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

        bytes_read = 
        kread_stream(sound_stream_ctx->file_handle, sound_stream_ctx->audio_windows[0], bytes_to_read);


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

        bytes_read = 
        kread_stream(sound_stream_ctx->file_handle, sound_stream_ctx->audio_windows[next_page], bytes_to_read);


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

//-----------------------------------------------------------------------------
//************** NON-STREAMING FULL RAM LOAD FUNCTIONS - START ****************
//=============================================================================
// FX_FullRAMLoad_ADPCM_DVI_WAV - standalone DVI/IMA ADPCM WAV -> PCM8 WAV.
// Uses standard Build fp (kopen4load/kread) and engine cache (allocache).
// Does NOT stream! Requires loading an entire file in RAM like regular
// Duke3D functions! Made to add ADPCM IMA/DVI support to all sound files.
//=============================================================================
//-----------------------------------------------------------------------------
#define WAV_PROBE_LEN 64
char FX_FullRAMLoad_ADPCM_DVI_WAV(long fp, long file_len, char **out_ptr, unsigned char *lock_ptr, long *out_size)
{
    int fmt_found, block_align, bits, mask, bit_cnt;
    int predictor, step_index, code;
    unsigned int bit_buf;
    long b, pos, data_offset, data_size;
    long sample_rate, total_blocks, samples_per_block;
    long total_samples, alloc_size, out_pos, payload_bytes, byte_idx;
    short smp;
    char blk_lock;
    unsigned char *wav_buf;
    unsigned char *pcm_dst;
    unsigned char *blk_buf;
    unsigned char probe[WAV_PROBE_LEN];

    if (file_len <= 44)
    {
        //sprintf(s_adpcm_last_err, "FAIL_LEN: %ld", file_len);
        return 0;
    }

    // First 24 bytes are already read in "loadsound" / "getsound" (RIFF[12] + "fmt "[4] + fmt_size[4] + fmt_tag[2] + channels[2]).
    // Read the remaining 12 bytes of the standard fmt-chunk sequentially without "klseek".
    if (kread(fp, probe, 12) != 12)
    {
        //sprintf(s_adpcm_last_err, "FAIL0: kread fmt tail");
        return 0;
    }

    sample_rate = (long)(*(probe + 0) | (*(probe + 1) << 8) | (*(probe + 2) << 16) | ((long)*(probe + 3) << 24));
    block_align = (int)(*(probe + 8) | (*(probe + 9) << 8));
    bits        = (int)(*(probe + 10) | (*(probe + 11) << 8));
    fmt_found   = 1;

    // We are now at byte offset 36 from the start of the file.
    // Locate the 'data' chunk using a 2-byte sliding window read.
    pos = 36;
    data_offset = 0;
    data_size = 0;

    while ((pos + 8) <= file_len)
    {
        if (kread(fp, probe, 2) != 2)
        {
            break;
        }
        pos += 2;

        if (*(probe + 0) == 'd' && *(probe + 1) == 'a')
        {
            if (kread(fp, probe + 2, 6) == 6)
            {
                pos += 6;
                if (*(probe + 2) == 't' && *(probe + 3) == 'a')
                {
                    data_size = (long)(*(probe + 4) | (*(probe + 5) << 8) |
                                      (*(probe + 6) << 16) | ((long)*(probe + 7) << 24));
                    data_offset = pos;
                    break;
                }
            }
        }
        if (pos > 512)
        {
            break;
        }
    }

    if (!fmt_found || data_offset <= 0 || data_size <= 4 || block_align <= 4 || block_align > 8192)
    {
        //sprintf(s_adpcm_last_err, "FAIL1: doff=%ld dsz=%ld ba=%d bits=%d", data_offset, data_size, block_align, bits);
        return 0;
    }

    // Validate bit-depth (supports 2, 3, 4, and 5 bits/sample DVI/IMA ADPCM)
    if (bits < 2 || bits > 5) bits = 4;
    if (sample_rate <= 0) sample_rate = 11025;
    mask = (1 << bits) - 1;

    // Compute exact sample count per block across all supported bit-depths
    samples_per_block = 1L + (((long)(block_align - 4) * 8L) / (long)bits);
    total_blocks      = data_size / (long)block_align;

    if (total_blocks <= 0)
    {
        //sprintf(s_adpcm_last_err, "FAIL1B: blks=%ld", total_blocks);
        return 0;
    }

    total_samples = total_blocks * samples_per_block;
    alloc_size    = 44L + total_samples;

    // Allocate single-block temporary scratch buffer in Build cache1d
    blk_buf = NULL;
    blk_lock = 200;
    allocache((long *)&blk_buf, (long)block_align, &blk_lock);

    if (blk_buf == NULL)
    {
        //sprintf(s_adpcm_last_err, "FAIL2A: blk_buf NULL");
        return 0;
    }

    // Allocate destination PCM8 WAV buffer in Build cache1d
    *lock_ptr = 200;
    allocache((long *)out_ptr, alloc_size, lock_ptr);

    if (*out_ptr == NULL)
    {
        blk_lock = 1;
        //sprintf(s_adpcm_last_err, "FAIL2B: out_ptr NULL sz=%ld", alloc_size);
        return 0;
    }

    wav_buf = (unsigned char *)(*out_ptr);
    pcm_dst = wav_buf + 44;

    out_pos = 0;

    // Decode all IMA ADPCM blocks sequentially using a register-friendly bit accumulator
    for (b = 0; b < total_blocks; b++)
    {
        if (kread(fp, blk_buf, (long)block_align) != (long)block_align)
        {
            break;
        }

        // Parse 4-byte block header: int16 initial predictor, uint8 initial step_index, uint8 reserved
        predictor  = (int)(short)(*(blk_buf + 0) | (*(blk_buf + 1) << 8));
        step_index = (int)(*(blk_buf + 2));
        if (step_index < 0)  step_index = 0;
        if (step_index > 88) step_index = 88;

        // Emit 1st sample directly from the block header (1-tap pass-through, unfiltered)
        if (predictor > 32512)  predictor = 32512;
        if (predictor < -32768) predictor = -32768;
        *(pcm_dst + out_pos) = (unsigned char)(((predictor + 128) >> 8) + 128);
        out_pos++;

        // Reset bit-stream accumulator at the start of each byte-aligned block payload
        bit_buf = 0;
        bit_cnt = 0;

        payload_bytes = (long)block_align - 4L;
        for (byte_idx = 0; byte_idx < payload_bytes; byte_idx++)
        {
            // Shift next 8 compressed bits into the LSB-first accumulator
            bit_buf |= ((unsigned int)(*(blk_buf + 4 + byte_idx))) << bit_cnt;
            bit_cnt += 8;

            while (bit_cnt >= bits)
            {
                code = (int)(bit_buf & mask);
                bit_buf >>= bits;
                bit_cnt -= bits;

                // Decode sample and quantize 16-bit signed PCM to 8-bit unsigned PCM (unfiltered)
                smp = Decode_ADPCM_DVI_smp(code, bits, &predictor, &step_index);
                if (smp > 32512)  smp = 32512;
                if (smp < -32768) smp = -32768;
                *(pcm_dst + out_pos) = (unsigned char)((((int)smp + 128) >> 8) + 128);
                out_pos++;
            }
        }
    }

    // Unlock temporary single-block scratch buffer
    blk_lock = 1;

    if (out_pos <= 0)
    {
        *lock_ptr = 1;
        //sprintf(s_adpcm_last_err, "FAIL3: out_pos=0");
        return 0;
    }

    // Reconstruct standard 44-byte RIFF PCM 8-bit Mono WAV header at native sample_rate
    memset(wav_buf, 0, 44);
    memcpy(wav_buf + 0,  "RIFF", 4);
    *(unsigned long *)(wav_buf + 4)  = 36UL + (unsigned long)out_pos;
    memcpy(wav_buf + 8,  "WAVE", 4);
    memcpy(wav_buf + 12, "fmt ", 4);
    *(unsigned long *)(wav_buf + 16) = 16UL;
    *(unsigned short *)(wav_buf + 20) = 1;
    *(unsigned short *)(wav_buf + 22) = 1;
    *(unsigned long *)(wav_buf + 24) = (unsigned long)sample_rate;
    *(unsigned long *)(wav_buf + 28) = (unsigned long)sample_rate;
    *(unsigned short *)(wav_buf + 32) = 1;
    *(unsigned short *)(wav_buf + 34) = 8;
    memcpy(wav_buf + 36, "data", 4);
    *(unsigned long *)(wav_buf + 40) = (unsigned long)out_pos;

    *out_size = 44L + out_pos;

    //sprintf(s_adpcm_last_err, "OK: sz=%ld rate=%ld", *out_size, sample_rate);

    return 1;
}

//************** NON-STREAMING FULL RAM LOAD FUNCTIONS - FINISH ***************
//-----------------------------------------------------------------------------



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






//*****************************************************************************
//******** 2-WAY PCM VOC/WAV TO IMA/DVI ADPCM WAV CONVERTER - START ***********
//-----------------------------------------------------------------------------
#define MAX_BATCH_FILES    32768
#define CNV_STREAM_BUFSZ   16384
#define ADPCM_MAX_BLK_SZ   2048
#define ADPCM_WAV_HDR_SZ   48
#define ADPCM_OUTPATH_LEN  64
#define ADPCM_BASENAME_LEN 32
#define PCM_OUTPATH_LEN    64
#define PCM_BASENAME_LEN   32
#define VOC_HDR_SZ         32
#define MAX_SKIP_LOG       128
#define SKIP_LINE_LEN      64

int cnv_adpcm_bits = 0;
int cnv_pcm_mode = -1;
static int  s_skip_log_cnt = 0;
static int  s_seen_count = 0;
static char s_seen_files[MAX_BATCH_FILES][16];
static char s_skip_log[MAX_SKIP_LOG][SKIP_LINE_LEN];
static unsigned char s_cnv_iobuf[CNV_STREAM_BUFSZ];

extern long numgroupfiles;
extern char *gfilelist[];
extern long gnumfiles[];

// Helper: refill 16KB input window from Build fp for streaming VOC->ADPCM conversion
typedef struct
{
    long fp;
    long file_len;
    long file_pos;
    int  buf_pos;
    int  buf_len;
    int  src_bits;
    int  src_channels;
    long sample_rate;
    int  is_voc;
    long data_rem;      // Remaining bytes in current WAV data or VOC block
    long voc_loop_pos;  // File offset right after VOC 0x06 block
    int  voc_loop_cnt;  // Remaining finite repeats for VOC 0x07
    int  voc_silence;   // 1 if currently emitting VOC 0x03 silence bytes
    int  eof;
} cnv_src_stream_t;

static void CNV_LogSkip(const char *fname, const char *reason, long detail1, long detail2)
{
    if (s_skip_log_cnt < MAX_SKIP_LOG)
    {
        sprintf(s_skip_log[s_skip_log_cnt], "  %-14s : %s (%ld, %ld)",
                fname, reason, detail1, detail2);
        s_skip_log_cnt++;
    }
}

static int CNV_ReadByte(cnv_src_stream_t *st, unsigned char *out_b)
{
    long to_read;
    if (st->buf_pos >= st->buf_len)
    {
        if (st->file_pos >= st->file_len) return 0;
        to_read = st->file_len - st->file_pos;
        if (to_read > CNV_STREAM_BUFSZ) to_read = CNV_STREAM_BUFSZ;
        st->buf_len = (int)kread(st->fp, s_cnv_iobuf, to_read);
        st->buf_pos = 0;
        if (st->buf_len <= 0) return 0;
        st->file_pos += st->buf_len;
    }
    *out_b = s_cnv_iobuf[st->buf_pos++];
    return 1;
}

static void CNV_SeekAbs(cnv_src_stream_t *st, long abs_pos)
{
    klseek(st->fp, abs_pos, SEEK_SET);
    st->file_pos = abs_pos;
    st->buf_pos  = 0;
    st->buf_len  = 0;
}

static int CNV_NextSample16(cnv_src_stream_t *st, int *out_smp16)
{
    int i, cnt, voc_fmt;
    long blk_len, skip_pos;
    unsigned char b0, b1, bh[16];

    if (st->eof) return 0;

    while (st->data_rem <= 0)
    {
        if (!st->is_voc)
        {
            st->eof = 1;
            return 0;
        }

        // Advance to next VOC block
        st->voc_silence = 0;
        if (!CNV_ReadByte(st, &b0) || b0 == 0x00)
        {
            st->eof = 1;
            return 0;
        }
        if (!CNV_ReadByte(st, &bh[0]) || !CNV_ReadByte(st, &bh[1]) || !CNV_ReadByte(st, &bh[2]))
        {
            st->eof = 1;
            return 0;
        }
        blk_len = (long)(bh[0] | (bh[1] << 8) | ((long)bh[2] << 16));
        if (blk_len < 0)
        {
            st->eof = 1;
            return 0;
        }

        if (b0 == 0x01 && blk_len > 2)
        {
            CNV_ReadByte(st, &bh[0]); // TC
            CNV_ReadByte(st, &bh[1]); // Pack
            if (bh[1] != 0) { st->eof = 1; return 0; }
            st->sample_rate = 1000000L / (256L - (long)bh[0]);
            st->src_bits = 8;
            st->src_channels = 1;
            st->data_rem = blk_len - 2;
        }
        else if (b0 == 0x02 && blk_len > 0)
        {
            st->data_rem = blk_len;
        }
        else if (b0 == 0x03 && blk_len >= 3)
        {
            CNV_ReadByte(st, &bh[0]);
            CNV_ReadByte(st, &bh[1]);
            CNV_ReadByte(st, &bh[2]);
            st->data_rem = (long)(bh[0] | (bh[1] << 8)) + 1L;
            if (st->sample_rate <= 0) st->sample_rate = 1000000L / (256L - (long)bh[2]);
            st->voc_silence = 1;
        }
        else if (b0 == 0x06 && blk_len >= 2)
        {
            CNV_ReadByte(st, &bh[0]);
            CNV_ReadByte(st, &bh[1]);
            cnt = (int)(bh[0] | (bh[1] << 8));
            st->voc_loop_cnt = (cnt == 0xFFFF || cnt < 0) ? 0 : cnt;
            st->voc_loop_pos = (st->file_pos - (long)(st->buf_len - st->buf_pos));
        }
        else if (b0 == 0x07)
        {
            if (st->voc_loop_cnt > 0 && st->voc_loop_pos > 0)
            {
                st->voc_loop_cnt--;
                CNV_SeekAbs(st, st->voc_loop_pos);
            }
        }
        else if (b0 == 0x09 && blk_len > 12)
        {
            for (i = 0; i < 12; i++) CNV_ReadByte(st, &bh[i]);
            st->sample_rate  = (long)(bh[0] | (bh[1] << 8) | (bh[2] << 16) | ((long)bh[3] << 24));
            st->src_bits     = (int)bh[4];
            st->src_channels = (int)bh[5];
            voc_fmt          = (int)(bh[6] | (bh[7] << 8));
            if ((voc_fmt != 0 && voc_fmt != 4) || st->src_channels != 1 || 
                                (st->src_bits != 8 && st->src_bits != 16))
            {
                st->eof = 1;
                return 0;
            }
            st->data_rem = blk_len - 12;
        }
        else
        {
            // Skip unknown/comment block (0x04, 0x05, 0x08, etc.)
            skip_pos = (st->file_pos - (long)(st->buf_len - st->buf_pos)) + blk_len;
            CNV_SeekAbs(st, skip_pos);
        }
    }

    if (st->voc_silence)
    {
        st->data_rem--;
        *out_smp16 = 0;
        return 1;
    }

    if (st->src_bits == 16)
    {
        if (st->data_rem < 2 || !CNV_ReadByte(st, &b0) || !CNV_ReadByte(st, &b1))
        {
            st->eof = 1;
            return 0;
        }
        st->data_rem -= 2;
        *out_smp16 = (int)(short)(b0 | (b1 << 8));
        return 1;
    }
    else
    {
        if (!CNV_ReadByte(st, &b0))
        {
            st->eof = 1;
            return 0;
        }
        st->data_rem--;
        *out_smp16 = ((int)b0 - 128) << 8;
        return 1;
    }
}

static int FX_BatchAlreadySeen(const char *fname)
{
    int i;
    for (i = 0; i < s_seen_count; i++)
    {
        if (stricmp(s_seen_files[i], fname) == 0)
            return 1;
    }
    if (s_seen_count < MAX_BATCH_FILES)
    {
        strncpy(s_seen_files[s_seen_count], fname, 15);
        s_seen_files[s_seen_count][15] = '\0';
        s_seen_count++;
    }
    return 0;
}

static int FX_HasSoundExt(const char *fname)
{
    const char *dot = strrchr(fname, '.');
    if (!dot) return 0;
    return (stricmp(dot, ".VOC") == 0 || stricmp(dot, ".WAV") == 0);
}

static void FX_RunBatchOverAllSoundFiles(int direction_mode, int param)
{
    int g, f, converted, skipped, res;
    long orig_sz, new_sz, total_orig, total_new;
    char fname[16];
    char out_path[ADPCM_OUTPATH_LEN];
    char base_name[ADPCM_BASENAME_LEN];
    char *dot_ptr;
    char *dir_name = (direction_mode == 0) ? "ADPCMCNV" : "PCMCONV";
    char *ext_str  = (direction_mode == 0) ? "WAV" : (param ? "WAV" : "VOC");
    struct find_t ff;
    unsigned rc;
    FILE *rep_fp;

    s_skip_log_cnt = 0;
    s_seen_count = 0;
    converted = 0;
    skipped = 0;
    total_orig = 0;
    total_new = 0;

    mkdir(dir_name);

    rep_fp = fopen("SNDCNVRP.TXT", "wt");
    if (rep_fp)
    {
        fprintf(rep_fp, "=== DUKE3D 2-WAY AUDIO CONVERTER REPORT ===\n");
        fprintf(rep_fp, "Mode: %s | Param: %d | Output dir: %s\\\n\n",
                (direction_mode == 0) ? "SND2WAVAD" : "ADPCMTOPCM", param, dir_name);
    }

    // 1. Scan loose *.VOC and *.WAV in game root directory first (overrides GRP!)
    for (g = 0; g < 2; g++)
    {
        rc = _dos_findfirst((g == 0) ? "*.VOC" : "*.WAV", _A_NORMAL | _A_RDONLY | _A_ARCH, &ff);
        while (rc == 0)
        {
            if (!FX_BatchAlreadySeen(ff.name))
            {
                strncpy(base_name, ff.name, ADPCM_BASENAME_LEN - 1);
                base_name[ADPCM_BASENAME_LEN - 1] = '\0';
                dot_ptr = strrchr(base_name, '.');
                if (dot_ptr) *dot_ptr = '\0';

                sprintf(out_path, "%s\\%s.%s", dir_name, base_name, ext_str);
                res = (direction_mode == 0)
                    ? FX_ConvertSingleFileToADPCM(ff.name, out_path, param, &orig_sz, &new_sz)
                    : FX_ConvertSingleADPCMToPCM(ff.name, out_path, param, &orig_sz, &new_sz);

                if (res == 1)
                {
                    converted++;
                    total_orig += orig_sz;
                    total_new  += new_sz;
                    printf("  [ROOT] %-14s -> %-22s (%ld -> %ld B)\n",
                           ff.name, out_path, orig_sz, new_sz);
                    if (rep_fp)
                    {
                        fprintf(rep_fp, "  [ROOT] %-14s -> %-22s (%ld -> %ld B)\n",
                                ff.name, out_path, orig_sz, new_sz);
                    }
                }
                else if (res < 0) skipped++;
            }
            rc = _dos_findnext(&ff);
        }
    }

    // 2. Scan ALL files inside all loaded .GRP archives (not restricted to USER.CON!)
    for (g = 0; g < numgroupfiles; g++)
    {
        if (gfilelist[g] == NULL) continue;
        for (f = 0; f < gnumfiles[g]; f++)
        {
            memcpy(fname, gfilelist[g] + (f * 16), 12);
            fname[12] = '\0';

            if (!FX_HasSoundExt(fname)) continue;
            if (FX_BatchAlreadySeen(fname)) continue; // Already converted loose override!

            strncpy(base_name, fname, ADPCM_BASENAME_LEN - 1);
            base_name[ADPCM_BASENAME_LEN - 1] = '\0';
            dot_ptr = strrchr(base_name, '.');
            if (dot_ptr) *dot_ptr = '\0';

            sprintf(out_path, "%s\\%s.%s", dir_name, base_name, ext_str);
            res = (direction_mode == 0)
                ? FX_ConvertSingleFileToADPCM(fname, out_path, param, &orig_sz, &new_sz)
                : FX_ConvertSingleADPCMToPCM(fname, out_path, param, &orig_sz, &new_sz);

            if (res == 1)
            {
                converted++;
                total_orig += orig_sz;
                total_new  += new_sz;
                printf("  [GRP ] %-14s -> %-22s (%ld -> %ld B)\n",
                       fname, out_path, orig_sz, new_sz);
                if (rep_fp)
                {
                    fprintf(rep_fp, "  [GRP ] %-14s -> %-22s (%ld -> %ld B)\n",
                            fname, out_path, orig_sz, new_sz);
                }
            }
            else if (res < 0) skipped++;
        }
    }

    printf("[BATCH CNV] Done! Converted: %d, Skipped: %d | Total: %ld KB -> %ld KB\n",
           converted, skipped, total_orig >> 10, total_new >> 10);

    if (s_skip_log_cnt > 0)
    {
        printf("\n--- Skipped files report (%d logged) ---\n", s_skip_log_cnt);
        for (f = 0; f < s_skip_log_cnt; f++)
        {
            printf("%s\n", s_skip_log[f]);
        }
        if (skipped > s_skip_log_cnt)
        {
            printf("  ... and %d more skipped files.\n", skipped - s_skip_log_cnt);
        }
    }

    if (rep_fp)
    {
        fprintf(rep_fp, "\nSummary: Converted = %d, Skipped = %d | Total: %ld KB -> %ld KB\n",
                converted, skipped, total_orig >> 10, total_new >> 10);
        if (s_skip_log_cnt > 0)
        {
            fprintf(rep_fp, "\n--- Skipped files (%d) ---\n", s_skip_log_cnt);
            for (f = 0; f < s_skip_log_cnt; f++)
            {
                fprintf(rep_fp, "%s\n", s_skip_log[f]);
            }
            if (skipped > s_skip_log_cnt)
            {
                fprintf(rep_fp, "  ... and %d more skipped files.\n", skipped - s_skip_log_cnt);
            }
        }
        fclose(rep_fp);
        printf("[BATCH CNV] Report saved to SNDCNVRP.TXT\n");
    }
}

void FX_BatchConvertSoundsToADPCM(int target_bits)
{
    if (target_bits < 2 || target_bits > 5) target_bits = 4;
    printf("\n[CNVSND2WAVAD] Converting ALL root + GRP VOC/WAV files to %d-bit ADPCM...\n", target_bits);
    FX_RunBatchOverAllSoundFiles(0, target_bits);
}

void FX_BatchConvertADPCMToPCM(int out_wav_mode)
{
    printf("\n[CNVADPCMTOPCM] Decoding ALL root + GRP ADPCM WAVs to %s...\n",
           out_wav_mode ? "WAV" : "VOC");
    FX_RunBatchOverAllSoundFiles(1, out_wav_mode);
}

// Encode_ADPCM_DVI_smp() - Encode one 16-bit signed PCM sample into 2/3/4/5-bit
// Intel/DVI IMA ADPCM code using exact inverse of Decode_ADPCM_DVI_smp().
static int Encode_ADPCM_DVI_smp(int sample16, int bits, int *predictor, int *step_index)
{
    int diff, sign, code, step, vpdiff, i, sign_mask;

    diff = sample16 - *predictor;
    sign_mask = 1 << (bits - 1);

    if (diff < 0)
    {
        sign = sign_mask;
        diff = -diff;
    }
    else
    {
        sign = 0;
    }

    step = stepSizADPCMTabDVI[*step_index];
    code = 0;
    vpdiff = step >> (bits - 1);

    for (i = bits - 2; i >= 0; i--)
    {
        if (diff >= step)
        {
            code |= (1 << i);
            diff -= step;
            vpdiff += step;
        }
        step >>= 1;
    }

    if (sign)
    {
        *predictor -= vpdiff;
        if (*predictor < -32768) *predictor = -32768;
    }
    else
    {
        *predictor += vpdiff;
        if (*predictor > 32767) *predictor = 32767;
    }

    if (bits == 2)        *step_index += idxAdjADPCMTabDVI2b[code & 1];
    else if (bits == 3)   *step_index += idxAdjADPCMTabDVI3b[code & 3];
    else if (bits == 4)   *step_index += idxAdjADPCMTabDVI4b[code & 7];
    else                  *step_index += idxAdjADPCMTabDVI5b[code & 15];

    if (*step_index < 0)  *step_index = 0;
    if (*step_index > 88) *step_index = 88;

    return (code | sign);
}

// FX_ConvertSingleFileToADPCM() - Zero-allocache streaming VOC/WAV -> ADPCM WAV
static int FX_ConvertSingleFileToADPCM(const char *src_name, const char *dst_path,
                                       int bits, long *out_orig_sz, long *out_new_sz)
{
    long fp, flen, pos, chunk_sz, data_off, data_sz, blocks, out_data_sz;
    int fmt_tag, block_align, hdr_off, predictor, step_index, code, bit_cnt, b_in_blk, smp16;
    unsigned int bit_buf;
    unsigned char hdr_probe[64];
    unsigned char blk_out[ADPCM_MAX_BLK_SZ];
    unsigned char wav_hdr[ADPCM_WAV_HDR_SZ];
    cnv_src_stream_t st;
    FILE *fout;

    fp = kopen4load((char *)src_name, 0);
    if (fp == -1)
    {
        CNV_LogSkip(src_name, "kopen4load_fail", 0, 0);
        return -1;
    }

    flen = kfilelength(fp);
    if (flen <= 32)
    {
        kclose(fp);
        CNV_LogSkip(src_name, "too_small", flen, 0);
        return 0;
    }

    memset(&st, 0, sizeof(st));
    st.fp = fp;
    st.file_len = flen;
    st.sample_rate = 11025;
    st.src_bits = 8;
    st.src_channels = 1;

    if (kread(fp, hdr_probe, 32) != 32)
    {
        kclose(fp);
        return 0;
    }

    if (memcmp(hdr_probe, "Creative Voice File", 19) == 0)
    {
        hdr_off = (int)(hdr_probe[0x14] | (hdr_probe[0x15] << 8));
        if (hdr_off < 26 || hdr_off >= flen)
        {
            kclose(fp);
            CNV_LogSkip(src_name, "bad_voc_hdroff", hdr_off, flen);
            return -1;
        }
        st.is_voc = 1;
        CNV_SeekAbs(&st, (long)hdr_off);

        // Prime first sample so sample_rate is known before choosing block_align
        if (!CNV_NextSample16(&st, &smp16))
        {
            kclose(fp);
            CNV_LogSkip(src_name, "voc_no_pcm_samples", st.src_bits, st.src_channels);
            return -1;
        }
    }
    else if (memcmp(hdr_probe, "RIFF", 4) == 0 && memcmp(hdr_probe + 8, "WAVE", 4) == 0)
    {
        fmt_tag = 0;
        data_off = 0;
        data_sz = 0;
        pos = 12;
        klseek(fp, pos, SEEK_SET);

        while ((pos + 8) <= flen)
        {
            if (kread(fp, hdr_probe, 8) != 8) break;
            chunk_sz = (long)(hdr_probe[4] | (hdr_probe[5] << 8) |
                             (hdr_probe[6] << 16) | ((long)hdr_probe[7] << 24));
            if (chunk_sz < 0 || (pos + 8 + chunk_sz) > flen) break;

            if (memcmp(hdr_probe, "fmt ", 4) == 0 && chunk_sz >= 16)
            {
                if (kread(fp, hdr_probe + 8, 16) != 16) break;
                fmt_tag         = (int)(hdr_probe[8] | (hdr_probe[9] << 8));
                st.src_channels = (int)(hdr_probe[10] | (hdr_probe[11] << 8));
                st.sample_rate  = (long)(hdr_probe[12] | (hdr_probe[13] << 8) |
                                        (hdr_probe[14] << 16) | ((long)hdr_probe[15] << 24));
                st.src_bits     = (int)(hdr_probe[22] | (hdr_probe[23] << 8));
            }
            else if (memcmp(hdr_probe, "data", 4) == 0)
            {
                data_off = pos + 8;
                data_sz  = chunk_sz;
                break;
            }
            pos += 8 + chunk_sz + (chunk_sz & 1);
            klseek(fp, pos, SEEK_SET);
        }

        if (fmt_tag != 1 || st.src_channels != 1 || (st.src_bits != 8 && st.src_bits != 16) ||
            data_off <= 0 || data_sz <= 16)
        {
            kclose(fp);
            CNV_LogSkip(src_name, "wav_unsupported_fmt_ch_bits", fmt_tag, (st.src_channels << 8) | st.src_bits);
            return -1;
        }

        st.is_voc = 0;
        st.data_rem = data_sz;
        CNV_SeekAbs(&st, data_off);

        if (!CNV_NextSample16(&st, &smp16))
        {
            kclose(fp);
            return -1;
        }
    }
    else
    {
        kclose(fp);
        CNV_LogSkip(src_name, "unknown_container_sig", hdr_probe[0], hdr_probe[1]);
        return -1;
    }

    if (st.sample_rate < 22050L)      block_align = 256;
    else if (st.sample_rate < 44100L) block_align = 512;
    else                              block_align = 1024;

    fout = fopen(dst_path, "wb");
    if (!fout)
    {
        kclose(fp);
        CNV_LogSkip(src_name, "fopen_dst_fail", 0, 0);
        return -2;
    }

    memset(wav_hdr, 0, 48);
    fwrite(wav_hdr, 1, 48, fout);

    step_index = 0;
    blocks = 0;

    // Note: first sample is already in smp16 from priming above
    while (1)
    {
        memset(blk_out, 0, block_align);

        predictor = smp16;
        *(blk_out + 0) = (unsigned char)(predictor & 0xFF);
        *(blk_out + 1) = (unsigned char)((predictor >> 8) & 0xFF);
        *(blk_out + 2) = (unsigned char)(step_index & 0xFF);
        *(blk_out + 3) = 0;

        bit_buf = 0;
        bit_cnt = 0;
        b_in_blk = 4;

        while (b_in_blk < block_align)
        {
            while (bit_cnt < 8)
            {
                if (!CNV_NextSample16(&st, &smp16))
                {
                    smp16 = 0;
                }
                code = Encode_ADPCM_DVI_smp(smp16, bits, &predictor, &step_index);
                bit_buf |= ((unsigned int)code) << bit_cnt;
                bit_cnt += bits;
            }
            *(blk_out + b_in_blk) = (unsigned char)(bit_buf & 0xFF);
            b_in_blk++;
            bit_buf >>= 8;
            bit_cnt -= 8;
        }

        fwrite(blk_out, 1, block_align, fout);
        blocks++;

        // Try reading 1st sample of next block; if EOF, stop cleanly
        if (!CNV_NextSample16(&st, &smp16)) break;
    }

    kclose(fp);
    out_data_sz = blocks * (long)block_align;

    memset(wav_hdr, 0, 48);
    memcpy(wav_hdr + 0,  "RIFF", 4);
    *(unsigned long *)(wav_hdr + 4)  = 40UL + (unsigned long)out_data_sz;
    memcpy(wav_hdr + 8,  "WAVE", 4);
    memcpy(wav_hdr + 12, "fmt ", 4);
    *(unsigned long *)(wav_hdr + 16) = 20UL;
    *(unsigned short *)(wav_hdr + 20) = 0x0011;
    *(unsigned short *)(wav_hdr + 22) = 1;
    *(unsigned long *)(wav_hdr + 24) = (unsigned long)st.sample_rate;
    *(unsigned long *)(wav_hdr + 28) = (unsigned long)((st.sample_rate * (long)bits) / 8L);
    *(unsigned short *)(wav_hdr + 32) = (unsigned short)block_align;
    *(unsigned short *)(wav_hdr + 34) = (unsigned short)bits;
    *(unsigned short *)(wav_hdr + 36) = 2;
    *(unsigned short *)(wav_hdr + 38) = (unsigned short)(1 + (((block_align - 4) * 8) / bits));
    memcpy(wav_hdr + 40, "data", 4);
    *(unsigned long *)(wav_hdr + 44) = (unsigned long)out_data_sz;

    fseek(fout, 0L, SEEK_SET);
    fwrite(wav_hdr, 1, 48, fout);
    fclose(fout);

    *out_orig_sz = flen;
    *out_new_sz  = 48L + out_data_sz;
    return 1;
}

// FX_ConvertSingleADPCMToPCM() - Zero-allocache streaming ADPCM WAV -> VOC (0) or WAV (1)
// or standard 8-bit Mono PCM WAV (1) with native sample rate preserved.
#define PCM_OUTPATH_LEN  64
#define PCM_BASENAME_LEN 32
#define VOC_HDR_SZ       32
static int FX_ConvertSingleADPCMToPCM(const char *src_name, const char *dst_path,
                                      int out_wav_mode, long *out_orig_sz, long *out_new_sz)
{
    long fp, flen, pos, chunk_sz, data_off, data_sz;
    long sample_rate, total_blocks, b, total_out_samples, payload_bytes, byte_idx, voc_blk_len;
    int fmt_found, fmt_tag, channels, block_align, bits, mask, bit_cnt;
    int predictor, step_index, code, tc, pcm_wpos;
    unsigned int bit_buf;
    unsigned char hdr_probe[32];
    unsigned char blk_in[ADPCM_MAX_BLK_SZ];
    unsigned char wav_hdr[ADPCM_WAV_HDR_SZ];
    unsigned char voc_hdr[VOC_HDR_SZ];
    unsigned char voc_eof;
    short smp;
    FILE *fout;

    fp = kopen4load((char *)src_name, 0);
    if (fp == -1)
    {
        CNV_LogSkip(src_name, "kopen4load_fail", 0, 0);
        return -1;
    }

    flen = kfilelength(fp);
    if (flen <= 44)
    {
        kclose(fp);
        CNV_LogSkip(src_name, "too_small", flen, 0);
        return 0;
    }

    if (kread(fp, hdr_probe, 12) != 12 ||
        memcmp(hdr_probe, "RIFF", 4) != 0 || memcmp(hdr_probe + 8, "WAVE", 4) != 0)
    {
        kclose(fp);
        return -1;
    }

    fmt_found = 0;
    fmt_tag = 0;
    channels = 1;
    sample_rate = 11025;
    block_align = 256;
    bits = 4;
    data_off = 0;
    data_sz = 0;

    pos = 12;
    while ((pos + 8) <= flen)
    {
        if (kread(fp, hdr_probe, 8) != 8) break;
        chunk_sz = (long)(hdr_probe[4] | (hdr_probe[5] << 8) |
                         (hdr_probe[6] << 16) | ((long)hdr_probe[7] << 24));
        if (chunk_sz < 0 || (pos + 8 + chunk_sz) > flen) break;

        if (memcmp(hdr_probe, "fmt ", 4) == 0 && chunk_sz >= 16)
        {
            if (kread(fp, hdr_probe + 8, 16) != 16) break;
            fmt_tag     =  (int)(hdr_probe[8]         | (hdr_probe[9]  << 8));
            channels    =  (int)(hdr_probe[10]        | (hdr_probe[11] << 8));
            sample_rate = (long)(hdr_probe[12]        | (hdr_probe[13] << 8) |
                                (hdr_probe[14] << 16) | ((long)hdr_probe[15] << 24));
            block_align =  (int)(hdr_probe[20]        | (hdr_probe[21] << 8));
            bits        =  (int)(hdr_probe[22]        | (hdr_probe[23] << 8));
            fmt_found   = 1;
        }
        else if (memcmp(hdr_probe, "data", 4) == 0)
        {
            data_off = pos + 8;
            data_sz  = chunk_sz;
            break;
        }
        pos += 8 + chunk_sz + (chunk_sz & 1);
        klseek(fp, pos, SEEK_SET);
    }

    if (!fmt_found || fmt_tag != 0x0011 || channels != 1 ||
        data_off <= 0 || data_sz <= 4 || block_align <= 4 || block_align > ADPCM_MAX_BLK_SZ)
    {
        kclose(fp);
        CNV_LogSkip(src_name, "not_mono_dvi_adpcm", fmt_tag, (channels << 8) | bits);
        return -1;
    }

    if (bits < 2 || bits > 5) bits = 4;
    if (sample_rate <= 0) sample_rate = 11025;
    mask = (1 << bits) - 1;

    total_blocks = data_sz / (long)block_align;
    if (total_blocks <= 0)
    {
        kclose(fp);
        return -1;
    }

    fout = fopen(dst_path, "wb");
    if (!fout)
    {
        kclose(fp);
        return -2;
    }

    // Reserve header space (44 bytes for WAV or 32 bytes for VOC)
    memset(wav_hdr, 0, 48);
    fwrite(wav_hdr, 1, out_wav_mode ? 44 : 32, fout);

    klseek(fp, data_off, SEEK_SET);
    total_out_samples = 0;
    pcm_wpos = 0;
    payload_bytes = (long)block_align - 4L;

    for (b = 0; b < total_blocks; b++)
    {
        if (kread(fp, blk_in, (long)block_align) != (long)block_align) break;

        predictor  = (int)(short)(blk_in[0] | (blk_in[1] << 8));
        step_index = (int)blk_in[2];
        if (step_index < 0)  step_index = 0;
        if (step_index > 88) step_index = 88;

        if (predictor > 32512)  predictor = 32512;
        if (predictor < -32768) predictor = -32768;
        s_cnv_iobuf[pcm_wpos++] = (unsigned char)(((predictor + 128) >> 8) + 128);
        total_out_samples++;

        if (pcm_wpos >= CNV_STREAM_BUFSZ)
        {
            fwrite(s_cnv_iobuf, 1, (size_t)pcm_wpos, fout);
            pcm_wpos = 0;
        }

        bit_buf = 0;
        bit_cnt = 0;

        for (byte_idx = 0; byte_idx < payload_bytes; byte_idx++)
        {
            bit_buf |= ((unsigned int)blk_in[4 + byte_idx]) << bit_cnt;
            bit_cnt += 8;

            while (bit_cnt >= bits)
            {
                code = (int)(bit_buf & mask);
                bit_buf >>= bits;
                bit_cnt -= bits;

                smp = Decode_ADPCM_DVI_smp(code, bits, &predictor, &step_index);
                if (smp > 32512)  smp = 32512;
                if (smp < -32768) smp = -32768;
                s_cnv_iobuf[pcm_wpos++] = (unsigned char)((((int)smp + 128) >> 8) + 128);
                total_out_samples++;

                if (pcm_wpos >= CNV_STREAM_BUFSZ)
                {
                    fwrite(s_cnv_iobuf, 1, (size_t)pcm_wpos, fout);
                    pcm_wpos = 0;
                }
            }
        }
    }

    if (pcm_wpos > 0)
    {
        fwrite(s_cnv_iobuf, 1, (size_t)pcm_wpos, fout);
    }

    kclose(fp);

    if (out_wav_mode)
    {
        memset(wav_hdr, 0, 44);
        memcpy(wav_hdr + 0,  "RIFF", 4);
        *(unsigned long *)(wav_hdr + 4)  = 36UL + (unsigned long)total_out_samples;
        memcpy(wav_hdr + 8,  "WAVE", 4);
        memcpy(wav_hdr + 12, "fmt ", 4);
        *(unsigned long *)(wav_hdr + 16) = 16UL;
        *(unsigned short *)(wav_hdr + 20) = 1;
        *(unsigned short *)(wav_hdr + 22) = 1;
        *(unsigned long *)(wav_hdr + 24) = (unsigned long)sample_rate;
        *(unsigned long *)(wav_hdr + 28) = (unsigned long)sample_rate;
        *(unsigned short *)(wav_hdr + 32) = 1;
        *(unsigned short *)(wav_hdr + 34) = 8;
        memcpy(wav_hdr + 36, "data", 4);
        *(unsigned long *)(wav_hdr + 40) = (unsigned long)total_out_samples;

        fseek(fout, 0L, SEEK_SET);
        fwrite(wav_hdr, 1, 44, fout);
        *out_new_sz = 44L + total_out_samples;
    }
    else
    {
        voc_eof = 0x00;
        fwrite(&voc_eof, 1, 1, fout);

        tc = (int)(256L - (1000000L / sample_rate));
        if (tc < 0)   tc = 0;
        if (tc > 255) tc = 255;

        memset(voc_hdr, 0, 32);
        memcpy(voc_hdr + 0, "Creative Voice File\x1A", 20);
        *(unsigned short *)(voc_hdr + 20) = 26;
        *(unsigned short *)(voc_hdr + 22) = 0x010A;
        *(unsigned short *)(voc_hdr + 24) = 0x1129;

        voc_blk_len = total_out_samples + 2L;
        *(voc_hdr + 26) = 0x01;
        *(voc_hdr + 27) = (unsigned char)(voc_blk_len & 0xFF);
        *(voc_hdr + 28) = (unsigned char)((voc_blk_len >> 8) & 0xFF);
        *(voc_hdr + 29) = (unsigned char)((voc_blk_len >> 16) & 0xFF);
        *(voc_hdr + 30) = (unsigned char)tc;
        *(voc_hdr + 31) = 0;

        fseek(fout, 0L, SEEK_SET);
        fwrite(voc_hdr, 1, 32, fout);
        *out_new_sz = 33L + total_out_samples;
    }

    fclose(fout);
    *out_orig_sz = flen;
    return 1;
}

//-----------------------------------------------------------------------------
//******** 2-WAY PCM VOC/WAV TO IMA/DVI ADPCM WAV CONVERTER - FINISH **********
//*****************************************************************************

