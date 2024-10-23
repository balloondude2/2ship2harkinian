
#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
    
#include "variables.h"
extern s32 sVoiceInitStatus;
extern PadMgr* sPadMgrInstance;
}

// I don't call the hook anywhere
void RegisterVoiceInput(){

    static uint32_t shouldHookId1 = 0;
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHookId1);

    shouldHookId1 = 0;

    if (!CVarGetInteger("gEnhancements.Restorations.VoiceInput", 0)) { return;}


    shouldHookId1 = REGISTER_VB_SHOULD(VB_CONTROLLER_PORT_4, {
            *should = false;
            sPadMgrInstance->ctrlrType[3] = PADMGR_CONT_NONE;
            if (sVoiceInitStatus == 0) { // VOICE_INIT_FAILED
                sVoiceInitStatus = 1; // VOICE_INIT_TRY
            }
        
    });
}