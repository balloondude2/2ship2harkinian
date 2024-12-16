#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "Enhancements/Enhancements.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
OSMesgQueue* PadMgr_AcquireSerialEventQueue(void);
void PadMgr_ReleaseSerialEventQueue(OSMesgQueue* serialEventQueue);
PlayState* gPlayState;
#include "z64voice.h"
#include "macros.h"
void func_801A4EB8(void);
}

#define CVAR_NAME "gEnhancements.Restorations.VoiceRecognitionUnit"
#define CVAR CVarGetInteger(CVAR_NAME, 0)
#define CVAR_NAME_WORD "gEnhancements.Restorations.VoiceRecognitionUnitWord"
#define CVAR_WORD CVarGetInteger(CVAR_NAME_WORD, VOICE_WORD_NONE)

// Implementation of VRU functions
int32_t osVoiceInit(OSMesgQueue* mq, OSVoiceHandle* hd, int channel) {
    return 0;
}

int32_t osVoiceSetWord(OSVoiceHandle* hd, u8* word) {
    return 0;
}

int32_t osVoiceCheckWord(u8* word) {
    return 0;
}

int32_t osVoiceStartReadData(OSVoiceHandle* hd) {
    return 0;
}

int32_t osVoiceStopReadData(OSVoiceHandle* hd) {
    return 0;
}

int32_t osVoiceGetReadData(OSVoiceHandle* hd, OSVoiceData* result) {

    result->answer[0] = VOICE_WORD_ID_NONE;

    if (gPlayState == nullptr or !CVAR) {
        return 0;
    }

    if (CHECK_BTN_ANY(gPlayState->state.input[0].cur.button, BTN_L)) {
        // These values arbitrarily chosen to meet thresholds required in voice_internal.c
        result->warning = 0;
        result->answerNum = 1;
        result->voiceLevel = 1000;
        result->voiceRelLevel = 2500;
        result->distance[0] = 10;

        result->answer[0] = CVAR_WORD;
    }

    return 0;
}

int32_t osVoiceClearDictionary(OSVoiceHandle* hd, u8 numWords) {
    return 0;
}

int32_t osVoiceMaskDictionary(OSVoiceHandle* hd, u8* maskPattern, int size) {
    return 0;
}

int32_t osVoiceControlGain(OSVoiceHandle* hd, s32 analog, s32 digital) {
    return 0;
}

void RegisterVoiceRecognitionUnit() {
    if (CVAR) {
        // Start the VRU loop
        func_801A4EB8();
    }
}

static RegisterShipInitFunc initFunc(RegisterVoiceRecognitionUnit, { CVAR_NAME });
