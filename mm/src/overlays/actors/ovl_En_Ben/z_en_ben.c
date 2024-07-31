/*
 * File: z_en_torch2.c
 * Overlay: ovl_En_Torch2
 * Description: Elegy of Emptiness Shell
 */

#include "z_en_ben.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#include "2s2h/Enhancements/GameInteractor/GameInteractor_Anchor.h"

#define FLAGS (ACTOR_FLAG_10)

#define THIS ((Ben*)thisx)

void Ben_Init(Actor* thisx, PlayState* play);
void Ben_Destroy(Actor* thisx, PlayState* play);
void Ben_Update(Actor* thisx, PlayState* play);
void Ben_Draw(Actor* thisx, PlayState* play2);

void Ben_UpdateIdle(Actor* thisx, PlayState* play);
void Ben_UpdateDeath(Actor* thisx, PlayState* play);

ActorInit En_Ben_InitVars = {
    /**/ ACTOR_EN_BEN,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(Ben),
    /**/ Ben_Init,
    /**/ Ben_Destroy,
    /**/ Ben_Update,
    /**/ Ben_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER | OC1_TYPE_1 | OC1_TYPE_2,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00100000, 0, 0 },
        { 0xF7CFFFFF, 0, 0 },
        TOUCH_NONE,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 20, 60, 0, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(colChkInfo.mass, MASS_IMMOVABLE, ICHAIN_STOP),
};

// Shells for each of Link's different forms
// (Playing elegy as Fierce Deity puts down a human shell)
static Gfx* sShellDLists[] = {
    gElegyShellHumanDL, gElegyShellGoronDL, gElegyShellZoraDL, gElegyShellDekuDL, gElegyShellHumanDL,
};

void Ben_Init(Actor* thisx, PlayState* play) {
    Ben* this = THIS;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);

    if (this->actor.params != BEN_PARAM_DEKU) {
        this->actor.flags |= ACTOR_FLAG_CAN_PRESS_SWITCH;
        if (this->actor.params == BEN_PARAM_GORON) {
            this->actor.flags |= ACTOR_FLAG_CAN_PRESS_HEAVY_SWITCH;
        }
    }
    this->framesUntilNextState = 0;
}

void Ben_Destroy(Actor* thisx, PlayState* play) {
    Ben* this = THIS;

    Collider_DestroyCylinder(play, &this->collider);
    // Play_SetRespawnData(&play->state, this->actor.params + RESPAWN_MODE_GORON - 1, 0xFF, 0,
    //                     PLAYER_PARAMS(0xFF, PLAYER_INITMODE_B), &this->actor.world.pos, this->actor.shape.rot.y);
    // play->actorCtx.elegyShells[this->actor.params] = NULL;
}

void Ben_Update(Actor* thisx, PlayState* play) {
    Ben* this = THIS;
    u16 targetAlpha;
    u16 remainingFrames;
    s32 pad[2];

    if (this->state == BEN_STATE_IDLE) {
        this->actor.update = Ben_UpdateIdle;
        return;
    }

    this->actor.gravity = -1.0f;
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 20.0f, 70.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);

    if (this->framesUntilNextState == 0) {
        remainingFrames = 0;
    } else {
        remainingFrames = --this->framesUntilNextState;
    }

    if (remainingFrames == 0) {
        if (this->state == BEN_STATE_INITIALIZED) {
            // Spawn in
            if (this->alpha == 0) {
                Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.home.pos);
                this->actor.shape.rot.y = this->actor.home.rot.y;
                this->state = BEN_STATE_FADING_IN;
            }
            targetAlpha = 0;
        } else if (this->state == BEN_STATE_FADING_IN) {
            // Stay semitransparent until the player moves away
            if ((this->actor.xzDistToPlayer > 32.0f) || (fabsf(this->actor.playerHeightRel) > 70.0f)) {
                this->state = BEN_STATE_SOLID;
            }
            targetAlpha = 60;
        } else {
            // Once the player has moved away, update collision and become opaque
            Collider_UpdateCylinder(&this->actor, &this->collider);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
            targetAlpha = 255;
        }
        Math_StepToS(&this->alpha, targetAlpha, 8);
    }

    if (Anchor_GetClientScene(this->actor.params - 3) == play->sceneId) {
        PosRot playerPosRot = Anchor_GetClientPosition(this->actor.params - 3);
        this->actor.world.pos = playerPosRot.pos;
        this->actor.shape.rot = playerPosRot.rot;
    } else {
        this->actor.world.pos.x = -9999.0f;
        this->actor.world.pos.y = -9999.0f;
        this->actor.world.pos.z = -9999.0f;
    }
}

void Ben_UpdateIdle(Actor* thisx, PlayState* play) {
    Ben* this = THIS;

    if (this->state == BEN_STATE_DYING) {
        // Start death animation
        this->actor.update = Ben_UpdateDeath;
        this->actor.velocity.y = 0.0f;
    }
}

void Ben_UpdateDeath(Actor* thisx, PlayState* play) {
    Ben* this = THIS;

    // Fall down and become transparent, then delete once invisible
    if (Math_StepToS(&this->alpha, 0, 8)) {
        Actor_Kill(&this->actor);
        return;
    }

    this->actor.gravity = -1.0f;
    Actor_MoveWithGravity(&this->actor);
}

void Ben_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Ben* this = THIS;
    u8 form = Anchor_GetClientPlayerData(this->actor.params - 3).playerForm;
    Gfx* gfx = sShellDLists[form];

    OPEN_DISPS(play->state.gfxCtx);

    if (this->alpha == 255) {
        Scene_SetRenderModeXlu(play, 0, 0x01);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
        Gfx_DrawDListOpa(play, gfx);
    } else {
        Scene_SetRenderModeXlu(play, 1, 0x02);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, this->alpha);
        Gfx_DrawDListXlu(play, gfx);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
