#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"
#include "functions.h"

#include "z64snap.h"
}

#define PICTO_FLAG(actorId) RANDO_INF_PICTO_##actorId

void Rando::ActorBehavior::InitPictoChecksBehavior() {
    bool shouldRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_PICTOS];

    COND_VB_SHOULD(VB_PICTO_TEST, shouldRegister, {
        Actor* actor = va_arg(args, Actor*);

        // check if using PICTO_VALID_0 matters
        // values in validate function are mostly taken from tingle's function, might need adjusting
        if (Snap_ValidatePictograph(gPlayState, actor, PICTO_VALID_0, &actor->focus.pos,
                                         &actor->shape.rot, 10.0f, 400.0f, -1) == 0) {

            // don't love this pattern                                
            switch (actor->id) {
                case ACTOR_OBJ_WARPSTONE:
                    Flags_SetRandoInf(PICTO_FLAG(ACTOR_OBJ_WARPSTONE));
                    break;
                case ACTOR_EN_BAL:
                    Flags_SetRandoInf(PICTO_FLAG(ACTOR_EN_BAL));
                    break;

            }
        }
    });
}