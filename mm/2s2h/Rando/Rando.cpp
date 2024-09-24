#include "Rando.h"
#include "Enhancements/GameInteractor/GameInteractor.h"
#include <libultraship/libultraship.h>
#include <random>
#include "Rando/ActorBehavior/ActorBehavior.h"

extern "C" {
#include "variables.h"
#include "functions.h"
}

void RandomizerQueueCheck(RandoCheckId randoCheckId) {
    auto& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];

    randoSaveCheck.eligible = true;
    GameInteractor::Instance->events.emplace_back(
        GIEventGiveItem{ .showGetItemCutscene = true,
                         .getItemText = Rando::StaticData::Items[randoSaveCheck.randoItemId].name,
                         .drawItem = [randoSaveCheck]() { Rando::DrawItem(randoSaveCheck.randoItemId); },
                         .giveItem =
                             [&randoSaveCheck]() {
                                 Rando::GiveItem(randoSaveCheck.randoItemId);
                                 randoSaveCheck.obtained = true;
                             } });
}

void RandomizerOnFlagSetHandler(FlagType flagType, u32 flag) {
    auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(flagType, flag);
    if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
        return;
    }

    RandomizerQueueCheck(randoStaticCheck.randoCheckId);
}

void RandomizerOnSceneFlagSetHandler(s16 sceneId, FlagType flagType, u32 flag) {
    auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(flagType, flag, sceneId);
    if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
        return;
    }

    RandomizerQueueCheck(randoStaticCheck.randoCheckId);
}

void RandomizerOnVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, void* optionalArg) {
    switch (id) {
        case GI_VB_GIVE_ITEM_FROM_GURUGURU:
        case GI_VB_GIVE_ITEM_FROM_SCRIPT:
        case GI_VB_GIVE_ITEM_FROM_SWORDSMAN:
            *should = false;
            break;
    }
}

// Very primitive randomizer implementation, when a save is created, if rando is enabled
// we set the save type to rando and shuffle all checks and persist the results to the save
void OnSaveInitHandler(s16 fileNum) {
    if (CVarGetInteger("gRando.Enabled", 0)) {
        gSaveContext.save.shipSaveInfo.saveType = SAVETYPE_RANDO;

        std::vector<RandoItemId> itemPool;
        for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
            if (randoStaticCheck.randoCheckId != RC_UNKNOWN) {
                itemPool.push_back(randoStaticCheck.randoItemId);
            }
        }

        std::shuffle(itemPool.begin(), itemPool.end(), std::mt19937(std::random_device()()));

        for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
            if (randoStaticCheck.randoCheckId != RC_UNKNOWN) {
                RANDO_SAVE_CHECKS[randoCheckId].randoItemId = itemPool.back();
                itemPool.pop_back();
            }
        }
    }
}

// When a save is loaded, we want to unregister all hooks and re-register them if it's a rando save
void OnSaveLoadHandler(s16 fileNum) {
    static uint32_t onFlagSetHook = 0;
    static uint32_t onSceneFlagSetHook = 0;
    static uint32_t onVanillaBehaviorHook = 0;

    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnFlagSet>(onFlagSetHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneFlagSet>(onSceneFlagSetHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::ShouldVanillaBehavior>(onVanillaBehaviorHook);

    onFlagSetHook = 0;
    onSceneFlagSetHook = 0;
    onVanillaBehaviorHook = 0;

    Rando::InitActorBehavior(gSaveContext.save.shipSaveInfo.saveType == SAVETYPE_RANDO);

    if (gSaveContext.save.shipSaveInfo.saveType != SAVETYPE_RANDO) {
        return;
    }

    onFlagSetHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagSet>(RandomizerOnFlagSetHandler);
    onSceneFlagSetHook =
        GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagSet>(RandomizerOnSceneFlagSetHandler);
    onVanillaBehaviorHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::ShouldVanillaBehavior>(
        RandomizerOnVanillaBehaviorHandler);
}

// Entry point for the rando module
void Rando::Init() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSaveInit>(OnSaveInitHandler);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSaveLoad>(OnSaveLoadHandler);
}
