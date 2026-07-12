// ============================================================
// CardputerGroovebox - sampler.h
// SD sample loading into a fixed RAM pool + sample playback voice
// ============================================================
#pragma once
#include "config.h"
#include <FS.h>

struct SampleInfo {
    char     name[SAMPLE_NAME_LEN];  // filename inside DIR_SAMPLES
    uint32_t offset;                 // start index in pool (int16 units)
    uint32_t length;                 // frames
    uint32_t rate;                   // native sample rate
    bool     used;
};

extern int16_t*   g_samplePool;      // allocated once at boot
extern uint32_t   g_poolUsed;        // int16 units used
extern uint32_t   g_poolCapacity;    // int16 units total
extern SampleInfo g_samples[MAX_SAMPLES];
extern uint8_t    g_numSamples;

bool samplerInit();                       // allocate pool, ensure SD dirs
void samplerClearAll();                   // wipe pool + registry
int  samplerLoad(const char* filename);   // returns slot index or -1
int  samplerFindByName(const char* filename);

// Decode an open WAV file to mono int16. Returns false on unsupported format.
// Used by both the sampler and the wavetable loader.
bool wavDecodeToMono16(File& f, int16_t* dst, uint32_t maxFrames,
                       uint32_t& outFrames, uint32_t& outRate);

// ---------- Playback voice ----------
struct SampleVoice {
    const int16_t* data;
    uint32_t len;
    float    pos;
    float    inc;      // rate/engine-rate * pitch
    float    gain;
    bool     active;

    void init() { data = nullptr; len = 0; pos = 0; inc = 1; gain = 1; active = false; }

    void trigger(int slot, float pitch, float volume) {
        if (slot < 0 || slot >= g_numSamples || !g_samples[slot].used) { active = false; return; }
        data   = g_samplePool + g_samples[slot].offset;
        len    = g_samples[slot].length;
        pos    = 0;
        inc    = ((float)g_samples[slot].rate * INV_SAMPLE_RATE) * pitch;
        gain   = volume;
        active = true;
    }

    void stop() { active = false; }

    inline float render() {
        if (!active) return 0.0f;
        uint32_t i = (uint32_t)pos;
        if (i + 1 >= len) { active = false; return 0.0f; }
        float frac = pos - (float)i;
        float s = ((float)data[i] * (1.0f - frac) + (float)data[i + 1] * frac) * (1.0f / 32768.0f);
        pos += inc;
        return s * gain;
    }
};

// One-shot preview player (SAMPLE page), mixed by the audio engine.
extern SampleVoice g_previewVoice;
