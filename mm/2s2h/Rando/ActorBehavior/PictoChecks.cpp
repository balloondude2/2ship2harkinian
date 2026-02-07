#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"
#include "functions.h"

#include "z64snap.h"
}


std::unordered_map<int16_t, int16_t> actorPictoFlagMap = {
    { ACTOR_OBJ_WARPSTONE, RANDO_INF_PICTO_ACTOR_OBJ_WARPSTONE },
    { ACTOR_EN_BAL, RANDO_INF_PICTO_ACTOR_EN_BAL },
};

void Rando::ActorBehavior::InitPictoChecksBehavior() {
    bool shouldRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_PICTOS];

    COND_VB_SHOULD(VB_PICTO_TEST, shouldRegister, {
        Actor* actor = va_arg(args, Actor*);

        // check if using PICTO_VALID_0 matters
        // values in validate function are mostly taken from tingle's function, might need adjusting
        if (Snap_ValidatePictograph(gPlayState, actor, PICTO_VALID_0, &actor->focus.pos,
                                         &actor->shape.rot, 10.0f, 400.0f, -1) == 0) {
                                  
            auto it = actorPictoFlagMap.find(actor->id);
            if (it != actorPictoFlagMap.end()) {
                int16_t flag = it->second;

                // Maybe set eligble flag instead and use randoinf flag for better picto message?
                Flags_SetRandoInf(flag);
            }
        }
    });
}