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

#include <conio.h>
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "sndcards.h"
#include "fx_man.h"
#include "music.h"
#include "util_lib.h"
#include "duke3d.h"

#include <io.h>
#include <fcntl.h>
#include "midslic.h"

#include <stdlib.h>
#include <malloc.h>


#define LOUDESTVOLUME 150

long backflag,numenvsnds;
extern char midram;      // comline parameter declared in game.c
extern long midslicesize_kb; // force midi/xmidi chunk size for streaming, in game.c

//===================
//=
//= Initialize memory
//=
//===================
static long m_mus_buf_size = MUS_BUF_8MB;
static long m_mus_half_size = (MUS_BUF_8MB >> 1);
static unsigned int m_mus_slice_limit = SLICE_LIMIT_8MB;
static char *MusicPtr = NULL; // RAM amount dependent initial MIDI buffer size
static char m_mus_cache_lock = 255; // 255 = permanent lock in Build cache1d

static void calctotalmemory(void)
{
    unsigned int forced_limit = 0;
    long needed_buf = 0;

    if (totalmemory >= MUS_HEAP_64MB)
    {
        m_mus_buf_size = MUS_BUF_64MB;
        m_mus_slice_limit = SLICE_LIMIT_64MB;
    }
    else if (totalmemory >= MUS_HEAP_32MB)
    {
        m_mus_buf_size = MUS_BUF_32MB;
        m_mus_slice_limit = SLICE_LIMIT_32MB;
    }
    else if (totalmemory >= MUS_HEAP_24MB)
    {
        m_mus_buf_size = MUS_BUF_24MB;
        m_mus_slice_limit = SLICE_LIMIT_24MB;
    }
    else if (totalmemory >= MUS_HEAP_16MB)
    {
        m_mus_buf_size = MUS_BUF_16MB;
        m_mus_slice_limit = SLICE_LIMIT_16MB;
    }
    else if (totalmemory >= MUS_HEAP_12MB)
    {
        m_mus_buf_size = MUS_BUF_12MB;
        m_mus_slice_limit = SLICE_LIMIT_12MB;
    }
    else if (totalmemory >= MUS_HEAP_10MB)
    {
        m_mus_buf_size = MUS_BUF_10MB;
        m_mus_slice_limit = SLICE_LIMIT_10MB;
    }
    else
    {
        m_mus_buf_size = MUS_BUF_8MB;
        m_mus_slice_limit = SLICE_LIMIT_8MB;
    }

    m_mus_cache_lock = 255;

    if (midslicesize_kb > 0)
    {
        forced_limit = (unsigned int)(midslicesize_kb * 1024L);
        needed_buf = ((long)forced_limit * 2L) + 8192L;
   
        m_mus_slice_limit = forced_limit;
        // Expand MusicPtr if forced slice is larger than the RAM-tier buffer
        if (needed_buf > m_mus_buf_size)
        {
            m_mus_buf_size = needed_buf;
        }
    }

    allocache((long *)&MusicPtr, m_mus_buf_size, &m_mus_cache_lock);
    if (MusicPtr == NULL)
    {
        m_mus_buf_size = MUS_BUF_8MB;
        m_mus_slice_limit = SLICE_LIMIT_8MB;
        allocache((long *)&MusicPtr, m_mus_buf_size, &m_mus_cache_lock);
        if (MusicPtr == NULL)
        {
            Error("Out of memory allocating MusicPtr!");
        }
    }

    m_mus_half_size = m_mus_buf_size >> 1;
    memset(MusicPtr, 0, (size_t)m_mus_buf_size);
    DPMI_LockMemory(MusicPtr, (unsigned)m_mus_buf_size);
}

//===================
//=
//= SoundStartup
//=
//===================
void SoundStartup(void)
{
    int MaxVoices, MaxBits, MaxChannels;
    int32 status;

    // If they chose None lets return
    if (FXDevice == NumSoundCards)
        return;

    // Do special Sound Blaster, AWE32 stuff
    if ((FXDevice == SoundBlaster) || (FXDevice == Awe32))
    {
        status = FX_SetupSoundBlaster(BlasterConfig, (int *)&MaxVoices,
                                      (int *)&MaxBits, (int *)&MaxChannels);
    } else {
        status = FX_Ok;
    }

    if (status == FX_Ok)
    {
        if (eightytwofifty && numplayers > 1)
        {
            status = FX_Init(FXDevice, min(NumVoices, 4), 1, 8, 8000);
        } else {
            // Standard init for the engine to keep mixer running
            status = FX_Init(FXDevice, NumVoices, NumChannels, NumBits, MixRate);
        }
        
        if (status == FX_Ok)
        {
            FX_SetVolume(FXVolume);

            if (ReverseStereo == 1)
            {
                FX_SetReverseStereo(!FX_GetReverseStereo());
            }
        }
    }

    if (status != FX_Ok)
    {
        Error(FX_ErrorString(FX_Error));
    }

    status = FX_SetCallBack(TestCallBack);

    if (status != FX_Ok)
    {
        Error(FX_ErrorString(FX_Error));
    }
}

//===================
//=
//= SoundShutdown
//=
//===================

void SoundShutdown(void)
{
    int32 status;

    // if they chose None lets return
    if (FXDevice == NumSoundCards)
    {
        return;
    }

    status = FX_Shutdown();
    if (status != FX_Ok)
    {
        Error(FX_ErrorString(FX_Error));
    }
}

//===================
//=
//= MusicStartup
//=
//===================

void MusicStartup(void)
{
    int MaxVoices, MaxBits, MaxChannels;
    int32 status;

    // if they chose None lets return
    if ((MusicDevice == NumSoundCards) || (eightytwofifty && numplayers > 1))
    {
        return;
    }

    // satisfy AWE32 and WAVEBLASTER stuff
    BlasterConfig.Midi = MidiPort;

    // Do special Sound Blaster, AWE32 stuff
    if (
        (FXDevice == SoundBlaster) ||
        (FXDevice == Awe32)
    )
    {
        FX_SetupSoundBlaster(BlasterConfig, (int *)&MaxVoices,
                        (int *)&MaxBits, (int *)&MaxChannels);
    }

    calctotalmemory();

    status = MUSIC_Init(MusicDevice, MidiPort);

    if (status == MUSIC_Ok)
    {
        MUSIC_SetVolume(MusicVolume);
    }
    else
    {
        puts("Couldn't find selected sound card, or, error w/ sound card itself.");

        SoundShutdown();
        uninittimer();
        uninitengine();
        CONTROL_Shutdown();
        CONFIG_WriteSetup();
        KB_Shutdown();
        uninitgroupfile();
        unlink("duke3d.tmp");
        exit(-1);
    }
}

//===================
//=
//= MusicShutdown
//=
//===================

void MusicShutdown(void)
{
    int32 status;

    MIDISLIC_CloseStream();
    MIDISLIC_CloseTempCache();
    if (MusicPtr != NULL)
    {
        DPMI_UnlockMemory(MusicPtr, (unsigned)m_mus_buf_size);
        m_mus_cache_lock = 1;
        suckcache((long *)MusicPtr);
        MusicPtr = NULL;
    }

    if ((MusicDevice == NumSoundCards) || (eightytwofifty && numplayers > 1))
    {
        return;
    }

    status = MUSIC_Shutdown();
    if (status != MUSIC_Ok)
    {
        Error(MUSIC_ErrorString(MUSIC_ErrorCode));
    }
}

int USRHOOKS_GetMem(char **ptr, unsigned long size)
{
    *ptr = malloc(size);

    if (*ptr == NULL)
    {
        return(USRHOOKS_Error);
    }

    return(USRHOOKS_Ok);
}

int USRHOOKS_FreeMem(char *ptr)
{
    free(ptr);
    return(USRHOOKS_Ok);
}

char menunum = 0;

void intomenusounds(void)
{
    short i;
    short menusnds[] =
    {
        LASERTRIP_EXPLODE,
        DUKE_GRUNT,
        DUKE_LAND_HURT,
        CHAINGUN_FIRE,
        SQUISHED,
        KICK_HIT,
        PISTOL_RICOCHET,
        PISTOL_BODYHIT,
        PISTOL_FIRE,
        SHOTGUN_FIRE,
        BOS1_WALK,
        RPG_EXPLODE,
        PIPEBOMB_BOUNCE,
        PIPEBOMB_EXPLODE,
        NITEVISION_ONOFF,
        RPG_SHOOT,
        SELECT_WEAPON
    };
    sound(menusnds[menunum++]);
    menunum %= 17;
}

//===========================================
//=
//= Music Playback regular and disk-cached XMI stream
//=
//===========================================
#define XMI_TMP_FILENAME   "DUKE3DMU.TMP"

static int m_midislic_busy = 0;
static int m_midislic_activepage = 0;
static int m_midislic_tmp_fd = -1;
static int m_midislic_slice_count = 0;
static int m_midislic_disk_mode = 0;
static unsigned int m_midislic_nextbytes = 0;

static void MIDISLIC_CloseTempCache(void)
{
    if (m_midislic_tmp_fd != -1)
    {
        close(m_midislic_tmp_fd);
        m_midislic_tmp_fd = -1;
    }
    m_midislic_slice_count = 0;
    unlink(XMI_TMP_FILENAME);
}

static int MIDISLIC_BuildTempCache(const char *fn)
{
    unsigned int sz;

    MIDISLIC_CloseTempCache();

    if (!MIDISLIC_OpenStreamFile(fn))
    {
        return 0;
    }

    m_midislic_tmp_fd = open(XMI_TMP_FILENAME, O_BINARY | O_CREAT | O_TRUNC | O_RDWR, S_IREAD | S_IWRITE);
    if (m_midislic_tmp_fd == -1)
    {
        MIDISLIC_CloseStream();
        return 0;
    }

    // Render all slices once at level load (loop_song = 0) into a single sequential temp file
    while (1)
    {
        sz = MIDISLIC_RenderNextSlice((unsigned char *)MusicPtr, m_mus_slice_limit, 0);
        if (sz == 0)
        {
            break;
        }
        if (write(m_midislic_tmp_fd, &sz, sizeof(sz)) != sizeof(sz) ||
            write(m_midislic_tmp_fd, MusicPtr, sz) != (int)sz)
        {
            MIDISLIC_CloseStream();
            MIDISLIC_CloseTempCache();
            return 0;
        }
        m_midislic_slice_count++;
    }

    MIDISLIC_CloseStream();

    if (m_midislic_slice_count == 0)
    {
        MIDISLIC_CloseTempCache();
        return 0;
    }

    lseek(m_midislic_tmp_fd, 0L, SEEK_SET);
    return 1;
}

static void MIDISLIC_NotifyFallbackFTA(void)
{
    // Use quote slot 122 (or any spare slot) to display fallback notice on HUD
    strcpy(fta_quotes[122], "TEMP FILE FAILED: STREAMING DIRECTLY (SLOWER)");
    FTA(122, &ps[myconnectindex]);
}

static void MIDISLIC_PrefillShadowPage(void)
{
    int got, shadow_page;
    unsigned int sz32;
    unsigned char *dst;

    shadow_page = m_midislic_activepage ^ 1;
    dst = (unsigned char *)MusicPtr + (shadow_page * m_mus_half_size);

    // Fallback mode: direct on-the-fly rendering without temp file
    if (!m_midislic_disk_mode)
    {
        if (!MIDISLIC_IsStreamActive())
        {
            m_midislic_nextbytes = 0;
            return;
        }
        memset(dst, 0, m_mus_half_size);
        m_midislic_nextbytes = MIDISLIC_RenderNextSlice(dst, m_mus_slice_limit, 1);
        return;
    }

    // Fast disk-cached mode
    if (m_midislic_tmp_fd == -1 || m_midislic_slice_count <= 0)
    {
        m_midislic_nextbytes = 0;
        return;
    }

    got = read(m_midislic_tmp_fd, &sz32, sizeof(sz32));
    if (got == 0)
    {
        lseek(m_midislic_tmp_fd, 0L, SEEK_SET);
        got = read(m_midislic_tmp_fd, &sz32, sizeof(sz32));
    }

    if (got != sizeof(sz32) || sz32 == 0 || sz32 > (unsigned int)m_mus_half_size)
    {
        m_midislic_nextbytes = 0;
        return;
    }

    memset(dst, 0, m_mus_half_size);
    if (read(m_midislic_tmp_fd, dst, sz32) != (int)sz32)
    {
        m_midislic_nextbytes = 0;
        return;
    }

    m_midislic_nextbytes = sz32;
}

static int MIDISLIC_SwapPage(int skip_reset)
{
    int shadow_page, play_status;
    unsigned char *play_ptr;

    if (m_midislic_busy)
    {
        return 0;
    }
    if (m_midislic_disk_mode && m_midislic_tmp_fd == -1)
    {
        return 0;
    }
    if (!m_midislic_disk_mode && !MIDISLIC_IsStreamActive())
    {
        return 0;
    }

    if (m_midislic_nextbytes == 0)
    {
        MIDISLIC_CloseStream();
        MIDISLIC_CloseTempCache();
        m_midislic_disk_mode = 0;
        return 0;
    }

    m_midislic_busy = 1;

    shadow_page = m_midislic_activepage ^ 1;
    play_ptr = (unsigned char *)MusicPtr + (shadow_page * m_mus_half_size);

    if (skip_reset)
    {
        play_status = MIDI_HotSwapXmi(play_ptr);
    }
    else
    {
        play_status = MUSIC_PlaySong(play_ptr, MUSIC_PlayOnce);
    }

    if (play_status != MUSIC_Ok)
    {
        MIDISLIC_CloseStream();
        MIDISLIC_CloseTempCache();
        m_midislic_disk_mode = 0;
        m_midislic_busy = 0;
        return 0;
    }

    m_midislic_activepage = shadow_page;
    MIDISLIC_PrefillShadowPage();

    m_midislic_busy = 0;
    return 1;
}

void MIDISLIC_UpdatePlaylist(void)
{
    if (m_midislic_busy || MusicToggle == 0)
    {
        return;
    }
    if (!m_midislic_disk_mode && !MIDISLIC_IsStreamActive())
    {
        return;
    }
    if (m_midislic_disk_mode && m_midislic_tmp_fd == -1)
    {
        return;
    }

    if (!MUSIC_SongPlaying())
    {
        MIDISLIC_SwapPage(1);
    }
}

void MIDISLIC_StopMusic(void)
{
    m_midislic_busy = 1;
    MIDISLIC_CloseStream();
    MIDISLIC_CloseTempCache();
    m_midislic_disk_mode = 0;
    m_midislic_activepage = 0;
    m_midislic_nextbytes = 0;
    MUSIC_StopSong();
    m_midislic_busy = 0;
}

void playmusic(char *fn)
{
    long l;
    short fp;

    if (MusicToggle == 0)                 return;
    if (MusicDevice == NumSoundCards)     return;
    if (eightytwofifty && numplayers > 1) return;

    m_midislic_busy = 1;
    MUSIC_StopSong();
    MIDISLIC_CloseStream();
    MIDISLIC_CloseTempCache();
    m_midislic_disk_mode = 0;
    m_midislic_activepage = 0;
    m_midislic_nextbytes = 0;

    fp = kopen4load(fn, 0);
    if (fp == -1)
    {
        m_midislic_busy = 0;
        return;
    }

    l = kfilelength(fp);

    if (l < m_mus_buf_size)
    {
        memset(MusicPtr, 0, m_mus_buf_size);
        kread(fp, MusicPtr, l);
        kclose(fp);
        MUSIC_PlaySong(MusicPtr, MUSIC_LoopSong);
        m_midislic_busy = 0;
        return;
    }

    kclose(fp);

    if (!midram && MIDISLIC_BuildTempCache(fn))
    {
        m_midislic_disk_mode = 1;
    }
    else
    {
        // Notify via FTA only if temp file creation failed unexpectedly (not forced via /midram)
        if (!midram)
        {
            MIDISLIC_NotifyFallbackFTA();
        }
        if (!MIDISLIC_OpenStreamFile(fn))
        {
            m_midislic_busy = 0;
            return;
        }
        m_midislic_disk_mode = 0;
    }

    m_midislic_activepage = 0;
    MIDISLIC_PrefillShadowPage();

    m_midislic_busy = 0;
    MIDISLIC_SwapPage(0);
}

void playmusicold(char *fn) // don't touch it
{
    long l;
    short fp;

    if (MusicToggle == 0)                 return;
    if (MusicDevice == NumSoundCards)     return;
    if (eightytwofifty && numplayers > 1) return;

    fp = kopen4load(fn, 0);

    if (fp == -1)
    {
        return;
    }

    l = kfilelength(fp);
    // Remember MusicPtr[72000] ?
    if (l >= 72000)
    {
        kclose(fp);
        return;
    }

    kread(fp, MusicPtr, l);
    kclose(fp);
    MUSIC_PlaySong(MusicPtr, MUSIC_LoopSong);
}

//===================
//=
//= Sound stuff
//=
//===================

char loadsound(unsigned short num)
{
    long fp, l;

    if (num >= NUM_SOUNDS || SoundToggle == 0) return 0;
    if (FXDevice == NumSoundCards)             return 0;

    fp = kopen4load(sounds[num], loadfromgrouponly);
    if (fp == -1)
    {
        sprintf(&fta_quotes[113][0], "Sound %s(#%ld) not found.", sounds[num], num);
        FTA(113, &ps[myconnectindex]);
        return 0;
    }

    l = kfilelength(fp);
    soundsiz[num] = l;

    Sound[num].lock = 200;

    allocache((long *)&Sound[num].ptr, l, &Sound[num].lock);
    kread(fp, Sound[num].ptr, l);
    kclose(fp);
    return 1;
}

int xyzsound(short num, short i, long x, long y, long z)
{
    int voice, sndang, ca, pitch;
    long sndist, cx, cy, cz, j, k;
    short pitche, pitchs, cs;
    unsigned short start;

    //if(num != 358) return 0;

    if (
        num >= NUM_SOUNDS ||
        FXDevice == NumSoundCards ||
        ((soundm[num] & 8) && ud.lockout) ||
        SoundToggle == 0 ||
        Sound[num].num > 3 ||
        FX_VoiceAvailable(soundpr[num]) == 0 ||
        (ps[myconnectindex].timebeforeexit > 0 && ps[myconnectindex].timebeforeexit <= 26 * 3) ||
        ps[myconnectindex].gm & MODE_MENU
    )
    {
        return -1;
    }

    if (soundm[num] & 128)
    {
        sound(num);
        return 0;
    }

    if (soundm[num] & 4)
    {
        if (VoiceToggle == 0 || (ud.multimode > 1 && PN == APLAYER && sprite[i].yvel != screenpeek && ud.coop != 1))
        {
            return -1;
        }

        for (j = 0; j < NUM_SOUNDS; j++)
        {
            for (k = 0; k < Sound[j].num; k++)
            {
                if ((Sound[j].num > 0) && (soundm[j] & 4))
                {
                    return -1;
                }
            }
        }
    }

    cx = ps[screenpeek].oposx;
    cy = ps[screenpeek].oposy;
    cz = ps[screenpeek].oposz;
    cs = ps[screenpeek].cursectnum;
    ca = ps[screenpeek].ang + ps[screenpeek].look_ang;

    sndist = FindDistance3D((cx - x), (cy - y), (cz - z) >> 4);

    if (i >= 0 && (soundm[num] & 16) == 0 && PN == MUSICANDSFX && SLT < 999 && (sector[SECT].lotag & 0xff) < 9)
    {
        sndist = divscale14(sndist, (SHT + 1));
    }

    pitchs = soundps[num];
    pitche = soundpe[num];
    cx = klabs(pitche - pitchs);

    if (cx)
    {
        if (pitchs < pitche)
        {
            pitch = pitchs + (rand() % cx);
        }
        else
        {
            pitch = pitche + (rand() % cx);
        }
    }
    else
    {
        pitch = pitchs;
    }

    sndist += soundvo[num];
    if (sndist < 0)
    {
        sndist = 0;
    }
    if (sndist && PN != MUSICANDSFX && !cansee(cx, cy, cz - (24 << 8), cs, SX, SY, SZ - (24 << 8), SECT))
    {
        sndist += sndist >> 5;
    }

    switch (num)
    {
        case PIPEBOMB_EXPLODE:
        case LASERTRIP_EXPLODE:
        case RPG_EXPLODE:
            if (sndist > (6144))
            {
                sndist = 6144;
            }
            if (sector[ps[screenpeek].cursectnum].lotag == 2)
            {
                pitch -= 1024;
            }
            break;
        default:
            if (sector[ps[screenpeek].cursectnum].lotag == 2 && (soundm[num] & 4) == 0)
            {
                pitch = -768;
            }
            if (sndist > 31444 && PN != MUSICANDSFX)
            {
                return -1;
            }
            break;
    }

    if (Sound[num].num > 0 && PN != MUSICANDSFX)
    {
        if (SoundOwner[num][0].i == i)
        {
            stopsound(num);
        }
        else if (Sound[num].num > 1)
        {
            stopsound(num);
        }
        else if (badguy(&sprite[i]) && sprite[i].extra <= 0)
        {
            stopsound(num);
        }
    }

    if (PN == APLAYER && sprite[i].yvel == screenpeek)
    {
        sndang = 0;
        sndist = 0;
    }
    else
    {
        sndang = 2048 + ca - getangle(cx - x, cy - y);
        sndang &= 2047;
    }

    if (Sound[num].ptr == 0)
    {
        if (loadsound(num) == 0)
        {
            return 0;
        }
    }
    else
    {
        if (Sound[num].lock < 200)
        {
            Sound[num].lock = 200;
        }
        else
        {
            Sound[num].lock++;
        }
    }

    if (soundm[num] & 16)
    {
        sndist = 0;
    }

    if (sndist < ((255 - LOUDESTVOLUME) << 6))
    {
        sndist = ((255 - LOUDESTVOLUME) << 6);
    }

    if (soundm[num] & 1)
    {
        if (Sound[num].num > 0)
        {
            return -1;
        }

        start = *(unsigned short *)(Sound[num].ptr + 0x14);

        if (*Sound[num].ptr == 'C')
        {
            voice = FX_PlayLoopedVOC(Sound[num].ptr, start, start + soundsiz[num],
                           pitch, sndist >> 6, sndist >> 6, 0, soundpr[num], num);
        }
        else
        {
            voice = FX_PlayLoopedWAV(Sound[num].ptr, start, start + soundsiz[num],
                           pitch, sndist >> 6, sndist >> 6, 0, soundpr[num], num);
        }
    }
    else
    {
        if (*Sound[num].ptr == 'C')
        {
            voice = FX_PlayVOC3D(Sound[num].ptr, pitch, sndang >> 6, 
                                    sndist >> 6, soundpr[num], num);
        }
        else
        {
            voice = FX_PlayWAV3D(Sound[num].ptr, pitch, sndang >> 6, 
                                    sndist >> 6, soundpr[num], num);
        }
    }

    if (voice > FX_Ok)
    {
        SoundOwner[num][Sound[num].num].i = i;
        SoundOwner[num][Sound[num].num].voice = voice;
        Sound[num].num++;
    }
    else
    {
        Sound[num].lock--;
    }
    return (voice);
}

void sound(short num)
{
    int voice;
    long start;
    short pitch, pitche, pitchs, cx;

    if (FXDevice == NumSoundCards)             return;
    if (SoundToggle == 0)                      return;
    if (VoiceToggle == 0 && (soundm[num] & 4)) return;
    if ((soundm[num] & 8) && ud.lockout)       return;
    if (FX_VoiceAvailable(soundpr[num]) == 0)  return;

    pitchs = soundps[num];
    pitche = soundpe[num];
    cx = klabs(pitche - pitchs);

    if (cx)
    {
        if (pitchs < pitche)
        {
            pitch = pitchs + (rand() % cx);
        }
        else
        {
            pitch = pitche + (rand() % cx);
        }
    }
    else
    {
        pitch = pitchs;
    }

    if (Sound[num].ptr == 0)
    {
        if (loadsound(num) == 0)
        {
            return;
        }
    }
    else
    {
        if (Sound[num].lock < 200)
        {
            Sound[num].lock = 200;
        }
        else
        {
            Sound[num].lock++;
        }
    }

    if (soundm[num] & 1)
    {
        if (*Sound[num].ptr == 'C')
        {
            start = (long)*(unsigned short *)(Sound[num].ptr + 0x14);
            voice = FX_PlayLoopedVOC(Sound[num].ptr, start, start + soundsiz[num],
                                     pitch, LOUDESTVOLUME, LOUDESTVOLUME, LOUDESTVOLUME, 
                                                                     soundpr[num], num);
        }
        else
        {
            start = (long)*(unsigned short *)(Sound[num].ptr + 0x14);
            voice = FX_PlayLoopedWAV(Sound[num].ptr, start, start + soundsiz[num],
                 pitch, LOUDESTVOLUME, LOUDESTVOLUME, LOUDESTVOLUME, soundpr[num], num);
        }
    }
    else
    {
        if (*Sound[num].ptr == 'C')
        {
            voice = FX_PlayVOC3D(Sound[num].ptr, pitch, 0, 255 - LOUDESTVOLUME, 
                                                                     soundpr[num], num);
        }
        else
        {
            voice = FX_PlayWAV3D(Sound[num].ptr, pitch, 0, 255 - LOUDESTVOLUME, 
                                                                     soundpr[num], num);
        }
    }

    if (voice > FX_Ok)
    {
        return;
    }
    Sound[num].lock--;
}

int spritesound(unsigned short num, short i)
{
    if (num >= NUM_SOUNDS)
    {
        return -1;
    }
    return xyzsound(num, i, SX, SY, SZ);
}

void stopsound(short num)
{
    if (Sound[num].num > 0)
    {
        FX_StopSound(SoundOwner[num][Sound[num].num - 1].voice);
        testcallback(num);
    }
}

void stopenvsound(short num, short i)
{
    short j, k;

    if (Sound[num].num > 0)
    {
        k = Sound[num].num;
        for (j = 0; j < k; j++)
        {
            if (SoundOwner[num][j].i == i)
            {
                FX_StopSound(SoundOwner[num][j].voice);
                break;
            }
        }
    }
}

void pan3dsound(void)
{
    long sndist, sx, sy, sz, cx, cy, cz;
    short sndang, ca, j, k, i, cs;

    numenvsnds = 0;

    if (ud.camerasprite == -1)
    {
        cx = ps[screenpeek].oposx;
        cy = ps[screenpeek].oposy;
        cz = ps[screenpeek].oposz;
        cs = ps[screenpeek].cursectnum;
        ca = ps[screenpeek].ang + ps[screenpeek].look_ang;
    }
    else
    {
        cx = sprite[ud.camerasprite].x;
        cy = sprite[ud.camerasprite].y;
        cz = sprite[ud.camerasprite].z;
        cs = sprite[ud.camerasprite].sectnum;
        ca = sprite[ud.camerasprite].ang;
    }

    for (j = 0; j < NUM_SOUNDS; j++)
    {
        for (k = 0; k < Sound[j].num; k++)
        {
            i = SoundOwner[j][k].i;

            sx = sprite[i].x;
            sy = sprite[i].y;
            sz = sprite[i].z;

            if (PN == APLAYER && sprite[i].yvel == screenpeek)
            {
                sndang = 0;
                sndist = 0;
            }
            else
            {
                sndang = 2048 + ca - getangle(cx - sx, cy - sy);
                sndang &= 2047;
                sndist = FindDistance3D((cx - sx), (cy - sy), (cz - sz) >> 4);
                if (i >= 0 && (soundm[j] & 16) == 0 && PN == MUSICANDSFX && SLT < 999 && 
                                                         (sector[SECT].lotag & 0xff) < 9)
                {
                    sndist = divscale14(sndist, (SHT + 1));
                }
            }

            sndist += soundvo[j];
            if (sndist < 0)
            {
                sndist = 0;
            }

            if (sndist && PN != MUSICANDSFX && !cansee(cx, cy, cz - (24 << 8), 
                                            cs, sx, sy, sz - (24 << 8), SECT))
            {
                sndist += sndist >> 5;
            }

            if (PN == MUSICANDSFX && SLT < 999)
            {
                numenvsnds++;
            }

            switch (j)
            {
                case PIPEBOMB_EXPLODE:
                case LASERTRIP_EXPLODE:
                case RPG_EXPLODE:
                    if (sndist > (6144))
                    {
                        sndist = (6144);
                    }
                    break;
                default:
                    if (sndist > 31444 && PN != MUSICANDSFX)
                    {
                        stopsound(j);
                        continue;
                    }
            }

            if (Sound[j].ptr == 0 && loadsound(j) == 0)
            {
                continue;
            }
            if (soundm[j] & 16)
            {
                sndist = 0;
            }

            if (sndist < ((255 - LOUDESTVOLUME) << 6))
            {
                sndist = ((255 - LOUDESTVOLUME) << 6);
            }

            FX_Pan3D(SoundOwner[j][k].voice, sndang >> 6, sndist >> 6);
        }
    }
}

void TestCallBack(long num)
{
    short tempi, tempj, tempk;

    if (num < 0)
    {
        if (lumplockbyte[-num] >= 200)
        {
            lumplockbyte[-num]--;
        }
        return;
    }

    tempk = Sound[num].num;

    if (tempk > 0)
    {
        if ((soundm[num] & 16) == 0)
        {
            for (tempj = 0; tempj < tempk; tempj++)
            {
                tempi = SoundOwner[num][tempj].i;
                if (sprite[tempi].picnum == MUSICANDSFX && 
                    sector[sprite[tempi].sectnum].lotag < 3 &&
                                      sprite[tempi].lotag < 999)
                {
                    hittype[tempi].temp_data[0] = 0;
                    if ((tempj + 1) < tempk)
                    {
                        SoundOwner[num][tempj].voice = SoundOwner[num][tempk - 1].voice;
                        SoundOwner[num][tempj].i = SoundOwner[num][tempk - 1].i;
                    }
                    break;
                }
            }
        }

        Sound[num].num--;
        SoundOwner[num][tempk - 1].i = -1;
    }

    Sound[num].lock--;
}

void clearsoundlocks(void)
{
    long i;

    MIDISLIC_UpdatePlaylist();

    for (i = 0; i < NUM_SOUNDS; i++)
    {
        if (Sound[i].lock >= 200)
        {
            Sound[i].lock = 199;
        }
    }

    for (i = 0; i < 11; i++)
    {
        if (lumplockbyte[i] >= 200)
        {
            lumplockbyte[i] = 199;
        }
    }
}
