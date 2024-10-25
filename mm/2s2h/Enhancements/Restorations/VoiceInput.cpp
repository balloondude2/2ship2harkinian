
#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {

#include "variables.h"
extern s32 sVoiceInitStatus;
extern PadMgr* sPadMgrInstance;
}

// Hook only works on startup
void RegisterVoiceInput() {

    static uint32_t shouldHookId1 = 0;
    static uint32_t shouldHookId2 = 0;
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHookId1);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHookId2);

    shouldHookId1 = 0;
    shouldHookId2 = 0;

    if (!CVarGetInteger("gEnhancements.Restorations.VoiceInput", 0)) {
        return;
    }

    shouldHookId1 = REGISTER_VB_SHOULD(VB_STANDARD_CONTROLLER, {
        s32 i = va_arg(args, s32);
        if (i == 3) {
            *should = false;
            if (sPadMgrInstance->ctrlrType[i] != PADMGR_CONT_VOICE) {

                // Mayber adjust ctrlrMask?
                sPadMgrInstance->ctrlrType[i] = PADMGR_CONT_VOICE_PLUGGED;
                sPadMgrInstance->pakType[i] = CONT_PAK_NONE;
            }
        }
    });

    shouldHookId2 = REGISTER_VB_SHOULD(VB_VOICE_INIT, {
        if (sVoiceInitStatus != 2) { // not VOICE_INIT_SUCCESS
            *should = true;
        }
    });
}