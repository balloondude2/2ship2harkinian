
// TODOs: send and receive flags/items; mabye update ben overlay; clean up organization (move json to jsonConversions
// file); nametags from Archez's PR

#include "GameInteractor_Anchor.h"
#include <libultraship/libultraship.h>
//#include <soh/Enhancements/item-tables/ItemTableManager.h>
//#include <soh/Enhancements/randomizer/randomizerTypes.h>
//#include <soh/Enhancements/randomizer/adult_trade_shuffle.h>
//#include <soh/Enhancements/nametag.h>
//#include <soh/util.h>
#include <nlohmann/json.hpp>
#include "2s2h/BenJsonConversions.hpp"
// mm\2s2h\BenJsonConversions.hpp

extern "C" {
#include <variables.h>
#include "macros.h"
#include "z64scene.h"
#include "z64actor.h"
#include "functions.h"
// extern "C" s16 gEnLinkPuppetId;
extern PlayState* gPlayState;
extern SaveContext gSaveContext;
}

s16 gEnLinkPuppetId = 0x2B2;

using json = nlohmann::json;


void from_json(const json& j, AnchorClient& client) {
    j.contains("clientId") ? j.at("clientId").get_to(client.clientId) : client.clientId = 0;
    j.contains("clientVersion") ? j.at("clientVersion").get_to(client.clientVersion) : client.clientVersion = "???";
    j.contains("name") ? j.at("name").get_to(client.name) : client.name = "???";
    j.contains("color") ? j.at("color").get_to(client.color) : client.color = { 255, 255, 255 };
    j.contains("seed") ? j.at("seed").get_to(client.seed) : client.seed = 0;
    j.contains("fileNum") ? j.at("fileNum").get_to(client.fileNum) : client.fileNum = 0xFF;
    j.contains("gameComplete") ? j.at("gameComplete").get_to(client.gameComplete) : client.gameComplete = false;
    j.contains("sceneNum") ? j.at("sceneNum").get_to(client.sceneNum) : client.sceneNum = SCENE_MAX;
    j.contains("roomIndex") ? j.at("roomIndex").get_to(client.roomIndex) : client.roomIndex = 0;
    j.contains("entranceIndex") ? j.at("entranceIndex").get_to(client.entranceIndex) : client.entranceIndex = 0;
    j.contains("posRot") ? j.at("posRot").get_to(client.posRot) : client.posRot = { -9999, -9999, -9999, 0, 0, 0 };
    j.contains("playerData") ? j.at("playerData").get_to(client.playerData) : client.playerData = { 0 };
}

void to_json(json& j, const CycleSceneFlags& flags) {
    j = json{
        {"chest", flags.chest},
        {"switch0", flags.switch0},
        {"switch1", flags.switch1},
        {"clearedRoom", flags.clearedRoom},
        {"collectible", flags.collectible},
    };
}

void from_json(const json& j, CycleSceneFlags& flags) {
    j.at("chest").get_to(flags.chest);
    j.at("switch0").get_to(flags.switch0);
    j.at("switch1").get_to(flags.switch1);
    j.at("clearedRoom").get_to(flags.clearedRoom);
    j.at("collectible").get_to(flags.collectible);
}


std::vector<std::string> sceneNames = {
    /* 0x00 */ "Southern Swamp (Clear)",
    /* 0x01 */ "0x01",
    /* 0x02 */ "0x02",
    /* 0x03 */ "0x03",
    /* 0x04 */ "0x04",
    /* 0x05 */ "0x05",
    /* 0x06 */ "0x06",
    /* 0x07 */ "Lone Peak Shrine & Grottos",
    /* 0x08 */ "Cutscene Scene",
    /* 0x09 */ "0x09",
    /* 0x0A */ "Magic Hags' Potion Shop",
    /* 0x0B */ "Majora's Lair",
    /* 0x0C */ "Beneath the Graveyard",
    /* 0x0D */ "Curiosity Shop",
    /* 0x0E */ "0x0E",
    /* 0x0F */ "0x0F",
    /* 0x10 */ "Mama's House & Barn",
    /* 0x11 */ "Honey & Darling's Shop",
    /* 0x12 */ "The Mayor's Residence",
    /* 0x13 */ "Ikana Canyon",
    /* 0x14 */ "Pirates' Fortress",
    /* 0x15 */ "Milk Bar",
    /* 0x16 */ "Stone Tower Temple",
    /* 0x17 */ "Treasure Chest Shop",
    /* 0x18 */ "Inverted Stone Tower Temple",
    /* 0x19 */ "Clock Tower Rooftop",
    /* 0x1A */ "Before Clock Town",
    /* 0x1B */ "Woodfall Temple",
    /* 0x1C */ "Path to Mountain Village",
    /* 0x1D */ "Ancient Castle of Ikana",
    /* 0x1E */ "Deku Scrub Playground",
    /* 0x1F */ "Odolwa's Lair",
    /* 0x20 */ "Town Shooting Gallery",
    /* 0x21 */ "Snowhead Temple",
    /* 0x22 */ "Milk Road",
    /* 0x23 */ "Pirates' Fortress Interior",
    /* 0x24 */ "Swamp Shooting Gallery",
    /* 0x25 */ "Pinnacle Rock",
    /* 0x26 */ "Fairy's Fountain",
    /* 0x27 */ "Swamp Spider House",
    /* 0x28 */ "Oceanside Spider House",
    /* 0x29 */ "Astral Observatory",
    /* 0x2A */ "Moon Deku Trial",
    /* 0x2B */ "Deku Palace",
    /* 0x2C */ "Mountain Smithy",
    /* 0x2D */ "Termina Field",
    /* 0x2E */ "Post Office",
    /* 0x2F */ "Marine Research Lab",
    /* 0x30 */ "Beneath Graveyard and Dampe's House",
    /* 0x31 */ "0x31",
    /* 0x32 */ "Goron Shrine",
    /* 0x33 */ "Zora Hall",
    /* 0x34 */ "Trading Post",
    /* 0x35 */ "Romani Ranch",
    /* 0x36 */ "Twinmold's Lair",
    /* 0x37 */ "Great Bay Coast",
    /* 0x38 */ "Zora Cape",
    /* 0x39 */ "Lottery Shop",
    /* 0x3A */ "0x3A",
    /* 0x3B */ "Pirates' Fortress Moat",
    /* 0x3C */ "Fisherman's Hut",
    /* 0x3D */ "Goron Shop",
    /* 0x3E */ "Deku King's Chamber",
    /* 0x3F */ "Moon Goron Trial",
    /* 0x40 */ "Road to Southern Swamp",
    /* 0x41 */ "Doggy Racetrack",
    /* 0x42 */ "Cucco Shack",
    /* 0x43 */ "Ikana Graveyard",
    /* 0x44 */ "Goht's Lair",
    /* 0x45 */ "Southern Swamp (poison)",
    /* 0x46 */ "Woodfall",
    /* 0x47 */ "Moon Zora Trial",
    /* 0x48 */ "Goron Village (spring)",
    /* 0x49 */ "Great Bay Temple",
    /* 0x4A */ "Waterfall Rapids",
    /* 0x4B */ "Beneath the Well",
    /* 0x4C */ "Zora Hall Rooms",
    /* 0x4D */ "Goron Village (winter)",
    /* 0x4E */ "Goron Graveyard",
    /* 0x4F */ "Sakon's Hideout",
    /* 0x50 */ "Mountain Village (winter)",
    /* 0x51 */ "Ghost Hut",
    /* 0x52 */ "Deku Shrine",
    /* 0x53 */ "Road to Ikana",
    /* 0x54 */ "Swordsman's School",
    /* 0x55 */ "Music Box House",
    /* 0x56 */ "Igos du Ikana's Lair",
    /* 0x57 */ "Tourist Information",
    /* 0x58 */ "Stone Tower",
    /* 0x59 */ "Inverted Stone Tower",
    /* 0x5A */ "Mountain Village (spring)",
    /* 0x5B */ "Path to Snowhead",
    /* 0x5C */ "Snowhead",
    /* 0x5D */ "Path to Goron Village (winter)",
    /* 0x5E */ "Path to Goron Village (spring)",
    /* 0x5F */ "Gyorg's Lair",
    /* 0x60 */ "Secret Shrine",
    /* 0x61 */ "Stock Pot Inn",
    /* 0x62 */ "Great Bay Cutscene",
    /* 0x63 */ "Clock Tower Interior",
    /* 0x64 */ "Woods of Mystery",
    /* 0x65 */ "Lost Woods (Intro)",
    /* 0x66 */ "Moon Link Trial",
    /* 0x67 */ "The Moon",
    /* 0x68 */ "Bomb Shop",
    /* 0x69 */ "Giants' Chamber",
    /* 0x6A */ "Gorman Track",
    /* 0x6B */ "Goron Racetrack",
    /* 0x6C */ "East Clock Town",
    /* 0x6D */ "West Clock Town",
    /* 0x6E */ "North Clock Town",
    /* 0x6F */ "South Clock Town",
    /* 0x70 */ "Laundry Pool",

};

const std::string& GetSceneName(uint8_t scene) {
    return sceneNames[scene];
}

std::map<uint32_t, AnchorClient> GameInteractorAnchor::AnchorClients = {};
std::vector<uint32_t> GameInteractorAnchor::ActorIndexToClientId = {};
std::string GameInteractorAnchor::clientVersion = "MM Anchor + Statues Alpha";
std::vector<std::pair<uint16_t, int16_t>> receivedItems = {};
std::vector<AnchorMessage> anchorMessages = {};
uint32_t notificationId = 0;

void Anchor_DisplayMessage(AnchorMessage message = {}) {
    message.id = notificationId++;
    anchorMessages.push_back(message);
}

void Anchor_SendClientData() {
    nlohmann::json payload;
    payload["data"]["name"] = CVarGetString("gRemote.AnchorName", "");
    payload["data"]["color"] = CVarGetColor24("gRemote.AnchorColor", { 100, 255, 100 });
    payload["data"]["clientVersion"] = GameInteractorAnchor::clientVersion;
    payload["type"] = "UPDATE_CLIENT_DATA";

    // TODO: Doesn't crash when Anchor is enabled at start, but needs reconnect to work. Maybe call this function on
    // game load
    if (GameInteractor::IsSaveLoaded()) {
        payload["data"]["fileNum"] = gSaveContext.fileNum;
        payload["data"]["sceneNum"] = gPlayState->sceneId;
        payload["data"]["entranceIndex"] = gSaveContext.save.entrance;
    }

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void GameInteractorAnchor::Enable() {
    if (isEnabled) {
        return;
    }

    isEnabled = true;
    GameInteractor::Instance->EnableRemoteInteractor();
    GameInteractor::Instance->RegisterRemoteJsonHandler([&](nlohmann::json payload) {
        try {
            HandleRemoteJson(payload);
        } catch (const std::exception& e) {
            SPDLOG_ERROR("[Anchor] Error handling remote JSON: {}", e.what());
        } catch (...) { SPDLOG_ERROR("[Anchor] Unknown Error handling remote JSON"); }
    });
    GameInteractor::Instance->RegisterRemoteConnectedHandler([&]() {
        Anchor_DisplayMessage({ .message = "Connected to Anchor" });
        Anchor_SendClientData();

        if (GameInteractor::IsSaveLoaded()) {
            Anchor_RequestSaveStateFromRemote();
        }
    });
    GameInteractor::Instance->RegisterRemoteDisconnectedHandler(
        [&]() { Anchor_DisplayMessage({ .message = "Disconnected from Anchor" }); });
}

void GameInteractorAnchor::Disable() {
    if (!isEnabled) {
        return;
    }

    isEnabled = false;
    GameInteractor::Instance->DisableRemoteInteractor();

    GameInteractorAnchor::AnchorClients.clear();
    // TODO: This crashes in player model build for some reason
    Anchor_RefreshClientActors();
}

void GameInteractorAnchor::TransmitJsonToRemote(nlohmann::json payload) {
    payload["roomId"] = CVarGetString("gRemote.AnchorRoomId", "");
    if (!payload.contains("quiet")) {
        SPDLOG_INFO("[Anchor] Sending payload:\n{}", payload.dump());
    }
    GameInteractor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_ParseSaveStateFromRemote(nlohmann::json payload);
void Anchor_PushSaveStateToRemote();

void GameInteractorAnchor::HandleRemoteJson(nlohmann::json payload) {
    if (!payload.contains("type")) {
        return;
    }

    if (!payload.contains("quiet")) {
        SPDLOG_INFO("[Anchor] Received payload:\n{}", payload.dump());
    }

    if ((payload["type"] != "ALL_CLIENT_DATA" && payload["type"] != "UPDATE_CLIENT_DATA")) {
        if (payload.contains("clientId")) {
            uint32_t clientId = payload["clientId"].get<uint32_t>();
            if (GameInteractorAnchor::AnchorClients.contains(clientId) &&
                GameInteractorAnchor::AnchorClients[clientId].clientVersion != GameInteractorAnchor::clientVersion) {
                return;
            }
        }
    }

    // if (payload["type"] == "GIVE_ITEM") {
    //     auto effect = new GameInteractionEffect::GiveItem();
    //     effect->parameters[0] = payload["modId"].get<uint16_t>();
    //     effect->parameters[1] = payload["getItemId"].get<int16_t>();
    //     CVarSetInteger("gFromGI", 1);
    //     receivedItems.push_back({ payload["modId"].get<uint16_t>(), payload["getItemId"].get<int16_t>() });
    //     if (effect->Apply() == Possible) {
    //         GetItemEntry getItemEntry = ItemTableManager::Instance->RetrieveItemEntry(effect->parameters[0],
    //         effect->parameters[1]);

    //         AnchorClient anchorClient = GameInteractorAnchor::AnchorClients[payload["clientId"].get<uint32_t>()];
    //         if (getItemEntry.getItemCategory != ITEM_CATEGORY_JUNK) {
    //             if (getItemEntry.modIndex == MOD_NONE) {
    //                 Anchor_DisplayMessage({
    //                     .itemIcon = SohUtils::GetIconNameFromItemID(getItemEntry.itemId),
    //                     .prefix = SohUtils::GetItemName(getItemEntry.itemId),
    //                     .message = "from",
    //                     .suffix = anchorClient.name
    //                 });
    //             } else if (getItemEntry.modIndex == MOD_RANDOMIZER) {
    //                 Anchor_DisplayMessage({
    //                     .itemIcon =
    //                     SohUtils::GetIconNameFromItemID(SohUtils::GetItemIdIconFromRandomizerGet(getItemEntry.getItemId)),
    //                     .prefix =
    //                     OTRGlobals::Instance->gRandomizer->EnumToSpoilerfileGetName[(RandomizerGet)getItemEntry.getItemId][gSaveContext.language],
    //                     .message = "from",
    //                     .suffix = anchorClient.name
    //                 });
    //             }
    //         }
    //     }
    //     CVarClear("gFromGI");
    // }
    // if (payload["type"] == "SET_SCENE_FLAG") {
    //     auto effect = new GameInteractionEffect::SetSceneFlag();
    //     effect->parameters[0] = payload["sceneNum"].get<int16_t>();
    //     effect->parameters[1] = payload["flagType"].get<int16_t>();
    //     effect->parameters[2] = payload["flag"].get<int16_t>();
    //     effect->Apply();
    // }
    // if (payload["type"] == "SET_FLAG") {
    //     auto effect = new GameInteractionEffect::SetFlag();
    //     effect->parameters[0] = payload["flagType"].get<int16_t>();
    //     effect->parameters[1] = payload["flag"].get<int16_t>();
    //     effect->Apply();

    //     // If mweep flag replace ruto's letter
    //     if (
    //         payload["flagType"].get<int16_t>() == FLAG_EVENT_CHECK_INF &&
    //         payload["flag"].get<int16_t>() == EVENTCHKINF_KING_ZORA_MOVED &&
    //         Inventory_HasSpecificBottle(ITEM_LETTER_RUTO)
    //     ) {
    //         Inventory_ReplaceItem(gPlayState, ITEM_LETTER_RUTO, ITEM_BOTTLE);
    //     }
    // }
    // if (payload["type"] == "UNSET_SCENE_FLAG") {
    //     auto effect = new GameInteractionEffect::UnsetSceneFlag();
    //     effect->parameters[0] = payload["sceneNum"].get<int16_t>();
    //     effect->parameters[1] = payload["flagType"].get<int16_t>();
    //     effect->parameters[2] = payload["flag"].get<int16_t>();
    //     effect->Apply();
    // }
    // if (payload["type"] == "UNSET_FLAG") {
    //     auto effect = new GameInteractionEffect::UnsetFlag();
    //     effect->parameters[0] = payload["flagType"].get<int16_t>();
    //     effect->parameters[1] = payload["flag"].get<int16_t>();
    //     effect->Apply();
    // }
    // if (payload["type"] == "DAMAGE_PLAYER") {
    //     if (payload["damageEffect"] > 0 && GET_PLAYER(gPlayState)->invincibilityTimer <= 0 &&
    //         !Player_InBlockingCsMode(gPlayState, GET_PLAYER(gPlayState))) {
    //         if (payload["damageEffect"] == PUPPET_DMGEFF_NORMAL) {
    //             u8 damage = payload["damageValue"];
    //             Player_InflictDamage(gPlayState, damage * -4);
    //             func_80837C0C(gPlayState, GET_PLAYER(gPlayState), 0, 0, 0, 0, 0);
    //             GET_PLAYER(gPlayState)->invincibilityTimer = 18;
    //             GET_PLAYER(gPlayState)->actor.freezeTimer = 0;
    //         } else if (payload["damageEffect"] == PUPPET_DMGEFF_ICE) {
    //             GET_PLAYER(gPlayState)->stateFlags1 &= ~(PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_ITEM_OVER_HEAD);
    //             func_80837C0C(gPlayState, GET_PLAYER(gPlayState), 3, 0.0f, 0.0f, 0, 20);
    //             GET_PLAYER(gPlayState)->invincibilityTimer = 18;
    //             GET_PLAYER(gPlayState)->actor.freezeTimer = 0;
    //         } else if (payload["damageEffect"] == PUPPET_DMGEFF_FIRE) {
    //             for (int i = 0; i < 18; i++) {
    //                 GET_PLAYER(gPlayState)->flameTimers[i] = Rand_S16Offset(0, 200);
    //             }
    //             GET_PLAYER(gPlayState)->isBurning = true;
    //             func_80837C0C(gPlayState, GET_PLAYER(gPlayState), 0, 0, 0, 0, 0);
    //             GET_PLAYER(gPlayState)->invincibilityTimer = 18;
    //             GET_PLAYER(gPlayState)->actor.freezeTimer = 0;
    //         } else if (payload["damageEffect"] == PUPPET_DMGEFF_THUNDER) {
    //             func_80837C0C(gPlayState, GET_PLAYER(gPlayState), 4, 0.0f, 0.0f, 0, 20);
    //             GET_PLAYER(gPlayState)->invincibilityTimer = 18;
    //             GET_PLAYER(gPlayState)->actor.freezeTimer = 0;
    //         } else if (payload["damageEffect"] == PUPPET_DMGEFF_KNOCKBACK) {
    //             func_8002F71C(gPlayState, &GET_PLAYER(gPlayState)->actor, 100.0f * 0.04f + 4.0f,
    //             GET_PLAYER(gPlayState)->actor.world.rot.y, 8.0f); GET_PLAYER(gPlayState)->invincibilityTimer = 28;
    //             GET_PLAYER(gPlayState)->actor.freezeTimer = 0;
    //         } else if (payload["damageEffect"] == PUPPET_DMGEFF_STUN) {
    //             GET_PLAYER(gPlayState)->actor.freezeTimer = 20;
    //             Actor_SetColorFilter(&GET_PLAYER(gPlayState)->actor, 0, 0xFF, 0, 10);
    //         }
    //     }
    // }
    if (payload["type"] == "CLIENT_UPDATE") {
        uint32_t clientId = payload["clientId"].get<uint32_t>();

        if (GameInteractorAnchor::AnchorClients.contains(clientId)) {
            GameInteractorAnchor::AnchorClients[clientId].sceneNum = payload["sceneNum"].get<int16_t>();
            GameInteractorAnchor::AnchorClients[clientId].roomIndex =
                payload.contains("roomIndex") ? payload.at("roomIndex").get<int16_t>() : 0;
            GameInteractorAnchor::AnchorClients[clientId].entranceIndex =
                payload.contains("entranceIndex") ? payload.at("entranceIndex").get<int16_t>() : 0;
            GameInteractorAnchor::AnchorClients[clientId].posRot = payload["posRot"].get<PosRot>();
            if (payload.contains("playerData")) {
                GameInteractorAnchor::AnchorClients[clientId].playerData = payload["playerData"].get<PlayerData>();
            }
            if (payload.contains("jointTable")) {
                std::vector<Vec3s> jointTable = payload["jointTable"].get<std::vector<Vec3s>>();
                for (int i = 0; i < 23; i++) {
                    GameInteractorAnchor::AnchorClients[clientId].jointTable[i] = jointTable[i];
                }
            }
        }
    }
    // if (payload["type"] == "PUSH_SAVE_STATE" && GameInteractor::IsSaveLoaded()) {
    //     Anchor_ParseSaveStateFromRemote(payload);
    // }
    // if (payload["type"] == "REQUEST_SAVE_STATE" && GameInteractor::IsSaveLoaded()) {
    //     Anchor_PushSaveStateToRemote();
    // }
    if (payload["type"] == "ALL_CLIENT_DATA") {
        std::vector<AnchorClient> newClients = payload["clients"].get<std::vector<AnchorClient>>();

        // add new clients
        for (auto& client : newClients) {
            if (!GameInteractorAnchor::AnchorClients.contains(client.clientId)) {
                GameInteractorAnchor::AnchorClients[client.clientId] = {
                    client.clientId,
                    client.clientVersion,
                    client.name,
                    client.color,
                    client.seed,
                    client.fileNum,
                    client.gameComplete,
                    client.sceneNum,
                    0,
                    client.entranceIndex,
                    { -9999, -9999, -9999, 0, 0, 0 },
                    { 0 },
                    {},
                };
                Anchor_DisplayMessage(
                    { .prefix = client.name, .prefixColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f), .message = "connected" });
            }
        }

        // remove clients that are no longer in the list
        std::vector<uint32_t> clientsToRemove;
        for (auto& [clientId, client] : GameInteractorAnchor::AnchorClients) {
            if (std::find_if(newClients.begin(), newClients.end(), [clientId = clientId](AnchorClient& c) {
                    return c.clientId == clientId;
                }) == newClients.end()) {
                clientsToRemove.push_back(clientId);
            }
        }
        for (auto& clientId : clientsToRemove) {
            Anchor_DisplayMessage({ .prefix = GameInteractorAnchor::AnchorClients[clientId].name,
                                    .prefixColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f),
                                    .message = "disconnected" });
            GameInteractorAnchor::AnchorClients.erase(clientId);
        }

        Anchor_RefreshClientActors();
    }
    if (payload["type"] == "UPDATE_CLIENT_DATA") {
        uint32_t clientId = payload["clientId"].get<uint32_t>();
        if (GameInteractorAnchor::AnchorClients.contains(clientId)) {
            AnchorClient client = payload["data"].get<AnchorClient>();
            GameInteractorAnchor::AnchorClients[clientId].clientVersion = client.clientVersion;
            GameInteractorAnchor::AnchorClients[clientId].name = client.name;
            GameInteractorAnchor::AnchorClients[clientId].color = client.color;
            GameInteractorAnchor::AnchorClients[clientId].seed = client.seed;
            GameInteractorAnchor::AnchorClients[clientId].fileNum = client.fileNum;
            GameInteractorAnchor::AnchorClients[clientId].gameComplete = client.gameComplete;
            GameInteractorAnchor::AnchorClients[clientId].sceneNum = client.sceneNum;
            GameInteractorAnchor::AnchorClients[clientId].entranceIndex = client.entranceIndex;
        }
    }
    // if (payload["type"] == "UPDATE_CHECK_DATA" && GameInteractor::IsSaveLoaded()) {
    //     auto check = payload["locationIndex"].get<uint32_t>();
    //     auto data = payload["checkData"].get<RandomizerCheckTrackerData>();
    //     CheckTracker::UpdateCheck(check, data);
    // }
    // if (payload["type"] == "ENTRANCE_DISCOVERED") {
    //     auto entranceIndex = payload["entranceIndex"].get<uint16_t>();
    //     discoveredEntrances.push_back(entranceIndex);
    //     Entrance_SetEntranceDiscovered(entranceIndex, 1);
    // }
    // if (payload["type"] == "UPDATE_BEANS_BOUGHT" && GameInteractor::IsSaveLoaded()) {
    //     BEANS_BOUGHT = payload["amount"].get<uint8_t>();
    // }
    // if (payload["type"] == "UPDATE_BEANS_COUNT" && GameInteractor::IsSaveLoaded()) {
    //     AMMO(ITEM_BEAN) = payload["amount"].get<uint8_t>();
    // }
    // if (payload["type"] == "CONSUME_ADULT_TRADE_ITEM" && GameInteractor::IsSaveLoaded()) {
    //     uint8_t itemId = payload["itemId"].get<uint8_t>();
    //     gSaveContext.adultTradeItems &= ~ADULT_TRADE_FLAG(itemId);
    //     Inventory_ReplaceItem(gPlayState, itemId, Randomizer_GetNextAdultTradeItem());
    // }
    // if (payload["type"] == "UPDATE_KEY_COUNT" && GameInteractor::IsSaveLoaded()) {
    //     gSaveContext.inventory.dungeonKeys[payload["sceneNum"].get<int16_t>()] = payload["amount"].get<int8_t>();
    // }
    // if (payload["type"] == "GIVE_DUNGEON_ITEM" && GameInteractor::IsSaveLoaded()) {
    //     gSaveContext.inventory.dungeonItems[payload["sceneNum"].get<int16_t>()] |=
    //     gBitFlags[payload["itemId"].get<uint16_t>() - ITEM_KEY_BOSS];
    // }
    // if (payload["type"] == "GAME_COMPLETE") {
    //     AnchorClient anchorClient = GameInteractorAnchor::AnchorClients[payload["clientId"].get<uint32_t>()];
    //     Anchor_DisplayMessage({
    //         .prefix = anchorClient.name,
    //         .message = "has killed Ganon.",
    //     });
    // }
    // if (payload["type"] == "REQUEST_TELEPORT") {
    //     Anchor_TeleportToPlayer(payload["clientId"].get<uint32_t>());
    // }
    // if (payload["type"] == "TELEPORT_TO") {
    //     uint32_t entranceIndex = payload["entranceIndex"].get<uint32_t>();
    //     uint32_t roomIndex = payload["roomIndex"].get<uint32_t>();
    //     PosRot posRot = payload["posRot"].get<PosRot>();

    //     Play_SetRespawnData(gPlayState, RESPAWN_MODE_DOWN, entranceIndex, roomIndex, 0xDFF, &posRot.pos,
    //     posRot.rot.y); Play_TriggerVoidOut(gPlayState);
    // }
    // if (payload["type"] == "SERVER_MESSAGE") {
    //     Anchor_DisplayMessage({
    //         .prefix = "Server:",
    //         .prefixColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f),
    //         .message = payload["message"].get<std::string>(),
    //     });
    // }
    if (payload["type"] == "DISABLE_ANCHOR") {
        GameInteractor::Instance->isRemoteInteractorEnabled = false;
        GameInteractorAnchor::Instance->isEnabled = false;
    }
    // if (payload["type"] == "RESET") {
    //     std::reinterpret_pointer_cast<LUS::ConsoleWindow>(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))->Dispatch("reset");
    // }
}

void Anchor_PushSaveStateToRemote() {
    nlohmann::json payload; // = gSaveContext;
    payload["type"] = "PUSH_SAVE_STATE";
    // manually update current scene flags
    // payload["sceneFlags"][gPlayState->sceneNum]["chest"] = gPlayState->actorCtx.flags.chest;
    // payload["sceneFlags"][gPlayState->sceneNum]["swch"] = gPlayState->actorCtx.flags.swch;
    // payload["sceneFlags"][gPlayState->sceneNum]["clear"] = gPlayState->actorCtx.flags.clear;
    // payload["sceneFlags"][gPlayState->sceneNum]["collect"] = gPlayState->actorCtx.flags.collect;

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_RequestSaveStateFromRemote() {
    nlohmann::json payload;
    payload["type"] = "REQUEST_SAVE_STATE";

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_ParseSaveStateFromRemote(nlohmann::json payload){
    // SaveContext loadedData = payload.get<SaveContext>();

    // gSaveContext.healthCapacity = loadedData.healthCapacity;
    // gSaveContext.magicLevel = loadedData.magicLevel;
    // gSaveContext.magicCapacity = gSaveContext.magic = loadedData.magicCapacity;
    // gSaveContext.isMagicAcquired = loadedData.isMagicAcquired;
    // gSaveContext.isDoubleMagicAcquired = loadedData.isDoubleMagicAcquired;
    // gSaveContext.isDoubleDefenseAcquired = loadedData.isDoubleDefenseAcquired;
    // gSaveContext.bgsFlag = loadedData.bgsFlag;
    // gSaveContext.swordHealth = loadedData.swordHealth;
    // // TODO: Packet to live update this
    // gSaveContext.adultTradeItems = loadedData.adultTradeItems;

    // for (int i = 0; i < 124; i++) {
    //     gSaveContext.sceneFlags[i] = loadedData.sceneFlags[i];
    //     if (gPlayState->sceneNum == i) {
    //         gPlayState->actorCtx.flags.chest = loadedData.sceneFlags[i].chest;
    //         gPlayState->actorCtx.flags.swch = loadedData.sceneFlags[i].swch;
    //         gPlayState->actorCtx.flags.clear = loadedData.sceneFlags[i].clear;
    //         gPlayState->actorCtx.flags.collect = loadedData.sceneFlags[i].collect;
    //     }
    // }

    // for (int i = 0; i < 14; i++) {
    //     gSaveContext.eventChkInf[i] = loadedData.eventChkInf[i];
    // }

    // for (int i = 0; i < 4; i++) {
    //     gSaveContext.itemGetInf[i] = loadedData.itemGetInf[i];
    // }

    // // Skip last row of infTable, don't want to sync swordless flag
    // for (int i = 0; i < 29; i++) {
    //     gSaveContext.infTable[i] = loadedData.infTable[i];
    // }

    // for (int i = 0; i < 9; i++) {
    //     gSaveContext.randomizerInf[i] = loadedData.randomizerInf[i];
    // }

    // for (int i = 0; i < 6; i++) {
    //     gSaveContext.gsFlags[i] = loadedData.gsFlags[i];
    // }

    // for (int i = 0; i < 746; i++) {
    //     if (!gSaveContext.sohStats.locationsSkipped[i]) {
    //         gSaveContext.sohStats.locationsSkipped[i] = loadedData.sohStats.locationsSkipped[i];
    //     }
    // }

    // // Restore master sword state
    // u8 hasMasterSword = CHECK_OWNED_EQUIP(EQUIP_SWORD, 1);
    // if (hasMasterSword) {
    //     loadedData.inventory.equipment |= 0x2;
    // } else {
    //     loadedData.inventory.equipment &= ~0x2;
    // }

    // // Restore bottle contents (unless it's ruto's letter)
    // for (int i = 0; i < 4; i++) {
    //     if (gSaveContext.inventory.items[SLOT_BOTTLE_1 + i] != ITEM_NONE &&
    //     gSaveContext.inventory.items[SLOT_BOTTLE_1 + i] != ITEM_LETTER_RUTO) {
    //         loadedData.inventory.items[SLOT_BOTTLE_1 + i] = gSaveContext.inventory.items[SLOT_BOTTLE_1 + i];
    //     }
    // }

    // // Restore ammo if it's non-zero, unless it's beans
    // for (int i = 0; i < ARRAY_COUNT(gSaveContext.inventory.ammo); i++) {
    //     if (gSaveContext.inventory.ammo[i] != 0 && i != SLOT(ITEM_BEAN) && i != SLOT(ITEM_BEAN + 1)) {
    //         loadedData.inventory.ammo[i] = gSaveContext.inventory.ammo[i];
    //     }
    // }

    // gSaveContext.inventory = loadedData.inventory;
    // Anchor_DisplayMessage({ .message = "State loaded from remote!" });
};

AnchorClient* Anchor_GetClientByActorIndex(uint32_t actorIndex) {
    if (actorIndex < GameInteractorAnchor::ActorIndexToClientId.size()) {
        uint32_t clientId = GameInteractorAnchor::ActorIndexToClientId[actorIndex];
        if (GameInteractorAnchor::AnchorClients.find(clientId) != GameInteractorAnchor::AnchorClients.end()) {
            return &GameInteractorAnchor::AnchorClients[clientId];
        }
    }

    return nullptr;
}

uint8_t Anchor_GetClientScene(uint32_t actorIndex) {
    AnchorClient* client = Anchor_GetClientByActorIndex(actorIndex);
    if (client == nullptr)
        return SCENE_MAX;

    return client->sceneNum;
}

PosRot Anchor_GetClientPosition(uint32_t actorIndex) {
    AnchorClient* client = Anchor_GetClientByActorIndex(actorIndex);
    if (client == nullptr)
        return { -9999.0, -9999.0, -9999.0, 0, 0, 0 };

    return client->posRot;
}

PlayerData Anchor_GetClientPlayerData(uint32_t actorIndex) {
    AnchorClient* client = Anchor_GetClientByActorIndex(actorIndex);
    if (client == nullptr)
        return { 0 };

    return client->playerData;
}

Vec3s* Anchor_GetClientJointTable(uint32_t actorIndex) {
    AnchorClient* client = Anchor_GetClientByActorIndex(actorIndex);
    if (client == nullptr)
        return {};

    return client->jointTable;
}

const char* Anchor_GetClientName(uint32_t actorIndex) {
    AnchorClient* client = Anchor_GetClientByActorIndex(actorIndex);
    if (client == nullptr)
        return "";

    return client->name.c_str();
}

uint8_t Anchor_GetClientRoomIndex(uint32_t actorIndex) {
    AnchorClient* client = Anchor_GetClientByActorIndex(actorIndex);
    if (client == nullptr)
        return 0xFF;

    return client->roomIndex;
}

Color_RGB8 Anchor_GetClientColor(uint32_t actorIndex) {
    AnchorClient* client = Anchor_GetClientByActorIndex(actorIndex);
    if (client == nullptr)
        return { 100, 255, 100 };

    return client->color;
}

void Anchor_RefreshClientActors() {
    if (!GameInteractor::IsSaveLoaded())
        return;
    Actor* actor = gPlayState->actorCtx.actorLists[ACTORCAT_ITEMACTION].first;
    while (actor != NULL) {
        if (gEnLinkPuppetId == actor->id) {
            Actor_Kill(actor);
        }
        actor = actor->next;
    }

    GameInteractorAnchor::ActorIndexToClientId.clear();

    uint32_t i = 0;
    for (auto [clientId, client] : GameInteractorAnchor::AnchorClients) {
        GameInteractorAnchor::ActorIndexToClientId.push_back(clientId);
        if (Actor_Spawn(&gPlayState->actorCtx, gPlayState, gEnLinkPuppetId, client.posRot.pos.x, client.posRot.pos.y,
                        client.posRot.pos.z, client.posRot.rot.x, client.posRot.rot.y, client.posRot.rot.z,
                        3 + i) == NULL) {}
        // Todo: This was removed in player models branch
        // NameTag_RegisterForActor(fairy, client.name.c_str());
        i++;
    }
}

static uint32_t lastSceneNum = SCENE_MAX;

void Anchor_RegisterHooks() {
    // TODO: Might need to change this hook to be after scene commands
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](s16 sceneId, s8 spawnNum) {
        if (gPlayState == NULL || !GameInteractor::Instance->isRemoteInteractorConnected)
            return;

        // Moved to a new scene
        if (lastSceneNum != gPlayState->sceneId) {
            Anchor_SendClientData();
        }

        if (GameInteractor::Instance->IsSaveLoaded()) {
            // Player loaded into file
            if (lastSceneNum == SCENE_MAX) {
                Anchor_RequestSaveStateFromRemote();
            }

            Anchor_RefreshClientActors();
        }

        lastSceneNum = gPlayState->sceneId;
    });
    // GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPresentFileSelect>([]() {
    //     lastSceneNum = SCENE_MAX;
    //     if (!GameInteractor::Instance->isRemoteInteractorConnected) return;

    //     Anchor_SendClientData();
    // });
    // GameInteractor::Instance->RegisterGameHook<GameInteractor::OnExitGame>([](int32_t fileNum) {
    //     lastSceneNum = SCENE_MAX;
    //     if (!GameInteractor::Instance->isRemoteInteractorConnected) return;

    //     Anchor_SendClientData();
    // });
    // GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemReceive>([](GetItemEntry itemEntry) {
    //     if (itemEntry.modIndex == MOD_NONE && ((itemEntry.itemId >= ITEM_KEY_BOSS && itemEntry.itemId <=
    //     ITEM_KEY_SMALL) || itemEntry.itemId == ITEM_SWORD_MASTER)) {
    //         return;
    //     }

    //     // If the item exists in receivedItems, remove it from the list and don't emit the packet
    //     auto it = std::find_if(receivedItems.begin(), receivedItems.end(), [itemEntry](std::pair<uint16_t, int16_t>
    //     pair) {
    //         return pair.first == itemEntry.tableId && pair.second == itemEntry.getItemId;
    //     });
    //     if (it != receivedItems.end()) {
    //         receivedItems.erase(it);
    //         return;
    //     }

    //     if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded())
    //     return;

    //     nlohmann::json payload;

    //     payload["type"] = "GIVE_ITEM";
    //     payload["modId"] = itemEntry.tableId;
    //     payload["getItemId"] = itemEntry.getItemId;

    //     GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
    // });
    // GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagSet>([](int16_t sceneNum, int16_t flagType,
    // int16_t flag) {
    //     if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded())
    //     return; nlohmann::json payload;

    //     payload["type"] = "SET_SCENE_FLAG";
    //     payload["sceneNum"] = sceneNum;
    //     payload["flagType"] = flagType;
    //     payload["flag"] = flag;
    //     payload["quiet"] = true;

    //     GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
    // });
    // GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagSet>([](int16_t flagType, int16_t flag) {
    //     if (flagType == FLAG_INF_TABLE && flag == INFTABLE_SWORDLESS) {
    //         return;
    //     }

    //     if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded())
    //     return; nlohmann::json payload;

    //     payload["type"] = "SET_FLAG";
    //     payload["flagType"] = flagType;
    //     payload["flag"] = flag;
    //     payload["quiet"] = true;

    //     GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
    // });
    // GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagUnset>([](int16_t sceneNum, int16_t
    // flagType, int16_t flag) {
    //     if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded())
    //     return; nlohmann::json payload;

    //     payload["type"] = "UNSET_SCENE_FLAG";
    //     payload["sceneNum"] = sceneNum;
    //     payload["flagType"] = flagType;
    //     payload["flag"] = flag;
    //     payload["quiet"] = true;

    //     GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
    // });
    // GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagUnset>([](int16_t flagType, int16_t flag) {
    //     if (flagType == FLAG_INF_TABLE && flag == INFTABLE_SWORDLESS) {
    //         return;
    //     }

    //     if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded())
    //     return; nlohmann::json payload;

    //     payload["type"] = "UNSET_FLAG";
    //     payload["flagType"] = flagType;
    //     payload["flag"] = flag;
    //     payload["quiet"] = true;

    //     GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
    // });
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorUpdate>(ACTOR_PLAYER, [](Actor* actor) {
        uint32_t currentPlayerCount = 0;
        for (auto& [clientId, client] : GameInteractorAnchor::AnchorClients) {
            if (client.sceneNum == gPlayState->sceneId) {
                currentPlayerCount++;
            }
        }
        if (!GameInteractor::Instance->isRemoteInteractorConnected || gPlayState == NULL ||
            !GameInteractor::Instance->IsSaveLoaded()) {
            return;
        }
        Player* player = GET_PLAYER(gPlayState);

        nlohmann::json payload;
        if (currentPlayerCount == 0)
            return;

        // gSaveContext.playerData.bootsType = player->currentBoots;
        // gSaveContext.playerData.shieldType = player->currentShield;
        // gSaveContext.playerData.sheathType = player->sheathType;
        // gSaveContext.playerData.leftHandType = player->leftHandType;
        // gSaveContext.playerData.rightHandType = player->rightHandType;
        // gSaveContext.playerData.tunicType = player->currentTunic;
        // gSaveContext.playerData.faceType = player->actor.shape.face;
        // gSaveContext.playerData.biggoron_broken = gSaveContext.swordHealth <= 0 ? 1 : 0;
        // gSaveContext.playerData.playerAge = gSaveContext.linkAge;
        // gSaveContext.playerData.playerHealth = gSaveContext.health;
        // gSaveContext.playerData.playerHealthCapacity = gSaveContext.healthCapacity;
        // gSaveContext.playerData.playerMagic = gSaveContext.magic;
        // gSaveContext.playerData.playerMagicCapacity = gSaveContext.magicCapacity;
        // gSaveContext.playerData.isPlayerMagicAcquired = gSaveContext.isMagicAcquired;
        // gSaveContext.playerData.isDoubleMagicAcquired = gSaveContext.isDoubleMagicAcquired;
        // gSaveContext.playerData.strengthValue = CUR_UPG_VALUE(UPG_STRENGTH);
        // gSaveContext.playerData.yOffset = player->actor.shape.yOffset;
        // gSaveContext.playerData.currentMask = player->currentMask;
        // gSaveContext.playerData.swordEquipped = gSaveContext.equips.buttonItems[0];
        // gSaveContext.playerData.playerStateFlags1 = player->stateFlags1;
        // gSaveContext.playerData.moveFlags = player->skelAnime.moveFlags;
        // gSaveContext.playerData.unk_6C4 = player->unk_6C4;
        // gSaveContext.playerData.speedXZ = player->actor.speedXZ;
        // gSaveContext.playerData.itemAction = player->itemAction;
        // gSaveContext.playerData.unk_85C = player->unk_85C;
        // gSaveContext.playerData.stickWeaponTip = player->meleeWeaponInfo[0].tip;
        // gSaveContext.playerData.unk_860 = player->unk_860;
        // gSaveContext.playerData.unk_862 = player->unk_862;
        PlayerData current;
        current.playerForm = player->transformation;

        payload["playerData"] = current;

        payload["type"] = "CLIENT_UPDATE";
        payload["sceneNum"] = gPlayState->sceneId;
        payload["roomIndex"] = gPlayState->roomCtx.curRoom.num;
        payload["entranceIndex"] = gSaveContext.save.entrance;

        PosRot playerPosRot;
        playerPosRot.pos = player->actor.world.pos;
        playerPosRot.rot = player->actor.shape.rot;
        payload["posRot"] = playerPosRot;

        // std::vector<Vec3s> jointTable = {};
        // for (int i = 0; i < 23; i++) {
        //     jointTable.push_back(player->skelAnime.jointTable[i]);
        // }

        // payload["jointTable"] = jointTable;
        payload["quiet"] = true;

        for (auto& [clientId, client] : GameInteractorAnchor::AnchorClients) {
            if (client.sceneNum == gPlayState->sceneId) {
                payload["targetClientId"] = clientId;
                GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
            }
        }

        // gSaveContext.playerData.damageEffect = 0;
        // gSaveContext.playerData.damageValue = 0;
        // gSaveContext.playerData.playerSound = 0;
    });
}

// void Anchor_EntranceDiscovered(uint16_t entranceIndex) {
//     if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;

//     // If the entrance exists in discoveredEntrances, remove it from the list and don't emit the packet
//     auto it = std::find(discoveredEntrances.begin(), discoveredEntrances.end(), entranceIndex);
//     if (it != discoveredEntrances.end()) {
//         discoveredEntrances.erase(it);
//         return;
//     }

//     nlohmann::json payload;

//     payload["type"] = "ENTRANCE_DISCOVERED";
//     payload["entranceIndex"] = entranceIndex;

//     GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
// }

// void Anchor_UpdateCheckData(uint32_t locationIndex) {
//     if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;

//     nlohmann::json payload;

//     payload["type"] = "UPDATE_CHECK_DATA";
//     payload["locationIndex"] = locationIndex;
//     payload["checkData"] = gSaveContext.checkTrackerData[locationIndex];
//     if (gSaveContext.checkTrackerData[locationIndex].status == RCSHOW_COLLECTED) {
//         payload["checkData"]["status"] = RCSHOW_SAVED;
//     }

//     GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
// }

// void Anchor_UpdateBeansBought(uint8_t amount) {
//     if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;

//     nlohmann::json payload;

//     payload["type"] = "UPDATE_BEANS_BOUGHT";
//     payload["amount"] = amount;

//     GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
// }

// void Anchor_UpdateBeansCount(uint8_t amount) {
//     if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;

//     nlohmann::json payload;

//     payload["type"] = "UPDATE_BEANS_COUNT";
//     payload["amount"] = amount;

//     GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
// }

// void Anchor_ConsumeAdultTradeItem(uint8_t itemId) {
//     if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;

//     nlohmann::json payload;

//     payload["type"] = "CONSUME_ADULT_TRADE_ITEM";
//     payload["itemId"] = itemId;

//     GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
// }

// void Anchor_UpdateKeyCount(int16_t sceneNum, int8_t amount) {
//     if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;

//     nlohmann::json payload;

//     payload["type"] = "UPDATE_KEY_COUNT";
//     payload["sceneNum"] = sceneNum;
//     payload["amount"] = amount;

//     GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
// }

// void Anchor_GiveDungeonItem(int16_t sceneNum, uint16_t itemId) {
//     if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;

//     nlohmann::json payload;

//     payload["type"] = "GIVE_DUNGEON_ITEM";
//     payload["sceneNum"] = sceneNum;
//     payload["itemId"] = itemId;

//     GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
// }

// void Anchor_DamagePlayer(uint32_t actorIndex, u8 damageEffect, u8 damageValue) {
//     if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded() ||
//         actorIndex >= GameInteractorAnchor::ActorIndexToClientId.size()) return;

//     uint32_t clientId = GameInteractorAnchor::ActorIndexToClientId[actorIndex];
//     nlohmann::json payload;

//     payload["type"] = "DAMAGE_PLAYER";
//     payload["damageEffect"] = damageEffect;
//     payload["damageValue"] = damageValue;
//     payload["targetClientId"] = clientId;

//     GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
// }

// void Anchor_GameComplete() {
//     if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;

//     nlohmann::json payload;

//     payload["type"] = "GAME_COMPLETE";

//     GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
//     Anchor_SendClientData();
// }

// void Anchor_RequestTeleport(uint32_t clientId) {
//     if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;

//     nlohmann::json payload;

//     payload["type"] = "REQUEST_TELEPORT";
//     payload["targetClientId"] = clientId;

//     GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
// }

// void Anchor_TeleportToPlayer(uint32_t clientId) {
//     if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;
//     Player* player = GET_PLAYER(gPlayState);

//     nlohmann::json payload;

//     payload["type"] = "TELEPORT_TO";
//     payload["targetClientId"] = clientId;
//     payload["entranceIndex"] = gSaveContext.entranceIndex;
//     payload["roomIndex"] = gPlayState->roomCtx.curRoom.num;
//     payload["posRot"] = player->actor.world;

//     GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
// }

////////////////

// const ImVec4 GRAY = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
// const ImVec4 WHITE = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
// const ImVec4 GREEN = ImVec4(0.5f, 1.0f, 0.5f, 1.0f);

void AnchorPlayerLocationWindow::DrawElement() {
    ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);
    ImGui::Begin("AnchorPlayerLocationWindow", &mIsVisible,
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing |
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);

    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", CVarGetString("gRemote.AnchorName", ""));
    if (gPlayState != NULL) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1), "%s", GetSceneName(gPlayState->sceneId).c_str());
    }
    for (auto& [clientId, client] : GameInteractorAnchor::AnchorClients) {
        ImGui::PushID(clientId);
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", client.name.c_str());
        if (client.sceneNum < SCENE_MAX) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1), "%s", GetSceneName(client.sceneNum).c_str());
            if (gPlayState != NULL && client.sceneNum != SCENE_MAX) {
                ImGui::SameLine();
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                if (ImGui::Button(ICON_FA_CHEVRON_RIGHT,
                                  ImVec2(ImGui::GetFontSize() * 1.0f, ImGui::GetFontSize() * 1.0f))) {
                    // Play_SetRespawnData(gPlayState, RESPAWN_MODE_DOWN, client.entranceIndex, client.roomIndex, 0xDFF,
                    // &client.posRot.pos, client.posRot.rot.y); Play_TriggerVoidOut(gPlayState);
                }
                ImGui::PopStyleVar();
            }
        }
        ImGui::PopID();
    }

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);
}

void AnchorLogWindow::DrawElement() {
    ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);
    ImGui::Begin("AnchorLogWindow", &mIsVisible,
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing |
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);

    // Options to stack notifications on top or bottom, and left or right
    if (ImGui::Button(CVarGetInteger("gRemote.AnchorLogWindowX", 1) ? ICON_FA_CHEVRON_RIGHT : ICON_FA_CHEVRON_LEFT,
                      ImVec2(20, 20))) {
        CVarSetInteger("gRemote.AnchorLogWindowX", !CVarGetInteger("gRemote.AnchorLogWindowX", 1));
    }
    ImGui::SameLine();
    if (ImGui::Button(CVarGetInteger("gRemote.AnchorLogWindowY", 1) ? ICON_FA_CHEVRON_DOWN : ICON_FA_CHEVRON_UP,
                      ImVec2(20, 20))) {
        CVarSetInteger("gRemote.AnchorLogWindowY", !CVarGetInteger("gRemote.AnchorLogWindowY", 1));
    }

    // Store x/y position of window
    ImVec2 anchorPos = ImGui::GetWindowPos();
    ImVec2 anchorSize = ImGui::GetWindowSize();

    for (int index = 0; index < anchorMessages.size(); ++index) {
        auto& message = anchorMessages[index];
        int inverseIndex = -ABS(index - (anchorMessages.size() - 1));
        ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
        if (message.remainingTime < 4.0f) {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, (message.remainingTime - 1) / 3.0f);
        } else {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
        }
        ImGui::Begin(("anchorLog" + std::to_string(message.id)).c_str(), nullptr,
                     ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing |
                         ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                         ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoScrollbar);
        ImGui::SetWindowPos(ImVec2(
            // X position should take into account both the alignment and the width of the message window
            anchorPos.x +
                (CVarGetInteger("gRemote.AnchorLogWindowX", 1) ? 0 : -(ImGui::GetWindowSize().x - anchorSize.x)),
            // Y Position should take into account the stack direction and index of the message
            anchorPos.y + (CVarGetInteger("gRemote.AnchorLogWindowY", 1)
                               ? (anchorSize.y + (ImGui::GetWindowSize().y * inverseIndex))
                               : -(ImGui::GetWindowSize().y * (inverseIndex + 1)))));
        ImGui::SetWindowFontScale(1.8f);

        if (message.itemIcon != nullptr) {
            ImGui::Image(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(message.itemIcon),
                         ImVec2(24, 24));
            ImGui::SameLine();
        }
        if (!message.prefix.empty()) {
            ImGui::TextColored(message.prefixColor, "%s", message.prefix.c_str());
            ImGui::SameLine();
        }
        ImGui::TextColored(message.messageColor, "%s", message.message.c_str());
        if (!message.suffix.empty()) {
            ImGui::SameLine();
            ImGui::TextColored(message.suffixColor, "%s", message.suffix.c_str());
        }
        ImGui::End();
        ImGui::PopStyleVar();

        // decrement remainingTime
        message.remainingTime -= ImGui::GetIO().DeltaTime;

        // remove message if it has expired
        if (message.remainingTime <= 0) {
            anchorMessages.erase(anchorMessages.begin() + index);
            --index;
        }
    }

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);
}
