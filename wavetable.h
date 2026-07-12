// ============================================================
// CardputerGroovebox - wavetable.h
// 256-sample single-cycle wavetables: 8 built-in + up to 8 user
// tables loaded from SD (/groovebox/wavetables/*.wav)
// ============================================================
#pragma once
#include "config.h"

extern int16_t g_wavetables[NUM_WT_TOTAL][WT_SIZE];
extern char    g_wtNames[NUM_WT_TOTAL][WT_NAME_LEN];
extern uint8_t g_numWavetables;   // built-ins + loaded user tables

void wavetableInitBuiltins();
// Scans DIR_WAVETABLES and loads up to MAX_USER_WT single-cycle wavs.
// Any-length mono/stereo 16-bit PCM is resampled to WT_SIZE points.
void wavetableLoadUserFromSD();

// Fast lookup with linear interpolation, phase in [0,1)
static inline float wavetableRead(uint8_t table, float phase) {
    float fidx = phase * (float)WT_SIZE;
    int   i    = (int)fidx;
    float frac = fidx - (float)i;
    int   i2   = (i + 1) & (WT_SIZE - 1);
    i &= (WT_SIZE - 1);
    const int16_t* t = g_wavetables[table];
    return ((float)t[i] + ((float)t[i2] - (float)t[i]) * frac) * (1.0f / 32768.0f);
}
