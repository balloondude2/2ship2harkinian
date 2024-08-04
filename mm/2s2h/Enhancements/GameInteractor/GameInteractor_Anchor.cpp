
// TODOs: send and receive flags/items; update ben overlay; clean up organization;
// nametags from Archez's PR

#include "GameInteractor_Anchor.h"
#include <libultraship/libultraship.h>
//#include <soh/Enhancements/item-tables/ItemTableManager.h>
//#include <soh/Enhancements/randomizer/randomizerTypes.h>
//#include <soh/Enhancements/randomizer/adult_trade_shuffle.h>
//#include <soh/Enhancements/nametag.h>
//#include <soh/util.h>
#include <nlohmann/json.hpp>
#include "2s2h/BenJsonConversions.hpp"

// copied from z_sram_nes. Can probably just use the struct there, but I was getting linker errors
typedef struct PersistentCycleSceneFlags {
    /* 0x0 */ u32 switch0;
    /* 0x4 */ u32 switch1;
    /* 0x8 */ u32 chest;
    /* 0xC */ u32 collectible;
} PersistentCycleSceneFlags; // size = 0x10

extern "C" {
#include <variables.h>
#include "macros.h"
#include "z64scene.h"
#include "z64actor.h"
#include "functions.h"
// extern "C" s16 gEnLinkPuppetId;
extern GameState* gGameState;
extern PlayState* gPlayState;
extern SaveContext gSaveContext;
extern TexturePtr gItemIcons[131];
extern u16 sPersistentCycleWeekEventRegs[ARRAY_COUNT(gSaveContext.save.saveInfo.weekEventReg)];
extern PersistentCycleSceneFlags sPersistentCycleSceneFlags[SCENE_MAX];
extern u8 Item_GiveImpl(PlayState* play, u8 item);
}

// TODO: Don't hardcode this, maybe move en_ben from actor_table. soh has actorDB
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
        { "chest", flags.chest },
        { "switch0", flags.switch0 },
        { "switch1", flags.switch1 },
        { "clearedRoom", flags.clearedRoom },
        { "collectible", flags.collectible },
    };
}

void from_json(const json& j, CycleSceneFlags& flags) {
    j.at("chest").get_to(flags.chest);
    j.at("switch0").get_to(flags.switch0);
    j.at("switch1").get_to(flags.switch1);
    j.at("clearedRoom").get_to(flags.clearedRoom);
    j.at("collectible").get_to(flags.collectible);
}

std::vector<std::string> itemNames = {
    /* 0x00 */ "ITEM_OCARINA_OF_TIME",
    /* 0x01 */ "ITEM_BOW",
    /* 0x02 */ "ITEM_ARROW_FIRE",
    /* 0x03 */ "ITEM_ARROW_ICE",
    /* 0x04 */ "ITEM_ARROW_LIGHT",
    /* 0x05 */ "ITEM_OCARINA_FAIRY",
    /* 0x06 */ "ITEM_BOMB",
    /* 0x07 */ "ITEM_BOMBCHU",
    /* 0x08 */ "ITEM_DEKU_STICK",
    /* 0x09 */ "ITEM_DEKU_NUT",
    /* 0x0A */ "ITEM_MAGIC_BEANS",
    /* 0x0B */ "ITEM_SLINGSHOT",
    /* 0x0C */ "ITEM_POWDER_KEG",
    /* 0x0D */ "ITEM_PICTOGRAPH_BOX",
    /* 0x0E */ "ITEM_LENS_OF_TRUTH",
    /* 0x0F */ "ITEM_HOOKSHOT",
    /* 0x10 */ "ITEM_SWORD_GREAT_FAIRY",
    /* 0x11 */ "ITEM_LONGSHOT", // OoT Leftover
    /* 0x12 */ "ITEM_BOTTLE",
    /* 0x13 */ "ITEM_POTION_RED",
    /* 0x14 */ "ITEM_POTION_GREEN",
    /* 0x15 */ "ITEM_POTION_BLUE",
    /* 0x16 */ "ITEM_FAIRY",
    /* 0x17 */ "ITEM_DEKU_PRINCESS",
    /* 0x18 */ "ITEM_MILK_BOTTLE",
    /* 0x19 */ "ITEM_MILK_HALF",
    /* 0x1A */ "ITEM_FISH",
    /* 0x1B */ "ITEM_BUG",
    /* 0x1C */ "ITEM_BLUE_FIRE",
    /* 0x1D */ "ITEM_POE",
    /* 0x1E */ "ITEM_BIG_POE",
    /* 0x1F */ "ITEM_SPRING_WATER",
    /* 0x20 */ "ITEM_HOT_SPRING_WATER",
    /* 0x21 */ "ITEM_ZORA_EGG",
    /* 0x22 */ "ITEM_GOLD_DUST",
    /* 0x23 */ "ITEM_MUSHROOM",
    /* 0x24 */ "ITEM_SEAHORSE",
    /* 0x25 */ "ITEM_CHATEAU",
    /* 0x26 */ "ITEM_HYLIAN_LOACH",
    /* 0x27 */ "ITEM_OBABA_DRINK",
    /* 0x28 */ "ITEM_MOONS_TEAR",
    /* 0x29 */ "ITEM_DEED_LAND",
    /* 0x2A */ "ITEM_DEED_SWAMP",
    /* 0x2B */ "ITEM_DEED_MOUNTAIN",
    /* 0x2C */ "ITEM_DEED_OCEAN",
    /* 0x2D */ "ITEM_ROOM_KEY",
    /* 0x2E */ "ITEM_LETTER_MAMA",
    /* 0x2F */ "ITEM_LETTER_TO_KAFEI",
    /* 0x30 */ "ITEM_PENDANT_OF_MEMORIES",
    /* 0x31 */ "ITEM_TINGLE_MAP",
    /* 0x32 */ "ITEM_MASK_DEKU",
    /* 0x33 */ "ITEM_MASK_GORON",
    /* 0x34 */ "ITEM_MASK_ZORA",
    /* 0x35 */ "ITEM_MASK_FIERCE_DEITY",
    /* 0x36 */ "ITEM_MASK_TRUTH",
    /* 0x37 */ "ITEM_MASK_KAFEIS_MASK",
    /* 0x38 */ "ITEM_MASK_ALL_NIGHT",
    /* 0x39 */ "ITEM_MASK_BUNNY",
    /* 0x3A */ "ITEM_MASK_KEATON",
    /* 0x3B */ "ITEM_MASK_GARO",
    /* 0x3C */ "ITEM_MASK_ROMANI",
    /* 0x3D */ "ITEM_MASK_CIRCUS_LEADER",
    /* 0x3E */ "ITEM_MASK_POSTMAN",
    /* 0x3F */ "ITEM_MASK_COUPLE",
    /* 0x40 */ "ITEM_MASK_GREAT_FAIRY",
    /* 0x41 */ "ITEM_MASK_GIBDO",
    /* 0x42 */ "ITEM_MASK_DON_GERO",
    /* 0x43 */ "ITEM_MASK_KAMARO",
    /* 0x44 */ "ITEM_MASK_CAPTAIN",
    /* 0x45 */ "ITEM_MASK_STONE",
    /* 0x46 */ "ITEM_MASK_BREMEN",
    /* 0x47 */ "ITEM_MASK_BLAST",
    /* 0x48 */ "ITEM_MASK_SCENTS",
    /* 0x49 */ "ITEM_MASK_GIANT",
    /* 0x4A */ "ITEM_BOW_FIRE",
    /* 0x4B */ "ITEM_BOW_ICE",
    /* 0x4C */ "ITEM_BOW_LIGHT",
    /* 0x4D */ "ITEM_SWORD_KOKIRI",
    /* 0x4E */ "ITEM_SWORD_RAZOR",
    /* 0x4F */ "ITEM_SWORD_GILDED",
    /* 0x50 */ "ITEM_SWORD_DEITY",
    /* 0x51 */ "ITEM_SHIELD_HERO",
    /* 0x52 */ "ITEM_SHIELD_MIRROR",
    /* 0x53 */ "ITEM_QUIVER_30",
    /* 0x54 */ "ITEM_QUIVER_40",
    /* 0x55 */ "ITEM_QUIVER_50",
    /* 0x56 */ "ITEM_BOMB_BAG_20",
    /* 0x57 */ "ITEM_BOMB_BAG_30",
    /* 0x58 */ "ITEM_BOMB_BAG_40",
    /* 0x59 */ "ITEM_WALLET_DEFAULT",
    /* 0x5A */ "ITEM_WALLET_ADULT",
    /* 0x5B */ "ITEM_WALLET_GIANT",
    /* 0x5C */ "ITEM_FISHING_ROD",
    /* 0x5D */ "ITEM_REMAINS_ODOLWA",
    /* 0x5E */ "ITEM_REMAINS_GOHT",
    /* 0x5F */ "ITEM_REMAINS_GYORG",
    /* 0x60 */ "ITEM_REMAINS_TWINMOLD",
    /* 0x61 */ "ITEM_SONG_SONATA",
    /* 0x62 */ "ITEM_SONG_LULLABY",
    /* 0x63 */ "ITEM_SONG_NOVA",
    /* 0x64 */ "ITEM_SONG_ELEGY",
    /* 0x65 */ "ITEM_SONG_OATH",
    /* 0x66 */ "ITEM_SONG_SARIA",
    /* 0x67 */ "ITEM_SONG_TIME",
    /* 0x68 */ "ITEM_SONG_HEALING",
    /* 0x69 */ "ITEM_SONG_EPONA",
    /* 0x6A */ "ITEM_SONG_SOARING",
    /* 0x6B */ "ITEM_SONG_STORMS",
    /* 0x6C */ "ITEM_SONG_SUN",
    /* 0x6D */ "ITEM_BOMBERS_NOTEBOOK",
    /* 0x6E */ "ITEM_SKULL_TOKEN",
    /* 0x6F */ "ITEM_HEART_CONTAINER",
    /* 0x70 */ "ITEM_HEART_PIECE",
    /* 0x71 */ "ITEM_71",
    /* 0x72 */ "ITEM_72",
    /* 0x73 */ "ITEM_SONG_LULLABY_INTRO",
    /* 0x74 */ "ITEM_KEY_BOSS",
    /* 0x75 */ "ITEM_COMPASS",
    /* 0x76 */ "ITEM_DUNGEON_MAP",
    /* 0x77 */ "ITEM_STRAY_FAIRIES",
    /* 0x78 */ "ITEM_KEY_SMALL",
    /* 0x79 */ "ITEM_MAGIC_JAR_SMALL",
    /* 0x7A */ "ITEM_MAGIC_JAR_BIG",
    /* 0x7B */ "ITEM_HEART_PIECE_2",
    /* 0x7C */ "ITEM_INVALID_1",
    /* 0x7D */ "ITEM_INVALID_2",
    /* 0x7E */ "ITEM_INVALID_3",
    /* 0x7F */ "ITEM_INVALID_4",
    /* 0x80 */ "ITEM_INVALID_5",
    /* 0x81 */ "ITEM_INVALID_6",
    /* 0x82 */ "ITEM_INVALID_7",
    /* 0x83 */ "ITEM_RECOVERY_HEART",
    /* 0x84 */ "ITEM_RUPEE_GREEN",
    /* 0x85 */ "ITEM_RUPEE_BLUE",
    /* 0x86 */ "ITEM_RUPEE_10",
    /* 0x87 */ "ITEM_RUPEE_RED",
    /* 0x88 */ "ITEM_RUPEE_PURPLE",
    /* 0x89 */ "ITEM_RUPEE_SILVER",
    /* 0x8A */ "ITEM_RUPEE_HUGE",
    /* 0x8B */ "ITEM_DEKU_STICKS_5",
    /* 0x8C */ "ITEM_DEKU_STICKS_10",
    /* 0x8D */ "ITEM_DEKU_NUTS_5",
    /* 0x8E */ "ITEM_DEKU_NUTS_10",
    /* 0x8F */ "ITEM_BOMBS_5",
    /* 0x90 */ "ITEM_BOMBS_10",
    /* 0x91 */ "ITEM_BOMBS_20",
    /* 0x92 */ "ITEM_BOMBS_30",
    /* 0x93 */ "ITEM_ARROWS_10",
    /* 0x94 */ "ITEM_ARROWS_30",
    /* 0x95 */ "ITEM_ARROWS_40",
    /* 0x96 */ "ITEM_ARROWS_50",
    /* 0x97 */ "ITEM_BOMBCHUS_20",
    /* 0x98 */ "ITEM_BOMBCHUS_10",
    /* 0x99 */ "ITEM_BOMBCHUS_1",
    /* 0x9A */ "ITEM_BOMBCHUS_5",
    /* 0x9B */ "ITEM_DEKU_STICK_UPGRADE_20",
    /* 0x9C */ "ITEM_DEKU_STICK_UPGRADE_30",
    /* 0x9D */ "ITEM_DEKU_NUT_UPGRADE_30",
    /* 0x9E */ "ITEM_DEKU_NUT_UPGRADE_40",
    /* 0x9F */ "ITEM_CHATEAU_2",
    /* 0xA0 */ "ITEM_MILK",
    /* 0xA1 */ "ITEM_GOLD_DUST_2",
    /* 0xA2 */ "ITEM_HYLIAN_LOACH_2",
    /* 0xA3 */ "ITEM_SEAHORSE_CAUGHT",
    // First entries of `MAP_POINT` must be continguous with `RegionId`
    /* 0xA4 */ "ITEM_MAP_POINT_GREAT_BAY",
    /* 0xA5 */ "ITEM_MAP_POINT_ZORA_HALL",
    /* 0xA6 */ "ITEM_MAP_POINT_ROMANI_RANCH",
    /* 0xA7 */ "ITEM_MAP_POINT_DEKU_PALACE",
    /* 0xA8 */ "ITEM_MAP_POINT_WOODFALL",
    /* 0xA9 */ "ITEM_MAP_POINT_CLOCK_TOWN",
    /* 0xAA */ "ITEM_MAP_POINT_SNOWHEAD",
    /* 0xAB */ "ITEM_MAP_POINT_IKANA_GRAVEYARD",
    /* 0xAC */ "ITEM_MAP_POINT_IKANA_CANYON",
    /* 0xAD */ "ITEM_MAP_POINT_GORON_VILLAGE",
    /* 0xAE */ "ITEM_MAP_POINT_STONE_TOWER",
    // Remaining map points are unique to owl warps
    /* 0xAF */ "ITEM_MAP_POINT_GREAT_BAY_COAST",
    /* 0xBO */ "ITEM_MAP_POINT_SOUTHERN_SWAMP",
    /* 0xB1 */ "ITEM_MAP_POINT_MOUNTAIN_VILLAGE",
    /* 0xB2 */ "ITEM_MAP_POINT_MILK_ROAD",
    /* 0xB3 */ "ITEM_MAP_POINT_ZORA_CAPE",
    /* 0xB4 */ "ITEM_B4",
    /* 0xB5 */ "ITEM_B5",
    /* 0xB6 */ "ITEM_B6",
    /* 0xB7 */ "ITEM_B7",
    /* 0xB8 */ "ITEM_B8",
    /* 0xB9 */ "ITEM_B9",
    /* 0xBA */ "ITEM_BA",
    /* 0xBB */ "ITEM_BB",
    /* 0xBC */ "ITEM_BC",
    /* 0xBD */ "ITEM_BD",
    /* 0xBE */ "ITEM_BE",
    /* 0xBF */ "ITEM_BF",
    /* 0xC0 */ "ITEM_C0",
    /* 0xC1 */ "ITEM_C1",
    /* 0xC2 */ "ITEM_C2",
    /* 0xC3 */ "ITEM_C3",
    /* 0xC4 */ "ITEM_C4",
    /* 0xC5 */ "ITEM_C5",
    /* 0xC6 */ "ITEM_C6",
    /* 0xC7 */ "ITEM_C7",
    /* 0xC8 */ "ITEM_C8",
    /* 0xC9 */ "ITEM_C9",
    /* 0xCA */ "ITEM_CA",
    /* 0xCB */ "ITEM_CB",
    /* 0xCC */ "ITEM_CC",
    /* 0xCD */ "ITEM_CD",
    /* 0xCE */ "ITEM_CE",
    /* 0xCF */ "ITEM_CF",
    /* 0xD0 */ "ITEM_D0",
    /* 0xD1 */ "ITEM_D1",
    /* 0xD2 */ "ITEM_D2",
    /* 0xD3 */ "ITEM_D3",
    /* 0xD4 */ "ITEM_D4",
    /* 0xD5 */ "ITEM_D5",
    /* 0xD6 */ "ITEM_D6",
    /* 0xD7 */ "ITEM_D7",
    /* 0xD8 */ "ITEM_D8",
    /* 0xD9 */ "ITEM_D9",
    /* 0xDA */ "ITEM_DA",
    /* 0xDB */ "ITEM_DB",
    /* 0xDC */ "ITEM_DC",
    /* 0xDD */ "ITEM_DD",
    /* 0xDE */ "ITEM_DE",
    /* 0xDF */ "ITEM_DF",
    /* 0xE0 */ "ITEM_E0",
    /* 0xE1 */ "ITEM_E1",
    /* 0xE2 */ "ITEM_E2",
    /* 0xE3 */ "ITEM_E3",
    /* 0xE4 */ "ITEM_E4",
    /* 0xE5 */ "ITEM_E5",
    /* 0xE6 */ "ITEM_E6",
    /* 0xE7 */ "ITEM_E7",
    /* 0xE8 */ "ITEM_E8",
    /* 0xE9 */ "ITEM_E9",
    /* 0xEA */ "ITEM_EA",
    /* 0xEB */ "ITEM_EB",
    /* 0xEC */ "ITEM_EC",
    /* 0xED */ "ITEM_ED",
    /* 0xEE */ "ITEM_EE",
    /* 0xEF */ "ITEM_EF",
    /* 0xF0 */ "ITEM_F0"  // PLAYER_MASK_BLAST"
    /* 0xF1 */ "ITEM_F1", // PLAYER_MASK_BREMEN"
    /* 0xF2 */ "ITEM_F2", // PLAYER_MASK_KAMARO"
    /* 0xF3 */ "ITEM_F3",
    /* 0xF4 */ "ITEM_F4",
    /* 0xF5 */ "ITEM_F5",
    /* 0xF6 */ "ITEM_F6",
    /* 0xF7 */ "ITEM_F7",
    /* 0xF8 */ "ITEM_F8",
    /* 0xF9 */ "ITEM_F9",
    /* 0xFA */ "ITEM_FA",
    /* 0xFB */ "ITEM_FB",
    /* 0xFC */ "ITEM_FC",
    /* 0xFD */ "ITEM_FD",
    /* 0xFE */ "ITEM_FE",
    /* 0xFF */ "ITEM_NONE",
};

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

const std::string& GetItemName(uint8_t item) {
    return itemNames[item];
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

    // TODO: Doesn't crash when Anchor is enabled at start, but needs reconnect to work. Maybe call this function again
    // on game load
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

bool WeekEventReg_Persistance(u32 flag) {
    u16 persistBits = sPersistentCycleWeekEventRegs[flag >> 8];
    u16 shifted_flag_bits = 3 << (2 * BIT_FLAG_TO_SHIFT(flag & 0xFF));
    return persistBits & shifted_flag_bits;
}

bool SceneFlag_Persistance(int16_t sceneNum, FlagType flagType, uint32_t flag) {
    // Only switch0, switch1, chest, and collectible persist
    // ClearedRoom might "persist" through permanentSceneFlags. unk_14 and rooms seem to be set to 0 on cycle reset
    switch (flagType) {
        case FLAG_CYCL_SCENE_CHEST:
            return (Flags_GetTreasure(gPlayState, flag) & sPersistentCycleSceneFlags[sceneNum].chest);
        case FLAG_CYCL_SCENE_SWITCH:
            if ((flag & ~0x1F) >> 5 == 0) {
                return (Flags_GetSwitch(gPlayState, flag) & sPersistentCycleSceneFlags[sceneNum].switch0);
            } else if ((flag & ~0x1F) >> 5 == 1) {
                return (Flags_GetSwitch(gPlayState, flag) & sPersistentCycleSceneFlags[sceneNum].switch1);
            }
            break;
        case FLAG_CYCL_SCENE_COLLECTIBLE:
            return (Flags_GetCollectible(gPlayState, flag) & sPersistentCycleSceneFlags[sceneNum].collectible);
        default:
            break;
    }
    return false;
}

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

    if (payload["type"] == "GIVE_ITEM") {
        // add item to inventory
        // use proxysaw's demo a few weeks ago


        AnchorClient anchorClient = GameInteractorAnchor::AnchorClients[payload["clientId"].get<uint32_t>()];
        int16_t item = payload["getItemId"].get<int16_t>();
        std::string itemString = GetItemName(item);
        

    
        // TODO: Look into quest items and dungeon items (songs and small keys)
        // Pretty sure Item_GiveImpl will try to add small key to current scene, not correct dungeon
        // Map is given but doesn't display on other clients
        // Magic not given, I assume same with double magic and double defense.
        // Might need to hook into z_bg_dy_yoseizo.c line:325ish
        Item_GiveImpl(gPlayState, item);

        if (item <= 0x82) {
            Anchor_DisplayMessage({ .itemIcon = (const char*)gItemIcons[item],
                                .prefix = itemString,
                                .message = "from",
                                .suffix = anchorClient.name });
        } else {
            Anchor_DisplayMessage({ // .itemIcon = (const char*)gItemIcons[item],
                                .prefix = itemString,
                                .message = "from",
                                .suffix = anchorClient.name });
        }

    }

    if (payload["type"] == "SET_SCENE_FLAG") {

        s16 sceneId = payload["sceneNum"].get<int16_t>();
        u32 flagType = payload["flagType"].get<uint32_t>();
        u32 flag = payload["flag"].get<uint32_t>();

        switch (flagType) {
            case FLAG_NONE:
            case FLAG_WEEK_EVENT_REG:
            case FLAG_EVENT_INF:
            case FLAG_SCENES_VISIBLE:
            case FLAG_OWL_ACTIVATION:
                // not scene flags
                break;
            case FLAG_PERM_SCENE_CHEST:
            case FLAG_PERM_SCENE_SWITCH:
            case FLAG_PERM_SCENE_CLEARED_ROOM:
            case FLAG_PERM_SCENE_COLLECTIBLE:
            case FLAG_PERM_SCENE_UNK_14:
            case FLAG_PERM_SCENE_ROOMS:
                // scene flag hook never sends PERM flags
                break;
            case FLAG_CYCL_SCENE_CHEST:
                if (gPlayState->sceneId == sceneId) {
                    // Flags_SetTreasure triggers hook, which we don't want
                    // Flags_SetTreasure(gPlayState, flag);
                    gPlayState->actorCtx.sceneFlags.chest |= (1 << flag);
                } else {
                    gSaveContext.cycleSceneFlags[sceneId].chest |= (1 << flag);
                }
                break;
            case FLAG_CYCL_SCENE_SWITCH:
                if (gPlayState->sceneId == sceneId) {
                    // Flags_SetSwitch(gPlayState, flag);
                    if ((flag > SWITCH_FLAG_NONE) && (flag < 0x80)) {
                        gPlayState->actorCtx.sceneFlags.switches[(flag & ~0x1F) >> 5] |= 1 << (flag & 0x1F);
                    }
                } else {
                    if ((flag & ~0x1F) >> 5 == 0) {
                        gSaveContext.cycleSceneFlags[sceneId].switch0 |= (1 << (flag & 0x1F));
                    } else if ((flag & ~0x1F) >> 5 == 1) {
                        gSaveContext.cycleSceneFlags[sceneId].switch1 |= (1 << (flag & 0x1F));
                    } else {
                        // It looks like temporary switch (switch[2] and [3]) data is stored in respawn stuff?
                        // LUSLOG_DEBUG("Nothing happend with flag index: %x", (flag & ~0x1F) >> 5);
                    }
                }
                break;
            case FLAG_CYCL_SCENE_CLEARED_ROOM:
                // what's the difference between clear and clearTemp?
                // currently not sending any cleared room flags
                if (gPlayState->sceneId == sceneId) {
                    Flags_SetClear(gPlayState, flag);
                } else {
                    gSaveContext.cycleSceneFlags[sceneId].clearedRoom |= (1 << flag);
                }
                break;
            case FLAG_CYCL_SCENE_COLLECTIBLE:
                if (gPlayState->sceneId == sceneId) {
                    // Flags_SetCollectible(gPlayState, flag);
                    if ((flag > 0) && (flag < 0x80)) {
                        gPlayState->actorCtx.sceneFlags.collectible[(flag & ~0x1F) >> 5] |= 1 << (flag & 0x1F);
                    }

                    // TODO: Are there any other actor categories to check and kill?
                    //  Check Boss remains
                    Actor* actor = gPlayState->actorCtx.actorLists[ACTORCAT_MISC].first;
                    while (actor != NULL) {
                        // Probably should check that actorId = 0x0E before casting to EnItem00
                        EnItem00* item = ((EnItem00*)actor);
                        if (item->collectibleFlag == flag) {
                            Actor_Kill(actor);
                        }
                        actor = actor->next;
                    }
                } else {
                    gSaveContext.cycleSceneFlags[sceneId].collectible |= (1 << (flag & 0x1F));
                }
                break;
            default:
                break;
        }

        // AnchorClient anchorClient = GameInteractorAnchor::AnchorClients[payload["clientId"].get<uint32_t>()];
        std::string s1 = std::to_string(sceneId);
        std::string s2 = std::to_string(flagType);
        std::string s3 = std::to_string(flag);

        Anchor_DisplayMessage({ //.prefix = payload["getItemId"].get<int16_t>(),
                                .prefix = "set scene flag",
                                .message = s2,
                                .suffix = s3 });
    }
    if (payload["type"] == "SET_FLAG") {
        // s16 sceneId = payload["sceneNum"].get<int16_t>();
        u32 flagType = payload["flagType"].get<uint32_t>();
        u32 flag = payload["flag"].get<uint32_t>();
        switch (flagType) {
            case FLAG_NONE:
                break;
            case FLAG_WEEK_EVENT_REG:
                Flags_SetWeekEventReg(flag);
                break;
            case FLAG_EVENT_INF:
                // do we ever want to set any of these?
            case FLAG_SCENES_VISIBLE:
            case FLAG_OWL_ACTIVATION:
                // scene flags never sent with hook
            case FLAG_PERM_SCENE_CHEST:
            case FLAG_PERM_SCENE_SWITCH:
            case FLAG_PERM_SCENE_CLEARED_ROOM:
            case FLAG_PERM_SCENE_COLLECTIBLE:
            case FLAG_PERM_SCENE_UNK_14:
            case FLAG_PERM_SCENE_ROOMS:
            case FLAG_CYCL_SCENE_CHEST:
            case FLAG_CYCL_SCENE_SWITCH:
            case FLAG_CYCL_SCENE_CLEARED_ROOM:
            case FLAG_CYCL_SCENE_COLLECTIBLE:
            default:
                break;
        }

        // AnchorClient anchorClient = GameInteractorAnchor::AnchorClients[payload["clientId"].get<uint32_t>()];
        // std::string s1 = std::to_string(sceneId);
        std::string s2 = std::to_string(flagType);
        std::string s3 = std::to_string(flag);

        Anchor_DisplayMessage({ //.prefix = payload["getItemId"].get<int16_t>(),
                                .prefix = "set flag",
                                .message = s2,
                                .suffix = s3 });
    }
    if (payload["type"] == "UNSET_SCENE_FLAG") {}
    if (payload["type"] == "UNSET_FLAG") {}

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
        }
    }
    if (payload["type"] == "PUSH_SAVE_STATE" && GameInteractor::IsSaveLoaded()) {
        Anchor_ParseSaveStateFromRemote(payload);
    }
    if (payload["type"] == "REQUEST_SAVE_STATE" && GameInteractor::IsSaveLoaded()) {
        Anchor_PushSaveStateToRemote();
    }
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

    if (payload["type"] == "REQUEST_TELEPORT") {
        Anchor_TeleportToPlayer(payload["clientId"].get<uint32_t>());
    }
    if (payload["type"] == "TELEPORT_TO") {
        uint32_t entranceIndex = payload["entranceIndex"].get<uint32_t>();
        uint32_t roomIndex = payload["roomIndex"].get<uint32_t>();
        PosRot posRot = payload["posRot"].get<PosRot>();

        // TODO: Check these parameters
        Play_SetRespawnData(gGameState, RESPAWN_MODE_DOWN, entranceIndex, roomIndex, 0xDFF, &posRot.pos, posRot.rot.y);
        // Play_TriggerVoidOut(gPlayState);
        func_80169EFC(gGameState);
    }
    if (payload["type"] == "SERVER_MESSAGE") {
        Anchor_DisplayMessage({
            .prefix = "Server:",
            .prefixColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f),
            .message = payload["message"].get<std::string>(),
        });
    }
    if (payload["type"] == "DISABLE_ANCHOR") {
        GameInteractor::Instance->isRemoteInteractorEnabled = false;
        GameInteractorAnchor::Instance->isEnabled = false;
    }
    // if (payload["type"] == "RESET") {
    //     std::reinterpret_pointer_cast<LUS::ConsoleWindow>(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))->Dispatch("reset");
    // }
}

void Anchor_PushSaveStateToRemote() {
    // TODO: This sends way more data than needed. Particularly pictoPhoto
    nlohmann::json payload = gSaveContext;
    payload["type"] = "PUSH_SAVE_STATE";

    payload["cycleSceneFlags"] = gSaveContext.cycleSceneFlags;

    // TODO: Probably need to account for inverted stone tower. See Play_SaveCycleSceneFlags() in z_play.c. Here and
    // other places 
    // manually update current scene flags
    payload["cycleSceneFlags"][gPlayState->sceneId]["chest"] = gPlayState->actorCtx.sceneFlags.chest;
    payload["cycleSceneFlags"][gPlayState->sceneId]["switch0"] = gPlayState->actorCtx.sceneFlags.switches[0];
    payload["cycleSceneFlags"][gPlayState->sceneId]["switch1"] = gPlayState->actorCtx.sceneFlags.switches[1];
    payload["cycleSceneFlags"][gPlayState->sceneId]["clearedRoom"] = gPlayState->actorCtx.sceneFlags.clearedRoom;
    payload["cycleSceneFlags"][gPlayState->sceneId]["collectible"] = gPlayState->actorCtx.sceneFlags.collectible[0];

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_RequestSaveStateFromRemote() {
    nlohmann::json payload;
    payload["type"] = "REQUEST_SAVE_STATE";

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_ParseSaveStateFromRemote(nlohmann::json payload) {
    SaveContext loadedData = payload.get<SaveContext>();

    // TODO: Decide what else to sync. Owls, threeDayResetCount, firstCycle, rupees, skulltulas, stolen items, regionsVisited, worldMapCloudVisiblity, etc?

    // from_json for SaveContext doesn't have CycleSceneFlags, so manually parse them
    CycleSceneFlags sceneFlags[120];
    for (int i = 0; i < SCENE_MAX; i++) {
        sceneFlags[i] = payload["cycleSceneFlags"][i].get<CycleSceneFlags>();
    }

    gSaveContext.save.saveInfo.playerData.healthCapacity = loadedData.save.saveInfo.playerData.healthCapacity;
    // TODO: Clean this up. gsSaveContext.magicCapacity isn't included in BenJsonConversions. When added, game crashes
    // on file select
    if (loadedData.save.saveInfo.playerData.magicLevel == 2) {
        gSaveContext.magicCapacity = gSaveContext.save.saveInfo.playerData.magic = MAGIC_DOUBLE_METER;
        gSaveContext.save.saveInfo.playerData.magicLevel = loadedData.save.saveInfo.playerData.magicLevel;
        gSaveContext.save.saveInfo.playerData.isMagicAcquired = loadedData.save.saveInfo.playerData.isMagicAcquired;
        gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired =
            loadedData.save.saveInfo.playerData.isDoubleMagicAcquired;
    } else if (loadedData.save.saveInfo.playerData.magicLevel == 1) {
        gSaveContext.magicCapacity = gSaveContext.save.saveInfo.playerData.magic = MAGIC_NORMAL_METER;
        gSaveContext.save.saveInfo.playerData.magicLevel = loadedData.save.saveInfo.playerData.magicLevel;
        gSaveContext.save.saveInfo.playerData.isMagicAcquired = loadedData.save.saveInfo.playerData.isMagicAcquired;
        gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired =
            loadedData.save.saveInfo.playerData.isDoubleMagicAcquired;
    }

    BUTTON_ITEM_EQUIP(PLAYER_FORM_DEKU, EQUIP_SLOT_B) =
        loadedData.save.saveInfo.equips.buttonItems[PLAYER_FORM_DEKU][EQUIP_SLOT_B];

    gSaveContext.save.saveInfo.playerData.doubleDefense = loadedData.save.saveInfo.playerData.doubleDefense;
    // gSaveContext.bgsFlag = loadedData.bgsFlag;
    // // TODO: Packet to live update this
    // gSaveContext.adultTradeItems = loadedData.adultTradeItems;

    // TODO: I doubt this is right; I think this is better now, but could still use work probs. now on v3
    for (int i = 0; i < SCENE_MAX; i++) {
        gSaveContext.cycleSceneFlags[i].chest = sceneFlags[i].chest;
        gSaveContext.cycleSceneFlags[i].switch0 = sceneFlags[i].switch0;
        gSaveContext.cycleSceneFlags[i].switch1 = sceneFlags[i].switch1;
        gSaveContext.cycleSceneFlags[i].clearedRoom = sceneFlags[i].clearedRoom;
        gSaveContext.cycleSceneFlags[i].collectible = sceneFlags[i].collectible;
        if (gPlayState->sceneId == i) {
            gPlayState->actorCtx.sceneFlags.chest = gSaveContext.cycleSceneFlags[i].chest;
            gPlayState->actorCtx.sceneFlags.switches[0] = gSaveContext.cycleSceneFlags[i].switch0;
            gPlayState->actorCtx.sceneFlags.switches[1] = gSaveContext.cycleSceneFlags[i].switch1;
            gPlayState->actorCtx.sceneFlags.clearedRoom = gSaveContext.cycleSceneFlags[i].clearedRoom;
            gPlayState->actorCtx.sceneFlags.collectible[0] = gSaveContext.cycleSceneFlags[i].collectible;
        }
    }

    for (int i = 0; i < 100; i++) {
        gSaveContext.save.saveInfo.weekEventReg[i] = loadedData.save.saveInfo.weekEventReg[i];
    }

    for (int i = 0; i < 8; i++) {
        gSaveContext.eventInf[i] = loadedData.eventInf[i];
    }

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

    // Set Sword and Sheild
    gSaveContext.save.saveInfo.equips.equipment = loadedData.save.saveInfo.equips.equipment;
    gSaveContext.save.saveInfo.playerData.swordHealth = loadedData.save.saveInfo.playerData.swordHealth;
    BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_B) = loadedData.save.saveInfo.equips.buttonItems[0][EQUIP_SLOT_B];
    Interface_LoadItemIconImpl(gPlayState, EQUIP_SLOT_B);

    // // Restore bottle contents (unless it's ruto's letter)
    // for (int i = 0; i < 4; i++) {
    //     if (gSaveContext.inventory.items[SLOT_BOTTLE_1 + i] != ITEM_NONE &&
    //     gSaveContext.inventory.items[SLOT_BOTTLE_1 + i] != ITEM_LETTER_RUTO) {
    //         loadedData.inventory.items[SLOT_BOTTLE_1 + i] = gSaveContext.inventory.items[SLOT_BOTTLE_1 + i];
    //     }
    // }

    // Restore ammo if it's non-zero,
    for (int i = 0; i < ARRAY_COUNT(gSaveContext.save.saveInfo.inventory.ammo); i++) {
        if (gSaveContext.save.saveInfo.inventory.ammo[i] != 0) {
            loadedData.save.saveInfo.inventory.ammo[i] = gSaveContext.save.saveInfo.inventory.ammo[i];
        }
    }

    // Set day/time
    gSaveContext.save.time = loadedData.save.time;
    gSaveContext.save.day = loadedData.save.day;
    gSaveContext.save.isNight = loadedData.save.isNight;
    gSaveContext.save.timeSpeedOffset = loadedData.save.timeSpeedOffset;

    gSaveContext.save.saveInfo.inventory = loadedData.save.saveInfo.inventory;

    // TODO: Day on clock isn't updating (and probably other things)
    //  Maybe void out?
    func_80169EFC(gGameState);

    gSaveContext.save.saveInfo.playerData.health = gSaveContext.save.saveInfo.playerData.healthCapacity;
    Anchor_DisplayMessage({ .message = "State loaded from remote!" });
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
        auto fairy =
            Actor_Spawn(&gPlayState->actorCtx, gPlayState, gEnLinkPuppetId, client.posRot.pos.x, client.posRot.pos.y,
                        client.posRot.pos.z, client.posRot.rot.x, client.posRot.rot.y, client.posRot.rot.z, 3 + i);
        // Todo: This was removed in player models branch
        // TODO: Add nametag stuff
        // NameTag_RegisterForActor(fairy, client.name.c_str());
        i++;
    }
}

static uint32_t lastSceneNum = SCENE_MAX;

void Anchor_RegisterHooks() {
    // TODO: Might need to change this hook to be after scene commands
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](s16 sceneId, s8 spawnNum) {
        if (gPlayState == NULL || !GameInteractor::Instance->isRemoteInteractorConnected) {
            return;
        }

        // Moved to a new scene
        if (lastSceneNum != gPlayState->sceneId) {
            Anchor_SendClientData();
        }

        if (GameInteractor::Instance->IsSaveLoaded()) {
            // Player loaded into file
            if (lastSceneNum == SCENE_MAX) {
                // TODO: I think this is firing an extra time?
                // yeah, behaves more how I'd expect when commented out
                // Anchor_RequestSaveStateFromRemote();
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

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemGive>([](u8 item) {
        nlohmann::json payload;

        payload["type"] = "GIVE_ITEM";
        payload["getItemId"] = item;

        // TODO: During a test, this triggered twice when getting Kafei's mask (the scene flags were also sent twice)

        GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
    });

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagSet>(
        [](int16_t sceneNum, FlagType flagType, uint32_t flag) {
            if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) {
                return;
            }

            // TODO: What flags do we send?
            // Currently only sending persistant flags
            // what about permanent flags (clearedRoom, unk_14, rooms)
            if (!SceneFlag_Persistance(sceneNum, flagType, flag)) {
                return;
            }

            nlohmann::json payload;

            payload["type"] = "SET_SCENE_FLAG";
            payload["sceneNum"] = sceneNum;
            payload["flagType"] = flagType;
            payload["flag"] = flag;
            payload["quiet"] = true;

            GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
        });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagSet>([](FlagType flagType, uint32_t flag) {
        if (flagType == FLAG_WEEK_EVENT_REG && flag == WEEKEVENTREG_92_80) {
            return;
        }

        if (flagType == FLAG_WEEK_EVENT_REG && !WeekEventReg_Persistance(flag)) {
            return;
        }
        // TODO: Check if any other flags are persistent/we want to send.

        if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) {
            return;
        }
        nlohmann::json payload;

        payload["type"] = "SET_FLAG";
        payload["flagType"] = flagType;
        payload["flag"] = flag;
        payload["quiet"] = true;

        GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagUnset>(
        [](int16_t sceneNum, FlagType flagType, uint32_t flag) {
            if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) {
                return;
            }

            if (!SceneFlag_Persistance(sceneNum, flagType, flag)) {
                return;
            }
            nlohmann::json payload;

            payload["type"] = "UNSET_SCENE_FLAG";
            payload["sceneNum"] = sceneNum;
            payload["flagType"] = flagType;
            payload["flag"] = flag;
            payload["quiet"] = true;

            GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
        });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagUnset>([](FlagType flagType, uint32_t flag) {
        if (flagType == FLAG_WEEK_EVENT_REG && flag == WEEKEVENTREG_92_80) {
            return;
        }

        if (flagType == FLAG_WEEK_EVENT_REG && !WeekEventReg_Persistance(flag)) {
            return;
        }

        if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) {
            return;
        }
        nlohmann::json payload;

        payload["type"] = "UNSET_FLAG";
        payload["flagType"] = flagType;
        payload["flag"] = flag;
        payload["quiet"] = true;

        GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
    });

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
        payload["quiet"] = true;

        for (auto& [clientId, client] : GameInteractorAnchor::AnchorClients) {
            if (client.sceneNum == gPlayState->sceneId) {
                payload["targetClientId"] = clientId;
                GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
            }
        }
    });
}

void Anchor_RequestTeleport(uint32_t clientId) {
    if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded())
        return;

    nlohmann::json payload;

    payload["type"] = "REQUEST_TELEPORT";
    payload["targetClientId"] = clientId;

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_TeleportToPlayer(uint32_t clientId) {
    if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded())
        return;
    Player* player = GET_PLAYER(gPlayState);

    nlohmann::json payload;

    payload["type"] = "TELEPORT_TO";
    payload["targetClientId"] = clientId;
    payload["entranceIndex"] = gSaveContext.save.entrance;
    payload["roomIndex"] = gPlayState->roomCtx.curRoom.num;
    payload["posRot"] = player->actor.world;

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

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
                    // TODO: Check these parameters. Some issues with teleporting
                    Play_SetRespawnData(gGameState, RESPAWN_MODE_DOWN, client.entranceIndex, client.roomIndex, 0xDFF,
                                        &client.posRot.pos, client.posRot.rot.y);
                    // Play_TriggerVoidOut(gPlayState);
                    func_80169EFC(gGameState);
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
