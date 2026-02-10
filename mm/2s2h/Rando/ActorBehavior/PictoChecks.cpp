#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"
#include "functions.h"

#include "z64snap.h"
}

using PictoPosFn = Vec3f* (*)(Actor*);

Vec3f* PictoFocusPos(Actor* actor) {
    return &actor->focus.pos;
}

Vec3f* PictoWorldPos(Actor* actor) {
    return &actor->world.pos;
}

struct PictoActorInfo {
    int16_t flag;
    PictoPosFn getPos;
};

std::unordered_map<int16_t, PictoActorInfo> actorPictoFlagMap = {
    { ACTOR_OBJ_WARPSTONE, { RANDO_INF_PICTO_ACTOR_OBJ_WARPSTONE, PictoFocusPos } },
    { ACTOR_EN_BAL, { RANDO_INF_PICTO_ACTOR_EN_BAL, PictoFocusPos } },
    { ACTOR_EN_ELF, { RANDO_INF_PICTO_ACTOR_EN_ELF, PictoWorldPos } },
};

void ValidateRandoPicto(Actor* actor) {
    auto it = actorPictoFlagMap.find(actor->id);
    if (it != actorPictoFlagMap.end()) {
        int16_t flag = it->second.flag;

        Vec3f* pos = it->second.getPos(actor);
        // check if using PICTO_VALID_0 matters
        // Snap_ValidatePictograph parameters might need tweaking per actor
        if (Snap_ValidatePictograph(gPlayState, actor, PICTO_VALID_0, pos, &actor->shape.rot, 10.0f, 400.0f, -1) == 0) {
            // Maybe set eligble flag instead and use randoinf flag for better picto message?
            Flags_SetRandoInf(flag);
        }
    }
}

void Rando::ActorBehavior::InitPictoChecksBehavior() {
    bool shouldRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_PICTOS];

    COND_VB_SHOULD(VB_PICTO_TEST, shouldRegister, {
        Actor* actor = va_arg(args, Actor*);
        ValidateRandoPicto(actor);
    });
}