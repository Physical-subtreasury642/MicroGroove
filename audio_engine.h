// ============================================================
// CardputerGroovebox - audio_engine.h
// ============================================================
#pragma once
#include "config.h"

extern float g_scopeBuf[SCREEN_W];
extern volatile int g_scopeIdx;

void audioEngineStart();   // creates the render task on core 0
