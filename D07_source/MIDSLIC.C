//============================================================================
// MIDSLIC.C
// Ultra Low-RAM Disk/GRP Streaming MIDI & XMI Slicer for Duke3D (Watcom C)
// K-way track merge directly from cache1d stream into 72,000-byte MusicPtr.
//============================================================================

#include <string.h>
#include <io.h>
#include <fcntl.h>
#include "midslic.h"

// External cache1d stream functions from engine/cache1d.c
extern long kopen4group_stream(char *filename);
extern long kopen4load(char *filename, char searchfirst);
extern long kread_stream(long handle, void *buffer, long leng);
extern long klseek_stream(long handle, long offset, long whence);
extern long kfilelength_stream(long handle);
extern long kfilelength(long handle);
extern void kclose_stream(long handle);

#define MIDISLIC_XMI_FREQ         120UL
#define MIDISLIC_DEFAULT_TIMEBASE 60UL
#define MIDISLIC_MAX_CHANNELS     16
#define MIDISLIC_MAX_NOTES        128
#define MIDISLIC_MAX_TIMB_CNT     256

// like if you counted AMOUNT of tracks in a DAW like FL Studio
// ATTENTION that the 1st track is considered a SERVICE one
// and is OUT of the equation, so it must be always amount + 1
#define MIDISLIC_MAX_TRACKS       32

// to handle long notes hold, larger buffer is better but more RAM!
#define MIDISLIC_TRK_BUF_SIZE     2048 // a golden middle

#define MIDISLIC_XMI_OFF_MAX      256

struct SlicTrackStream
{
	int active;
	long start_off;
	long end_off;
	long file_pos;
	unsigned int buf_pos;
	unsigned int buf_len;
	unsigned char running_status;
	unsigned long cur_tick;
	int has_event;
	unsigned char ev_status;
	unsigned char ev_d1;
	unsigned char ev_d2;
	unsigned long ev_dur;
	unsigned int ev_order;
	unsigned char *buf_ptr;
};

struct SlicXmiPendingOff
{
	unsigned long rel_ticks;
	unsigned char ch;
	unsigned char note;
};

// Static BSS pools (zero heap usage)
static unsigned char m_trk_buf_pool[MIDISLIC_MAX_TRACKS * MIDISLIC_TRK_BUF_SIZE];
static struct SlicTrackStream m_tracks[MIDISLIC_MAX_TRACKS];
static unsigned short m_slice_timbres[MIDISLIC_MAX_TIMB_CNT];
static unsigned short m_local_timbres[MIDISLIC_MAX_TIMB_CNT];
static short m_last_cc_val[MIDISLIC_MAX_CHANNELS * MIDISLIC_MAX_NOTES];
static int m_last_pitch_val[MIDISLIC_MAX_CHANNELS];
static unsigned char m_ch_prog[MIDISLIC_MAX_CHANNELS];
static unsigned char m_ch_bank[MIDISLIC_MAX_CHANNELS];
static unsigned char m_ch_vol[MIDISLIC_MAX_CHANNELS];
static unsigned char m_ch_pan[MIDISLIC_MAX_CHANNELS];
static unsigned char m_ch_pitch_lsb[MIDISLIC_MAX_CHANNELS];
static unsigned char m_ch_pitch_msb[MIDISLIC_MAX_CHANNELS];
static unsigned char m_ch_has_pitch[MIDISLIC_MAX_CHANNELS];
static unsigned char m_ch_rpn_msb[MIDISLIC_MAX_CHANNELS];
static unsigned char m_ch_rpn_lsb[MIDISLIC_MAX_CHANNELS];
static unsigned char m_ch_rpn_range[MIDISLIC_MAX_CHANNELS];
static unsigned char m_ch_has_rpn[MIDISLIC_MAX_CHANNELS];
static unsigned int m_xmi_off_cnt = 0;
static unsigned char m_io_tmp[16];
static unsigned long m_active_note_end[MIDISLIC_MAX_CHANNELS][MIDISLIC_MAX_NOTES];
static unsigned char m_active_note_vel[MIDISLIC_MAX_CHANNELS][MIDISLIC_MAX_NOTES];
static struct SlicXmiPendingOff m_xmi_offs[MIDISLIC_XMI_OFF_MAX];

// Global stream state
static long m_file_handle = -1;
static long m_file_size = 0;
static int m_stream_active = 0;
static int m_is_xmi = 0;
static int m_is_smpte = 0;
static unsigned short m_division = MIDISLIC_DEFAULT_TIMEBASE;

static unsigned long m_smpte_num = 0;
static unsigned long m_smpte_den = 1;
static unsigned long m_tick_num = 150000UL;
static unsigned long m_tick_den = 150000UL;
static unsigned long m_dda_sum = 0;

static unsigned long m_xmi_interval = 0;
static unsigned long m_last_raw_tick = 0;
static unsigned long m_prev_xmi_tick = 0;
static int m_num_tracks = 0;
static int m_slice_index = 0;
static char m_fname_buf[64];
static unsigned long m_last_slice_ticks = 0;



//------------------+------+------------------------------------------
// Component        | Size |Purpose                                  
//                  | bytes|                                          
//------------------+------+------------------------------------------
// MusicPtr         |72000 |Divided into two 36,000-byte pages       
// (original Duke3D |      |(ping-pong buffer)                       
// buffer)          |      |                                        
//------------------+------+------------------------------------------
// m_trk_buf_pool   |32768 |Sliding read windows for 32 MIDI/XMI trac
// (32 x 1024)      |      |                                         
//------------------+------+------------------------------------------
// m_active_note_end|6144  |Long-note tracking table                 
// + velocity       |      |(Note Chopping, 16-bit remaining ticks)  
// (16 x 128 x 3)   |      |                                         
//------------------+------+------------------------------------------
// m_last_cc_val    |4096  |Deduplication of repeated CC controllers 
// (16 x 128 x 2)   |      |                                         
//------------------+------+------------------------------------------
// _MIDI_XmiOffQueue|2048  |Queue for deferred Note-Off events in    
// (MIDI.C)         |      |MIDI.C                                   
//------------------+------+------------------------------------------
// Track and channel|4200  |State of 32 tracks (m_tracks), 16 channels
// utility arrays   |      |and timbres                              
//------------------+------+------------------------------------------
// TOTAL: add-on    |49256 |Total streaming overhead (BSS + heap)    
// (BSS + heap)     |      |                                         
//------------------+------+------------------------------------------
// TOTAL mus memory |121256|Instead of the 1-4+ MB required for heavy
// (includ MusicPtr)|      |MIDI/XMI files and conversion            
//------------------+------+------------------------------------------



//----------------------------------------------------------------------------
// Byte-order and VLQ helpers (no square brackets used)
//----------------------------------------------------------------------------
static void MIDISLIC_write_be32_mem(unsigned char *dst, unsigned int val)
{
	*(dst + 0) = (unsigned char)((val >> 24) & 0xFF);
	*(dst + 1) = (unsigned char)((val >> 16) & 0xFF);
	*(dst + 2) = (unsigned char)((val >> 8) & 0xFF);
	*(dst + 3) = (unsigned char)(val & 0xFF);
}

static unsigned int MIDISLIC_read_be32(const unsigned char *p)
{
	return ((unsigned int)(*(p + 0)) << 24) |
		((unsigned int)(*(p + 1)) << 16) |
		((unsigned int)(*(p + 2)) << 8) |
		(unsigned int)(*(p + 3));
}

static unsigned short MIDISLIC_read_be16(const unsigned char *p)
{
	return (unsigned short)(((unsigned short)(*(p + 0)) << 8) |
		(unsigned short)(*(p + 1)));
}

static void MIDISLIC_write_vlq(unsigned char **ptr, unsigned long val)
{
	unsigned long buffer = val & 0x7F;
	while ((val >>= 7))
	{
		buffer <<= 8;
		buffer |= (unsigned long)((val & 0x7F) | 0x80);
	}
	while (1)
	{
		*(*ptr)++ = (unsigned char)(buffer & 0xFF);
		if (buffer & 0x80)
		{
			buffer >>= 8;
		}
		else
		{
			break;
		}
	}
}

static unsigned int MIDISLIC_vlq_len(unsigned long val)
{
	unsigned int len = 1;
	while ((val >>= 7))
	{
		len++;
	}
	return len;
}

static void MIDISLIC_write_xmi_delay(unsigned char **ptr, unsigned long delay)
{
	int need_final = 0;
	if (delay == 0)
	{
		return;
	}
	while (delay >= 0x7FUL)
	{
		*(*ptr)++ = 0x7F;
		delay -= 0x7FUL;
		need_final = 1;
	}
	if (delay > 0 || need_final)
	{
		*(*ptr)++ = (unsigned char)delay;
	}
}

static unsigned int MIDISLIC_xmi_delay_len(unsigned long delay)
{
	unsigned int len = 0;
	int need_final = 0;
	if (delay == 0)
	{
		return 0;
	}
	while (delay >= 0x7FUL)
	{
		len++;
		delay -= 0x7FUL;
		need_final = 1;
	}
	if (delay > 0 || need_final)
	{
		len++;
	}
	return len;
}

//----------------------------------------------------------------------------
// Track sliding-window stream reader
//----------------------------------------------------------------------------

static int MIDISLIC_trk_refill(struct SlicTrackStream *tr)
{
	long rem;
	long to_read;
	long got;

	if (tr->file_pos >= tr->end_off)
	{
		return 0;
	}

	rem = tr->end_off - tr->file_pos;
	to_read = (rem > MIDISLIC_TRK_BUF_SIZE) ? MIDISLIC_TRK_BUF_SIZE : rem;

	klseek_stream(m_file_handle, tr->file_pos, SEEK_SET);
	got = kread_stream(m_file_handle, tr->buf_ptr, to_read);
	if (got <= 0)
	{
		tr->file_pos = tr->end_off;
		return 0;
	}

	tr->file_pos += got;
	tr->buf_pos = 0;
	tr->buf_len = (unsigned int)got;
	return 1;
}

static int MIDISLIC_trk_get_byte(struct SlicTrackStream *tr, unsigned char *out_b)
{
	if (tr->buf_pos >= tr->buf_len)
	{
		if (!MIDISLIC_trk_refill(tr))
		{
			return 0;
		}
	}
	*out_b = *(tr->buf_ptr + tr->buf_pos);
	tr->buf_pos++;
	return 1;
}

static void MIDISLIC_trk_skip_bytes(struct SlicTrackStream *tr, unsigned long count)
{
	while (count > 0)
	{
		unsigned int avail = tr->buf_len - tr->buf_pos;
		if (avail > 0)
		{
			unsigned long step = ((unsigned long)avail < count) ? (unsigned long)avail : count;
			tr->buf_pos += (unsigned int)step;
			count -= step;
		}
		else
		{
			long rem = tr->end_off - tr->file_pos;
			if (rem <= 0)
			{
				break;
			}
			if (count >= (unsigned long)rem)
			{
				tr->file_pos = tr->end_off;
				tr->buf_pos = 0;
				tr->buf_len = 0;
				break;
			}
			tr->file_pos += (long)count;
			tr->buf_pos = 0;
			tr->buf_len = 0;
			break;
		}
	}
}

static unsigned long MIDISLIC_trk_read_vlq(struct SlicTrackStream *tr, int *ok)
{
	int guard = 0;
	unsigned long val = 0;
	unsigned char c;

	*ok = 0;
	while (guard < 5)
	{
		if (!MIDISLIC_trk_get_byte(tr, &c))
		{
			return val;
		}
		val = (val << 7) | (unsigned long)(c & 0x7F);
		guard++;
		if (!(c & 0x80))
		{
			*ok = 1;
			return val;
		}
	}
	*ok = 1;
	return val;
}

static void MIDISLIC_trk_ensure_lookahead(struct SlicTrackStream *tr, unsigned int min_bytes)
{
    unsigned int avail;
    long rem, to_read, got;

    if (!tr || tr->buf_len < tr->buf_pos)
    {
        return;
    }

    avail = tr->buf_len - tr->buf_pos;
    if (avail >= min_bytes || tr->file_pos >= tr->end_off)
    {
        return;
    }

    if (avail > 0 && tr->buf_pos > 0)
    {
        memmove(tr->buf_ptr, tr->buf_ptr + tr->buf_pos, avail);
    }
    tr->buf_pos = 0;
    tr->buf_len = avail;

    rem = tr->end_off - tr->file_pos;
    to_read = (long)(MIDISLIC_TRK_BUF_SIZE - avail);
    if (to_read > rem)
    {
        to_read = rem;
    }

    if (to_read > 0)
    {
        klseek_stream(m_file_handle, tr->file_pos, SEEK_SET);
        got = kread_stream(m_file_handle, tr->buf_ptr + avail, to_read);
        if (got > 0)
        {
            tr->file_pos += got;
            tr->buf_len += (unsigned int)got;
        }
    }
}

static unsigned long MIDISLIC_peek_midi_note_duration(struct SlicTrackStream *tr,
       unsigned char target_ch, unsigned char target_note, unsigned char start_rs)
{
    int guard;
    unsigned int pos = tr->buf_pos;
    unsigned int end = tr->buf_len;
    unsigned char rs = start_rs;
    unsigned long acc_ticks = 0;
    unsigned long d;
    unsigned char b, st, cmd;
    unsigned char ch, d1, d2;
    unsigned long mlen;

    while (pos < end)
    {
        d = 0;
        guard = 0;
        while (pos < end && guard < 4)
        {
            b = *(tr->buf_ptr + pos++);
            d = (d << 7) | (unsigned long)(b & 0x7F);
            guard++;
            if (!(b & 0x80)) break;
        }
        acc_ticks += d;
        if (acc_ticks > 60000UL) break; // remove notes longer than 60s
        if (pos >= end) break;

        b = *(tr->buf_ptr + pos);
        if (b & 0x80)
        {
            st = b;
            pos++;
            if (st < 0xF0) rs = st;
        }
        else
        {
            st = rs;
            if (st == 0) break;
        }

        cmd = st & 0xF0;
        ch = st & 0x0F;

        if (cmd == 0x80 || cmd == 0x90 || cmd == 0xA0 || cmd == 0xB0 || cmd == 0xE0)
        {
            if (pos + 1 >= end) break;
            d1 = *(tr->buf_ptr + pos++);
            d2 = *(tr->buf_ptr + pos++);

            if (ch == target_ch && d1 == target_note)
            {
            	// Stop counting note length on a repeated Note-On
            	if (cmd == 0x80 || (cmd == 0x90 && d2 == 0))
            	{
		return (acc_ticks > 0) ? acc_ticks : 1UL;
            	}
            	if (cmd == 0x90 && d2 > 0)
            	{
            		return (acc_ticks > 0) ? (acc_ticks - 1UL) : 1UL;
            	}
            }
        }
        else if (cmd == 0xC0 || cmd == 0xD0)
        {
            if (pos >= end) break;
            pos++;
        }
        else if (st == 0xFF || st == 0xF0 || st == 0xF7)
        {
            if (st == 0xFF)
            {
                if (pos >= end) break;
                pos++;
            }
            mlen = 0;
            guard = 0;
            while (pos < end && guard < 4)
            {
                b = *(tr->buf_ptr + pos++);
                mlen = (mlen << 7) | (unsigned long)(b & 0x7F);
                guard++;
                if (!(b & 0x80)) break;
            }
            if (pos + mlen > end) break;
            pos += (unsigned int)mlen;
        }
        else
        {
            break;
        }
    }
    return 0;
}

//----------------------------------------------------------------------------
// Fetch next playable event for a MIDI track
//----------------------------------------------------------------------------
static int MIDISLIC_midi_fetch_next(struct SlicTrackStream *tr)
{
	int ok;
	unsigned long mlen, delta;
	unsigned long tempo_us, raw_ticks;
	unsigned long clamped_raw, xmi_t;
	unsigned char b, status, cmd, ch;
	unsigned char mtype, t0, t1, t2;

	while (tr->active)
	{
		delta = MIDISLIC_trk_read_vlq(tr, &ok);
		if (!ok)
		{
			tr->active = 0;
			tr->has_event = 0;
			return 0;
		}

		if (delta > 500000UL)
		{
			delta = 500000UL;
		}
		tr->cur_tick += delta;

                if (!MIDISLIC_trk_get_byte(tr, &b)) // line#515 - type of parameter 2 does not agree with previous definition
		{
			tr->active = 0;
			tr->has_event = 0;
			return 0;
		}

		if (b & 0x80)
		{
			status = b;
			if (status < 0xF0)
			{
				tr->running_status = status;
			}
		}
		else
		{
			status = tr->running_status;
			if (status == 0)
			{
				continue;
			}
			// Push back first data byte by stepping buf_pos back
			tr->buf_pos--;
		}

		cmd = status & 0xF0;
		ch = status & 0x0F;

		if (cmd == 0x80 || cmd == 0x90 || cmd == 0xA0 || cmd == 0xB0 || cmd == 0xE0)
		{
			if (!MIDISLIC_trk_get_byte(tr, &t0) || !MIDISLIC_trk_get_byte(tr, &t1))
			{
				tr->active = 0;
				tr->has_event = 0;
				return 0;
			}
		
			if (cmd == 0x90 && t1 == 0)
			{
				status = (unsigned char)(0x80 | ch);
				cmd = 0x80;
			}
		
			if (cmd == 0xA0)
			{
				// Skip poly aftertouch
				continue;
			}
		
			tr->has_event = 1;
			tr->ev_status = status;
			tr->ev_d1 = t0;
			tr->ev_d2 = t1;

			if (cmd == 0x90)
			{
				MIDISLIC_trk_ensure_lookahead(tr, 768U);
				raw_ticks = MIDISLIC_peek_midi_note_duration(tr, ch, t0, tr->running_status);
				if (raw_ticks > 0 && m_tick_den > 0)
				{
					clamped_raw = (raw_ticks > 16384UL) ? 16384UL : raw_ticks;
					xmi_t = (clamped_raw * m_tick_num) / m_tick_den;
					if (xmi_t < 4UL) xmi_t = 4UL;
					if (xmi_t > 32000UL) xmi_t = 32000UL;
					tr->ev_dur = xmi_t;
				}
				else
				{
					// Note-Off lies beyond 512-byte lookahead -> 
					// -> treat as long pad (8s, chopped cleanly across slices)
					tr->ev_dur = (ch == 9) ? 12UL : 960UL;
				}
			}
			else
			{
				tr->ev_dur = 0;
			}
			return 1;
		}
		else if (cmd == 0xC0 || cmd == 0xD0)
		{
			if (!MIDISLIC_trk_get_byte(tr, &t0))
			{
				tr->active = 0;
				tr->has_event = 0;
				return 0;
			}
			if (cmd == 0xD0)
			{
				continue;
			}
			tr->has_event = 1;
			tr->ev_status = status;
			tr->ev_d1 = t0;
			tr->ev_d2 = 0;
			tr->ev_dur = 0;
			return 1;
		}
		else if (status == 0xFF)
		{
			if (!MIDISLIC_trk_get_byte(tr, &mtype))
			{
				tr->active = 0;
				tr->has_event = 0;
				return 0;
			}
			mlen = MIDISLIC_trk_read_vlq(tr, &ok);
			if (!ok)
			{
				tr->active = 0;
				tr->has_event = 0;
				return 0;
			}
			if (mtype == 0x2F)
			{
				tr->active = 0;
				tr->has_event = 0;
				return 0;
			}
			else if (mtype == 0x51 && mlen == 3 && !m_is_smpte)
			{
				if (!MIDISLIC_trk_get_byte(tr, &t0) ||
					!MIDISLIC_trk_get_byte(tr, &t1) ||
					!MIDISLIC_trk_get_byte(tr, &t2))
				{
					tr->active = 0;
					tr->has_event = 0;
					return 0;
				}
				tempo_us = ((unsigned long)t0 << 16) |
					((unsigned long)t1 << 8) |
					(unsigned long)t2;
				tr->has_event = 1;
				tr->ev_status = 0xFF;
				tr->ev_d1 = 0x51;
				tr->ev_d2 = 0;
				tr->ev_dur = tempo_us;
				return 1;
			}
			else
			{
				MIDISLIC_trk_skip_bytes(tr, mlen);
				continue;
			}
		}
		else if (status == 0xF0 || status == 0xF7)
		{
			mlen = MIDISLIC_trk_read_vlq(tr, &ok);
			if (!ok)
			{
				tr->active = 0;
				tr->has_event = 0;
				return 0;
			}
			MIDISLIC_trk_skip_bytes(tr, mlen);
			continue;
		}
	}
	tr->has_event = 0;
	return 0;
}

//----------------------------------------------------------------------------
// Fetch next playable event for a native XMI EVNT stream
//----------------------------------------------------------------------------
static int MIDISLIC_xmi_fetch_next(struct SlicTrackStream *tr)
{
    int ok;
    unsigned long dur, mlen, delay = 0;
    unsigned char b, status, cmd, t0, t1;

    while (tr->active)
    {
        if (!MIDISLIC_trk_get_byte(tr, &b))
        {
            tr->active = 0;
            tr->has_event = 0;
            return 0;
        }
        if (b < 0x80)
        {
            while (b == 0x7F)
            {
                delay += 0x7FUL;
                if (!MIDISLIC_trk_get_byte(tr, &b))
                {
                    tr->active = 0;
                    tr->has_event = 0;
                    return 0;
                }
            }
            delay += (unsigned long)b;
            tr->cur_tick += delay;
            continue;
        }
        status = b;
        cmd = status & 0xF0;
        if (status == 0xFF)
        {
            if (!MIDISLIC_trk_get_byte(tr, &t0))
            {
                tr->active = 0;
                tr->has_event = 0;
                return 0;
            }
            if (t0 == 0x2F)
            {
                tr->active = 0;
                tr->has_event = 0;
                return 0;
            }
            mlen = MIDISLIC_trk_read_vlq(tr, &ok);
            if (!ok)
            {
                tr->active = 0;
                tr->has_event = 0;
                return 0;
            }
            MIDISLIC_trk_skip_bytes(tr, mlen);
            continue;
        }
        else if (cmd == 0x90)
        {
            if (!MIDISLIC_trk_get_byte(tr, &t0) || !MIDISLIC_trk_get_byte(tr, &t1))
            {
                tr->active = 0;
                tr->has_event = 0;
                return 0;
            }
            dur = MIDISLIC_trk_read_vlq(tr, &ok);
            if (!ok) dur = 12UL;
            if (dur > 120UL) dur = 120UL;
            tr->has_event = 1;
            tr->ev_status = status;
            tr->ev_d1 = t0;
            tr->ev_d2 = t1;
            tr->ev_dur = dur;
            return 1;
        }
        else if (cmd == 0x80 || cmd == 0xA0 || cmd == 0xB0 || cmd == 0xE0)
        {
            if (!MIDISLIC_trk_get_byte(tr, &t0) || !MIDISLIC_trk_get_byte(tr, &t1))
            {
                tr->active = 0;
                tr->has_event = 0;
                return 0;
            }
            if (cmd == 0x80 || cmd == 0xA0) continue;
            tr->has_event = 1;
            tr->ev_status = status;
            tr->ev_d1 = t0;
            tr->ev_d2 = t1;
            tr->ev_dur = 0;
            return 1;
        }
        else if (cmd == 0xC0 || cmd == 0xD0)
        {
            if (!MIDISLIC_trk_get_byte(tr, &t0))
            {
                tr->active = 0;
                tr->has_event = 0;
                return 0;
            }
            if (cmd == 0xD0) continue;
            tr->has_event = 1;
            tr->ev_status = status;
            tr->ev_d1 = t0;
            tr->ev_d2 = 0;
            tr->ev_dur = 0;
            return 1;
        }
        else if (status == 0xF0 || status == 0xF7)
        {
            mlen = MIDISLIC_trk_read_vlq(tr, &ok);
            if (!ok)
            {
                tr->active = 0;
                tr->has_event = 0;
                return 0;
            }
            MIDISLIC_trk_skip_bytes(tr, mlen);
            continue;
        }
    }
    tr->has_event = 0;
    return 0;
}

//----------------------------------------------------------------------------
// Channel state tracking & restoration across slices
//----------------------------------------------------------------------------
static void MIDISLIC_state_reset(void)
{
	memset(m_ch_prog, 0xFF, MIDISLIC_MAX_CHANNELS);
	memset(m_ch_bank, 0, MIDISLIC_MAX_CHANNELS);
	memset(m_ch_vol, 0xFF, MIDISLIC_MAX_CHANNELS);
	memset(m_ch_pan, 0xFF, MIDISLIC_MAX_CHANNELS);
	memset(m_ch_pitch_lsb, 0x00, MIDISLIC_MAX_CHANNELS);
	memset(m_ch_pitch_msb, 0x40, MIDISLIC_MAX_CHANNELS);
	memset(m_ch_has_pitch, 0, MIDISLIC_MAX_CHANNELS);
	memset(m_ch_rpn_msb, 0x7F, MIDISLIC_MAX_CHANNELS);
	memset(m_ch_rpn_lsb, 0x7F, MIDISLIC_MAX_CHANNELS);
	memset(m_ch_rpn_range, 2, MIDISLIC_MAX_CHANNELS);
	memset(m_ch_has_rpn, 0, MIDISLIC_MAX_CHANNELS);
	memset(m_active_note_end, 0, sizeof(m_active_note_end));
	memset(m_active_note_vel, 0, sizeof(m_active_note_vel));
	m_xmi_off_cnt = 0;
}

static void MIDISLIC_rewind_all_tracks(void)
{
	int i;
	struct SlicTrackStream *tr;

	for (i = 0; i < m_num_tracks; i++)
	{
		tr = m_tracks + i;
		tr->active = 1;
		tr->file_pos = tr->start_off;
		tr->buf_pos = 0;
		tr->buf_len = 0;
		tr->running_status = 0;
		tr->cur_tick = 0;
		tr->has_event = 0;
		tr->ev_order = (unsigned int)i;
		if (m_is_xmi)
		{
			MIDISLIC_xmi_fetch_next(tr);
		}
		else
		{
			MIDISLIC_midi_fetch_next(tr);
		}
	}

	if (m_division == 0)
	{
		m_division = MIDISLIC_DEFAULT_TIMEBASE;
	}

	if (m_is_smpte)
	{
		m_tick_num = 120UL * m_smpte_den;
		m_tick_den = (m_smpte_num > 0) ? m_smpte_num : 1UL;
	}
	else
	{
		// Default 120 BPM = 500,000 us per quarter note
		m_tick_num = (3UL * 500000UL) / 10UL;
		m_tick_den = 2500UL * (unsigned long)m_division;
	}

	m_dda_sum = 0;
	m_xmi_interval = 0;
	m_last_raw_tick = 0;
	m_prev_xmi_tick = 0;
	m_slice_index = 0;
	MIDISLIC_state_reset();
}

static void MIDISLIC_emit_channel_state(unsigned char **cur, unsigned short *timbres, size_t *timb_cnt)
{
	int c, n, found, has_state, retriggered;
	unsigned long rem_dur;
	unsigned short tb;
	size_t k;

	*timb_cnt = 0;

	for (c = 0; c < MIDISLIC_MAX_CHANNELS; c++)
	{
		has_state = (*(m_ch_prog + c) != 0xFF) ||
			(*(m_ch_vol + c) != 0xFF) ||
			(*(m_ch_pan + c) != 0xFF) ||
			(*(m_ch_has_pitch + c) != 0) ||
			(*(m_ch_has_rpn + c) != 0);

		if (!has_state)
		{
			continue;
		}

		if (*(m_ch_prog + c) != 0xFF)
		{
			tb = (unsigned short)(*(m_ch_prog + c) |
				(((unsigned short)(*(m_ch_bank + c))) << 8));
			found = 0;

			if (*(m_ch_bank + c) != 0)
			{
				*(*cur)++ = 0x00;
				*(*cur)++ = (unsigned char)(0xB0 | c);
				*(*cur)++ = 0x00;
				*(*cur)++ = *(m_ch_bank + c);
			}

			*(*cur)++ = 0x00;
			*(*cur)++ = (unsigned char)(0xC0 | c);
			*(*cur)++ = *(m_ch_prog + c);

			for (k = 0; k < *timb_cnt; k++)
			{
				if (*(timbres + k) == tb)
				{
					found = 1;
					break;
				}
			}
			if (!found && *timb_cnt < MIDISLIC_MAX_TIMB_CNT)
			{
				*(timbres + (*timb_cnt)++) = tb;
			}
		}

		if (*(m_ch_vol + c) != 0xFF)
		{
			*(*cur)++ = 0x00;
			*(*cur)++ = (unsigned char)(0xB0 | c);
			*(*cur)++ = 0x07;
			*(*cur)++ = *(m_ch_vol + c);
		}

		if (*(m_ch_pan + c) != 0xFF)
		{
			*(*cur)++ = 0x00;
			*(*cur)++ = (unsigned char)(0xB0 | c);
			*(*cur)++ = 0x0A;
			*(*cur)++ = *(m_ch_pan + c);
		}

		// Restore Pitch Bend range (RPN 0, 0) so MUSIC_PlaySong doesn't reset it to +-2 semitones
		if (*(m_ch_has_rpn + c))
		{
			*(*cur)++ = 0x00;
			*(*cur)++ = (unsigned char)(0xB0 | c);
			*(*cur)++ = 101;
			*(*cur)++ = 0x00;

			*(*cur)++ = 0x00;
			*(*cur)++ = (unsigned char)(0xB0 | c);
			*(*cur)++ = 100;
			*(*cur)++ = 0x00;

			*(*cur)++ = 0x00;
			*(*cur)++ = (unsigned char)(0xB0 | c);
			*(*cur)++ = 6;
			*(*cur)++ = *(m_ch_rpn_range + c);
		}

		// Always emit explicit Pitch Bend (saved or center 0x4000)
		*(*cur)++ = 0x00;
		*(*cur)++ = (unsigned char)(0xE0 | c);
		if (*(m_ch_has_pitch + c))
		{
			*(*cur)++ = *(m_ch_pitch_lsb + c);
			*(*cur)++ = *(m_ch_pitch_msb + c);
		}
		else
		{
			*(*cur)++ = 0x00;
			*(*cur)++ = 0x40;
		}
	}

	// Note chopping: re-trigger sustained notes that crossed the previous slice boundary
	retriggered = 0;
	for (c = 0; c < MIDISLIC_MAX_CHANNELS && retriggered < 16; c++)
	{
		if (c == 9)
		{
			continue;
		}
		for (n = 0; n < MIDISLIC_MAX_NOTES && retriggered < 16; n++)
		{
			if (m_active_note_end[c][n] > 0 && m_active_note_vel[c][n] > 0)
			{
				if (rem_dur > 0x0FFFFFFFUL)
				{
					rem_dur = 0x0FFFFFFFUL;
				}
				*(*cur)++ = 0x00;
				*(*cur)++ = (unsigned char)(0x90 | c);
				*(*cur)++ = (unsigned char)n;
				*(*cur)++ = m_active_note_vel[c][n];
				MIDISLIC_write_vlq(cur, rem_dur);
				retriggered++;
			}
		}
	}
}

//----------------------------------------------------------------------------
// Public Streaming API
//----------------------------------------------------------------------------
unsigned long MIDISLIC_GetLastSliceTicks(void)
{
    return m_last_slice_ticks;
}

void MIDISLIC_CloseStream(void)
{
	if (m_file_handle != -1)
	{
		kclose_stream(m_file_handle);
		m_file_handle = -1;
	}
	m_stream_active = 0;
	m_num_tracks = 0;
	m_slice_index = 0;
	m_file_size = 0;
}

int MIDISLIC_IsStreamActive(void)
{
	return m_stream_active;
}

int MIDISLIC_OpenStreamFile(const char *filename)
{
	int i, fps_val;
	unsigned int clen, hdr_len;
	long pos;
	unsigned long fps_u;
	unsigned short ntrks, div_word;
	unsigned char hdr[16], hi, tpf;
	struct SlicTrackStream *tr;

	MIDISLIC_CloseStream();

	if (!filename)
	{
		return 0;
	}

	// Copy filename safely
	for (i = 0; i < 63 && *(filename + i) != 0; i++)
	{
		*(m_fname_buf + i) = *(filename + i);
	}

	*(m_fname_buf + i) = 0;

	// 1. Try group archive stream first, then external file via kopen4load
	m_file_handle = kopen4group_stream(m_fname_buf);
	if (m_file_handle != -1)
	{
		m_file_size = kfilelength_stream(m_file_handle);
	}
	else
	{
		m_file_handle = kopen4load(m_fname_buf, 0);
		if (m_file_handle == -1)
		{
			return 0;
		}
		m_file_size = kfilelength(m_file_handle);
	}

	if (m_file_size < 14)
	{
		MIDISLIC_CloseStream();
		return 0;
	}

	// Bind static 1KB buffers to each track slot
	for (i = 0; i < MIDISLIC_MAX_TRACKS; i++)
	{
		tr = m_tracks + i;
		memset(tr, 0, sizeof(struct SlicTrackStream));
		tr->buf_ptr = m_trk_buf_pool + ((size_t)i * MIDISLIC_TRK_BUF_SIZE);
	}

	klseek_stream(m_file_handle, 0L, SEEK_SET);

	if (kread_stream(m_file_handle, hdr, 14) < 14)
	{
		MIDISLIC_CloseStream();
		return 0;
	}

	m_is_xmi = 0;
	m_is_smpte = 0;
	m_num_tracks = 0;
	if (memcmp(hdr, "FORM", 4) == 0)
	{
		// Scan IFF chunks for "EVNT"
		m_is_xmi = 1;
		m_division = MIDISLIC_DEFAULT_TIMEBASE;
		pos = 0;

		while (pos + 8 <= m_file_size)
		{
			klseek_stream(m_file_handle, pos, SEEK_SET);
			if (kread_stream(m_file_handle, hdr, 8) < 8)
			{
				break;
			}
			if (memcmp(hdr, "FORM", 4) == 0 || memcmp(hdr, "CAT ", 4) == 0)
			{
				pos += 12;
			}
			else if (memcmp(hdr, "EVNT", 4) == 0)
			{
				clen = MIDISLIC_read_be32(hdr + 4);
				tr = m_tracks + 0;
				tr->start_off = pos + 8;
				tr->end_off = pos + 8 + (long)clen;
				if (tr->end_off > m_file_size)
				{
					tr->end_off = m_file_size;
				}
				m_num_tracks = 1;
				break;
			}
			else
			{
				clen = MIDISLIC_read_be32(hdr + 4);
				pos += 8 + (long)clen + (long)(clen & 1);
			}
		}
	}
	else if (memcmp(hdr, "MThd", 4) == 0)
	{
		hdr_len = MIDISLIC_read_be32(hdr + 4);
		ntrks = MIDISLIC_read_be16(hdr + 10);
		div_word = MIDISLIC_read_be16(hdr + 12);
		if (div_word & 0x8000)
		{
			hi = (unsigned char)((div_word >> 8) & 0xFF);
			fps_val = (hi >= 0xE0) ? -(signed char)hi : (hi & 0x7F);
			tpf = (unsigned char)(div_word & 0xFF);
			if (tpf == 0) tpf = 40;
			m_is_smpte = 1;
			if (fps_val == 29)
			{
				// 29.97 fps = 30000 / 1001
				m_smpte_num = 30000UL * (unsigned long)tpf;
				m_smpte_den = 1001UL;
			}
			else
			{
				fps_u = (unsigned long)((fps_val <= 0 || fps_val > 60) ? 25 : fps_val);
				m_smpte_num = fps_u * (unsigned long)tpf;
				m_smpte_den = 1UL;
			}
		}
		else
		{
			m_division = (div_word > 0) ? div_word : MIDISLIC_DEFAULT_TIMEBASE;
		}
		pos = 8L + (long)hdr_len;

		while (pos + 8 <= m_file_size && m_num_tracks < MIDISLIC_MAX_TRACKS && m_num_tracks < (int)ntrks)
		{
			klseek_stream(m_file_handle, pos, SEEK_SET);

			if (kread_stream(m_file_handle, hdr, 8) < 8)
			{
				break;
			}

			clen = MIDISLIC_read_be32(hdr + 4);

			if (memcmp(hdr, "MTrk", 4) == 0)
			{
				tr = m_tracks + m_num_tracks;
				tr->start_off = pos + 8;
				tr->end_off = pos + 8 + (long)clen;
				if (tr->end_off > m_file_size)
				{
					tr->end_off = m_file_size;
				}

				m_num_tracks++;
			}

			pos += 8L + (long)clen;

		}
	}

	if (m_num_tracks == 0)
	{
		MIDISLIC_CloseStream();
		return 0;
	}

	MIDISLIC_rewind_all_tracks();
	m_stream_active = 1;

	return 1;
}

unsigned int MIDISLIC_RenderNextSlice(unsigned char *out_music_ptr, unsigned int slice_max_bytes, int loop_song)
{
	int best_idx, pval, found, ch_i, c_i, n_i;
	unsigned int best_order, cur_evnt_len, next_timb_cnt;
	unsigned int ev_bytes, projected_evnt, projected_pad;
	unsigned int projected_total, IEVNT, evnt_pad, ITIMB;
	unsigned int IFORM_inner, ICAT, IFORM_outer;
	unsigned long best_tick, raw_dur, event_delta, steps;
	unsigned long cur_xmi_tick, delay, xmi_dur;
	unsigned long slice_start_xmi_tick, chunk, end_rel;
	unsigned short tb, elapsed_s;
	unsigned char st, cmd, ch, d1, d2;
	unsigned char *evnt_start, *ecur, *hdr_ptr;
	size_t slice_timb_cnt = 0, i, k, off;
	struct SlicTrackStream *tr;

	if (!m_stream_active || !out_music_ptr || slice_max_bytes < 512)
	{
		return 0;
	}

	// Check if any track still has events
	best_idx = -1;
	for (i = 0; i < (size_t)m_num_tracks; i++)
	{
		if ((m_tracks + i)->has_event)
		{
			best_idx = (int)i;
			break;
		}
	}
	if (best_idx == -1)
	{
		if (!loop_song)
		{
			MIDISLIC_CloseStream();
			return 0;
		}
		MIDISLIC_rewind_all_tracks();
	}

	// Reserve max XMI header space (58 + 256 * 2 = 570 bytes) directly in out_music_ptr
	evnt_start = out_music_ptr + 58 + (MIDISLIC_MAX_TIMB_CNT * 2);
	ecur = evnt_start;

	slice_start_xmi_tick = m_prev_xmi_tick;
	*m_slice_timbres = 0x0000;
	slice_timb_cnt = 1;

	memset(m_last_cc_val, -1, MIDISLIC_MAX_CHANNELS * MIDISLIC_MAX_NOTES * sizeof(short));

	for (i = 0; i < MIDISLIC_MAX_CHANNELS; i++)
	{
		*(m_last_pitch_val + i) = -1;
	}

	while (1)
	{
		// K-way merge: pick track with earliest cur_tick
		best_idx = -1;
		best_tick = 0xFFFFFFFFUL;
		best_order = 0xFFFFU;
		for (i = 0; i < (size_t)m_num_tracks; i++)
		{
			tr = m_tracks + i;
			if (tr->has_event)
			{
				if (tr->cur_tick < best_tick ||
					(tr->cur_tick == best_tick && tr->ev_order < best_order))
				{
					best_tick = tr->cur_tick;
					best_order = tr->ev_order;
					best_idx = (int)i;
				}
			}
		}

		if (best_idx == -1)
		{
			break;
		}
		tr = m_tracks + best_idx;
		st = tr->ev_status;
		d1 = tr->ev_d1;
		d2 = tr->ev_d2;
		raw_dur = tr->ev_dur;
		cmd = st & 0xF0;
		ch = st & 0x0F;

		// Quantize raw tick to 120 Hz XMI tick (exact integer Bresenham accumulator)
		if (m_is_xmi)
		{
			cur_xmi_tick = best_tick;
		}
		else
		{
			if (best_tick > m_last_raw_tick && m_tick_den > 0)
			{
				event_delta = best_tick - m_last_raw_tick;
				m_last_raw_tick = best_tick;
				while (event_delta > 0)
				{
					chunk = (event_delta > 4096UL) ? 4096UL : event_delta;
					m_dda_sum += chunk * m_tick_num;
					if (m_dda_sum >= m_tick_den)
					{
						steps = m_dda_sum / m_tick_den;
						m_xmi_interval += steps;
						m_dda_sum -= steps * m_tick_den;
					}
					event_delta -= chunk;
				}
			}
			cur_xmi_tick = m_xmi_interval;
		}

		// Tempo meta event in MIDI stream
		if (st == 0xFF && d1 == 0x51)
		{
			if (!m_is_smpte && !m_is_xmi && m_division > 0 && raw_dur > 0)
			{
				m_tick_num = (3UL * raw_dur) / 10UL;
				m_tick_den = 2500UL * (unsigned long)m_division;
			}
			MIDISLIC_midi_fetch_next(tr);
			continue;
		}

		// Handle real Note-Off (0x80 or 0x90 with velocity 0)
		if (cmd == 0x80 || (cmd == 0x90 && d2 == 0))
		{
			if (m_active_note_end[ch][d1] > 0)
			{
				if (m_slice_index > 0 && ecur == evnt_start)
				{
                                        MIDISLIC_emit_channel_state(&ecur, m_slice_timbres, &slice_timb_cnt);     //line #1358
					m_prev_xmi_tick = cur_xmi_tick;
				}

				delay = (cur_xmi_tick >= m_prev_xmi_tick) ? (cur_xmi_tick - m_prev_xmi_tick) : 0;
				cur_evnt_len = (unsigned int)(ecur - evnt_start);
				next_timb_cnt = (unsigned int)slice_timb_cnt;
				ev_bytes = MIDISLIC_xmi_delay_len(delay) + 3;

				if (next_timb_cnt == 0)
				{
					next_timb_cnt = 1;
				}
				projected_evnt = cur_evnt_len + ev_bytes + 51;
				projected_pad = (projected_evnt % 2 != 0) ? 1 : 0;
				projected_total = 58 + (next_timb_cnt * 2) + projected_evnt + projected_pad;

				if (projected_total > slice_max_bytes && cur_evnt_len > 0)
				{
					break;
				}

				MIDISLIC_write_xmi_delay(&ecur, delay);
				m_prev_xmi_tick = cur_xmi_tick;
				*ecur++ = (unsigned char)(0x80 | ch);
				*ecur++ = d1;
				*ecur++ = 0x00;
			}

			m_active_note_end[ch][d1] = 0;
			m_active_note_vel[ch][d1] = 0;
			if (m_is_xmi) MIDISLIC_xmi_fetch_next(tr);
			else MIDISLIC_midi_fetch_next(tr);
			continue;
		}

		// Deduplicate identical consecutive CCs
		if (cmd == 0xB0)
		{
			off = (size_t)ch * MIDISLIC_MAX_NOTES + (size_t)d1;
			if (*(m_last_cc_val + off) == (short)d2)
			{
				if (m_is_xmi) MIDISLIC_xmi_fetch_next(tr);
				else MIDISLIC_midi_fetch_next(tr);
				continue;
			}
			*(m_last_cc_val + off) = (short)d2;
		}

		// Deduplicate identical consecutive Pitch Bend
		if (cmd == 0xE0)
		{
			pval = ((int)d2 << 7) | (int)d1;
			if (*(m_last_pitch_val + ch) == pval)
			{
				if (m_is_xmi) MIDISLIC_xmi_fetch_next(tr);
				else MIDISLIC_midi_fetch_next(tr);
				continue;
			}
			*(m_last_pitch_val + ch) = pval;
		}

		// Emit channel setup & re-trigger chopped sustained notes at start of slice > 0
		if (m_slice_index > 0 && ecur == evnt_start)
		{
                        MIDISLIC_emit_channel_state(&ecur, m_slice_timbres, &slice_timb_cnt);  //line #1423
			m_prev_xmi_tick = cur_xmi_tick;
		}

		delay = (cur_xmi_tick >= m_prev_xmi_tick) ? (cur_xmi_tick - m_prev_xmi_tick) : 0;
		cur_evnt_len = (unsigned int)(ecur - evnt_start);
		next_timb_cnt = (unsigned int)slice_timb_cnt;
		ev_bytes = MIDISLIC_xmi_delay_len(delay);

		xmi_dur = raw_dur;

		if (cmd == 0x90)
		{
			if (xmi_dur == 0) xmi_dur = 12UL;
			ev_bytes += 3 + MIDISLIC_vlq_len(xmi_dur);
		}
		else if (cmd == 0xB0 || cmd == 0xE0)
		{
			ev_bytes += 3;
		}
		else if (cmd == 0xC0)
		{
			tb = (unsigned short)(d1 | (((unsigned short)(*(m_ch_bank + ch))) << 8));
			found = 0;
			ev_bytes += 2;
			for (k = 0; k < slice_timb_cnt; k++)
			{
				if (*(m_slice_timbres + k) == tb)
				{
					found = 1;
					break;
				}
			}
			if (!found && next_timb_cnt < MIDISLIC_MAX_TIMB_CNT)
			{
				next_timb_cnt++;
			}
		}

		if (next_timb_cnt == 0)
		{
			next_timb_cnt = 1;
		}
		// Reserve 48 bytes for trailing All Notes Off (16 * 3) + 3 bytes for End of Track
		projected_evnt = cur_evnt_len + ev_bytes + 51;
		projected_pad = (projected_evnt % 2 != 0) ? 1 : 0;

		projected_total = 58 + (next_timb_cnt * 2) + projected_evnt + projected_pad;
		// Slice buffer is full -> stop and keep current track event for next slice!
		if (projected_total > slice_max_bytes && cur_evnt_len > 0)
		{
			break;
		}

		// Update persistent channel state
		if (cmd == 0xB0)
		{
			if (d1 == 0x00) *(m_ch_bank + ch) = d2;
			else if (d1 == 0x07) *(m_ch_vol + ch) = d2;
			else if (d1 == 0x0A) *(m_ch_pan + ch) = d2;
			else if (d1 == 101) *(m_ch_rpn_msb + ch) = d2;
			else if (d1 == 100) *(m_ch_rpn_lsb + ch) = d2;
			else if (d1 == 6 && *(m_ch_rpn_msb + ch) == 0 && *(m_ch_rpn_lsb + ch) == 0)
			{
				*(m_ch_rpn_range + ch) = d2;
				*(m_ch_has_rpn + ch) = 1;
			}
		}
		else if (cmd == 0xC0)
		{
			memset(m_active_note_end[ch], 0, sizeof(m_active_note_end[ch]));
			memset(m_active_note_vel[ch], 0, sizeof(m_active_note_vel[ch]));
			tb = (unsigned short)(d1 | (((unsigned short)(*(m_ch_bank + ch))) << 8));
			found = 0;
			*(m_ch_prog + ch) = d1;
			for (k = 0; k < slice_timb_cnt; k++)
			{
				if (*(m_slice_timbres + k) == tb)
				{
					found = 1;
					break;
				}
			}
			if (!found && slice_timb_cnt < MIDISLIC_MAX_TIMB_CNT)
			{
				*(m_slice_timbres + slice_timb_cnt++) = tb;
			}
		}
		else if (cmd == 0xE0)
		{
			*(m_ch_pitch_lsb + ch) = d1;
			*(m_ch_pitch_msb + ch) = d2;
			*(m_ch_has_pitch + ch) = 1;
		}
		MIDISLIC_write_xmi_delay(&ecur, delay);
		m_prev_xmi_tick = cur_xmi_tick;
		if (cmd == 0x90)
		{
			end_rel = (cur_xmi_tick - slice_start_xmi_tick) + xmi_dur;
			m_active_note_end[ch][d1] = (end_rel > 65000UL) ? 65000U : (unsigned short)end_rel;
			m_active_note_vel[ch][d1] = d2;
			*ecur++ = st;
			*ecur++ = d1;
			*ecur++ = d2;
			MIDISLIC_write_vlq(&ecur, xmi_dur);
		}
		else if (cmd == 0xB0 || cmd == 0xE0)
		{
			*ecur++ = st;
			*ecur++ = d1;
			*ecur++ = d2;
		}
		else if (cmd == 0xC0)
		{
			*ecur++ = st;
			*ecur++ = d1;
		}
		// Advance selected track to its next event
		if (m_is_xmi)
		{
			MIDISLIC_xmi_fetch_next(tr);
		}
		else
		{
			MIDISLIC_midi_fetch_next(tr);
		}
	}

	// Flush active voices before end of slice to prevent Apogee XMI voice pool overflow
	for (ch_i = 0; ch_i < MIDISLIC_MAX_CHANNELS; ch_i++)
	{
		if (*(m_ch_prog + ch_i) != 0xFF || ch_i == 9)
		{
			*ecur++ = (unsigned char)(0xB0 | ch_i);
			*ecur++ = 0x7B; // All Notes Off
			*ecur++ = 0x00;
		}
	}

	// End of track marker
	*ecur++ = 0xFF;
	*ecur++ = 0x2F;
	*ecur++ = 0x00;
	IEVNT = (unsigned int)(ecur - evnt_start);
	evnt_pad = (IEVNT % 2 != 0) ? 1 : 0;

	if (slice_timb_cnt == 0)
	{
		*m_local_timbres = 0x0000;
		slice_timb_cnt = 1;
	}
	else
	{
		if (slice_timb_cnt > MIDISLIC_MAX_TIMB_CNT) slice_timb_cnt = MIDISLIC_MAX_TIMB_CNT;
		memcpy(m_local_timbres, m_slice_timbres, slice_timb_cnt * sizeof(unsigned short));
	}

	ITIMB = (unsigned int)(slice_timb_cnt * 2);
	IFORM_inner = 4 + 8 + ITIMB + 8 + IEVNT + evnt_pad;
	ICAT = 4 + 8 + IFORM_inner;
	IFORM_outer = 4 + 8 + 2;
	hdr_ptr = out_music_ptr;
	memcpy(hdr_ptr, "FORM", 4); hdr_ptr += 4;
	MIDISLIC_write_be32_mem(hdr_ptr, IFORM_outer); hdr_ptr += 4;
	memcpy(hdr_ptr, "XDIRINFO", 8); hdr_ptr += 8;
	MIDISLIC_write_be32_mem(hdr_ptr, 2); hdr_ptr += 4;
	*hdr_ptr++ = 1;
	*hdr_ptr++ = 0;
	memcpy(hdr_ptr, "CAT ", 4); hdr_ptr += 4;
	MIDISLIC_write_be32_mem(hdr_ptr, ICAT); hdr_ptr += 4;
	memcpy(hdr_ptr, "XMIDFORM", 8); hdr_ptr += 8;
	MIDISLIC_write_be32_mem(hdr_ptr, IFORM_inner); hdr_ptr += 4;
	memcpy(hdr_ptr, "XMID", 4); hdr_ptr += 4;
	memcpy(hdr_ptr, "TIMB", 4); hdr_ptr += 4;
	MIDISLIC_write_be32_mem(hdr_ptr, ITIMB); hdr_ptr += 4;

	for (i = 0; i < slice_timb_cnt; i++)
	{
		*hdr_ptr++ = (unsigned char)(*(m_local_timbres + i) & 0xFF);
		*hdr_ptr++ = (unsigned char)((*(m_local_timbres + i) >> 8) & 0xFF);
	}

	memcpy(hdr_ptr, "EVNT", 4); hdr_ptr += 4;
	MIDISLIC_write_be32_mem(hdr_ptr, IEVNT); hdr_ptr += 4;
	if (evnt_pad)
	{
		*ecur++ = 0;
	}
	// Shift EVNT chunk backward right after the EVNT size header (zero extra RAM!)
	memmove(hdr_ptr, evnt_start, IEVNT + evnt_pad);
	hdr_ptr += IEVNT + evnt_pad;

	m_last_slice_ticks =
	(m_prev_xmi_tick >= slice_start_xmi_tick) ? (m_prev_xmi_tick - slice_start_xmi_tick) : 0;

	// Decrement active note remaining durations by elapsed slice ticks
	elapsed_s = (m_last_slice_ticks > 65000UL) ? 65000U : (unsigned short)m_last_slice_ticks;

	for (c_i = 0; c_i < MIDISLIC_MAX_CHANNELS; c_i++)
	{
		for (n_i = 0; n_i < MIDISLIC_MAX_NOTES; n_i++)
		{
			if (m_active_note_end[c_i][n_i] > elapsed_s)
			{
				m_active_note_end[c_i][n_i] -= elapsed_s;
			}
			else
			{
				m_active_note_end[c_i][n_i] = 0;
				m_active_note_vel[c_i][n_i] = 0;
			}
		}
	}

	m_slice_index++;

	return (unsigned int)(hdr_ptr - out_music_ptr);
}


