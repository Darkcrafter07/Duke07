//============================================================================
// MIDSLIC.H
// Ultra Low-RAM Disk/GRP Streaming MIDI & XMI Slicer for Duke3D (Watcom C)
// Uses ~18KB static BSS and renders directly into 72,000-byte MusicPtr.
//============================================================================

#ifndef MIDMIDISLIC_H
#define MIDMIDISLIC_H

// RAM tiers (based on Z_AvailHeap() / totalmemory in bytes)
#define MUS_HEAP_12MB         3500000L
#define MUS_HEAP_16MB         7500000L
#define MUS_HEAP_24MB         15000000L
#define MUS_HEAP_32MB         23000000L

// Total MusicPtr buffer sizes per RAM tier
#define MUS_BUF_8MB           72000L     // 2 x 36 KB (32 KB slices)
#define MUS_BUF_12MB          144000L    // 2 x 72 KB (60 KB slices)
#define MUS_BUF_16MB          640000L    // 2 x 320 KB (60 KB slices, <640KB plays whole)
#define MUS_BUF_24MB          860000L    // 2 x 430 KB (60 KB slices, <860KB plays whole)
#define MUS_BUF_32MB          1048576L   // 2 x 512 KB (60 KB slices, <1MB plays whole)

// Safe XMI slice limit (fits inside unsigned int / 64 KB XMI chunk logic)
#define SLICE_LIMIT_8MB       32000U
#define SLICE_LIMIT_HIGHRAM   60000U

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
