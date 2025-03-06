#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/Enhancements/Enhancements.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_En_Rg/z_en_rg.h"

s32 func_80BF42BC(EnRg* enRg, f32 arg1);

}

#define CVAR_NAME "gEnhancements.Minigames.GoronRace"
#define CVAR CVarGetInteger(CVAR_NAME, GORON_RACE_VANILLA)

void RegisterGoronRace() {
    COND_VB_SHOULD(VB_GORON_RACE, CVAR != GORON_RACE_VANILLA, {
        EnRg* enRg = va_arg(args, EnRg*);
        Player* player = GET_PLAYER(gPlayState);
        f32 phi_f0;
        f32 phi_f2;

        switch (CVAR) {
            case GORON_RACE_EASY:

                if (player->linearVelocity < 14.0f) { // change from 20
                    phi_f2 = 14.0f; // change from 20
                } else {
                    phi_f2 = player->linearVelocity;
                }

                if ((enRg->unk_310 & 0x400) || (enRg->unk_310 & 0x1000)) {
                    phi_f0 = 0.0f;
                } else if (enRg->numCheckpointsAheadOfPlayer >= 2) {
                    phi_f0 = phi_f2 * 0.4f; // change from .5
                } else if (enRg->numCheckpointsAheadOfPlayer == 1) {
                    phi_f0 = phi_f2 * 0.75f; // change from .75
                } else if (enRg->numCheckpointsAheadOfPlayer == 0) {
                    s16 temp_v0_3 = enRg->actor.yawTowardsPlayer - enRg->actor.world.rot.y;

                    if ((ABS_ALT(temp_v0_3) > 0x4000) || (enRg->unk_326 > 0)) { // think unk_326 is a 28 frame timer after a collision with the player
                        phi_f0 = phi_f2 * 0.3f; // changed from .72
                    } else {
                        phi_f0 = phi_f2 * 0.94f;
                    }
                } else if (enRg->numCheckpointsAheadOfPlayer == -1) {
                    phi_f0 = phi_f2 * 1.1f; // change from 1.6
                } else {
                    phi_f0 = 1.4f * phi_f2; // change from 2.0
                }
                break;
            case GORON_RACE_EASIER:

                if (player->linearVelocity > 10.0f) { // change
                    phi_f2 = 10.0f; // change from 20
                } else {
                    phi_f2 = player->linearVelocity;
                }
                
                if ((enRg->unk_310 & 0x400) || (enRg->unk_310 & 0x1000)) {
                    phi_f0 = 0.0f;
                } else if (enRg->numCheckpointsAheadOfPlayer >= 2) {
                    phi_f0 = phi_f2 * 0.3f; // change from .5
                } else if (enRg->numCheckpointsAheadOfPlayer == 1) {
                    phi_f0 = phi_f2 * 0.5f; // change from .75
                } else if (enRg->numCheckpointsAheadOfPlayer == 0) {
                    s16 temp_v0_3 = enRg->actor.yawTowardsPlayer - enRg->actor.world.rot.y;

                    if ((ABS_ALT(temp_v0_3) > 0x4000) || (enRg->unk_326 > 0)) { // think unk_326 is a 28 frame timer after a collision with the player
                        phi_f0 = phi_f2 * 0.3f; // changed from .72
                    } else {
                        phi_f0 = phi_f2 * 0.7f; // changed from .94
                    }
                } else if (enRg->numCheckpointsAheadOfPlayer == -1) {
                    phi_f0 = phi_f2 * 1.0f; // change from 1.6
                } else {
                    phi_f0 = 1.2f * phi_f2; // change from 2.0
                }
                break;
        }

        if (phi_f0 > 0.0f) {
                    if (enRg->unk_320 != 0) {
                        enRg->unk_320--;
                    }
                    phi_f2 = enRg->unk_320 * 0.01f;
                    phi_f0 += phi_f2;
                }

                func_80BF42BC(enRg, phi_f0);

        *should = true;
    });
}

static RegisterShipInitFunc initFunc(RegisterGoronRace, { CVAR_NAME });
