//============================================================================
// MIDSLIC.H
// Ultra Low-RAM Disk/GRP Streaming MIDI & XMI Slicer for Duke3D (Watcom C)
// Uses ~18KB static BSS and renders directly into 72,000-byte MusicPtr.
//============================================================================

#ifndef MIDMIDISLIC_H
#define MIDMIDISLIC_H

#define MAX_MUSIC_BUF_BYTES   72000L   // Original Duke3D MusicPtr size
#define HALF_MUSIC_BUF_BYTES  36000L   // Ping-pong half buffer size (2 x 36 KB = 72 KB)
#define SLICE_TARGET_BYTES    32000U   // 32 KB per slice (fits cleanly inside 36 KB half)

// Opens a MIDI ("MThd") or XMI ("FORM") file from GRP or disk for streaming.
// Returns 1 on success, 0 on error.
int MIDISLIC_OpenStreamFile(const char *filename);

// Renders the next <= slice_max_bytes XMI chunk directly into out_music_ptr.
// Automatically seeks back to track starts when EOF is reached (if loop != 0).
// Returns byte size of rendered XMI slice, or 0 on error/finish.
unsigned int MIDISLIC_RenderNextSlice(unsigned char *out_music_ptr,
                                  unsigned int slice_max_bytes,
                                  int loop_song);

// Returns 1 if a stream is currently open, 0 otherwise.
int MIDISLIC_IsStreamActive(void);

// Closes file handle and resets static stream state.
void MIDISLIC_CloseStream(void);

unsigned long MIDISLIC_GetLastSliceTicks(void);

#endif // MIDMIDISLIC_H
