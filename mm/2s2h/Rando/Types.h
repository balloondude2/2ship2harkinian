#ifndef RANDO_TYPES_H
#define RANDO_TYPES_H

typedef enum {
    RCTYPE_UNKNOWN,
    RCTYPE_CHEST,
    RCTYPE_FREESTANDING,
    RCTYPE_SKULL_TOKEN,
    RCTYPE_SHOP,
    RCTYPE_STRAY_FAIRY,
    RCTYPE_NPC,
    RCTYPE_MAX,
} RandoCheckType;

typedef enum {
    RC_UNKNOWN,

    // Verified as working & name decided, no more iteration necessary
    RC_CLOCK_TOWN_EAST_UPPER_CHEST,
    RC_CLOCK_TOWN_GREAT_FAIRY_ALT,
    RC_CLOCK_TOWN_GREAT_FAIRY,
    RC_CLOCK_TOWN_LAUNDRY_GURU_GURU,
    RC_CLOCK_TOWN_NORTH_TREE_HP,
    RC_CLOCK_TOWN_SOUTH_CHEST_LOWER,
    RC_CLOCK_TOWN_SOUTH_CHEST_UPPER,
    RC_CLOCK_TOWN_SOUTH_PLATFORM_HP,
    RC_CLOCK_TOWN_STRAY_FAIRY,
    RC_GREAT_BAY_GREAT_FAIRY,
    RC_IKANA_GREAT_FAIRY,
    RC_SNOWHEAD_GREAT_FAIRY,
    RC_SNOWHEAD_TEMPLE_BLOCK_ROOM_LEDGE_CHEST,
    RC_SNOWHEAD_TEMPLE_BLOCK_ROOM,
    RC_SNOWHEAD_TEMPLE_BOSS_HC,
    RC_SNOWHEAD_TEMPLE_BOSS_KEY,
    RC_SNOWHEAD_TEMPLE_BRIDGE_ROOM,
    RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_ALCOVE_CHEST,
    RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_BOTTOM_CHEST,
    RC_SNOWHEAD_TEMPLE_COMPASS_ROOM_LEDGE_CHEST,
    RC_SNOWHEAD_TEMPLE_COMPASS,
    RC_SNOWHEAD_TEMPLE_FIRE_ARROW,
    RC_SNOWHEAD_TEMPLE_ICICLE_ROOM_ALCOVE_CHEST,
    RC_SNOWHEAD_TEMPLE_ICICLE_ROOM,
    RC_SNOWHEAD_TEMPLE_MAP_ALCOVE_CHEST,
    RC_SNOWHEAD_TEMPLE_MAP,
    RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_CHEST,
    RC_SNOWHEAD_TEMPLE_SF_BRIDGE_PILLAR,
    RC_SNOWHEAD_TEMPLE_SF_BRIDGE_UNDER_PLATFORM,
    RC_SNOWHEAD_TEMPLE_SF_COMPASS_ROOM_CRATE,
    RC_SNOWHEAD_TEMPLE_SF_DINOLFOS_1,
    RC_SNOWHEAD_TEMPLE_SF_DINOLFOS_2,
    RC_SNOWHEAD_TEMPLE_SF_DUAL_SWITCHES,
    RC_SNOWHEAD_TEMPLE_SF_MAP_ROOM,
    RC_SNOWHEAD_TEMPLE_SF_SNOW_ROOM,
    RC_STOCK_POT_INN_GRANDMA_LONG_STORY,
    RC_STOCK_POT_INN_GRANDMA_SHORT_STORY,
    RC_STOCK_POT_INN_GUEST_ROOM_CHEST,
    RC_STOCK_POT_INN_LETTER_TO_KAFEI,
    RC_STOCK_POT_INN_ROOM_KEY,
    RC_STOCK_POT_INN_STAFF_ROOM_CHEST,
    RC_SWORDSMAN_SCHOOL_HP,
    RC_WOODFALL_GREAT_FAIRY,

    // Not finished
    RC_ANCIENT_CASTLE_OF_IKANA_HP,
    RC_ASTRAL_OBSERVATORY_PASSAGE_CHEST,
    RC_BENEATH_THE_GRAVEYARD_CHEST,
    RC_BENEATH_THE_GRAVEYARD_DAMPE_CHEST,
    RC_BENEATH_THE_GRAVEYARD_HP,
    RC_BENEATH_THE_WELL_COW,
    RC_BENEATH_THE_WELL_KEESE_CHEST,
    RC_BENEATH_THE_WELL_MIRROR_SHIELD,
    RC_BENEATH_THE_WELL_SKULLTULLA_CHEST,
    RC_DEKU_PALACE_GROTTO_CHEST,
    RC_DEKU_PALACE_HP,
    RC_DOGGY_RACETRACK_CHEST,
    RC_GORON_VILLAGE_HP,
    RC_GREAT_BAY_COAST_COW_BACK,
    RC_GREAT_BAY_COAST_COW_FRONT,
    RC_GREAT_BAY_COAST_FISHERMAN_GROTTO,
    RC_GREAT_BAY_COAST_HP,
    RC_GREAT_BAY_TEMPLE_BABA_CHEST,
    RC_GREAT_BAY_TEMPLE_BOSS_HC,
    RC_GREAT_BAY_TEMPLE_BOSS_KEY,
    RC_GREAT_BAY_TEMPLE_COMPASS_ROOM_UNDERWATER,
    RC_GREAT_BAY_TEMPLE_COMPASS,
    RC_GREAT_BAY_TEMPLE_ENTRANCE_CHEST,
    RC_GREAT_BAY_TEMPLE_GREEN_PIPE_1_CHEST,
    RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_LOWER_CHEST,
    RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_UPPER_CHEST,
    RC_GREAT_BAY_TEMPLE_GREEN_PIPE_3_CHEST,
    RC_GREAT_BAY_TEMPLE_ICE_ARROW,
    RC_GREAT_BAY_TEMPLE_MAP,
    RC_GREAT_BAY_TEMPLE_SF_CENTRAL_ROOM_BARREL,
    RC_GREAT_BAY_TEMPLE_SF_CENTRAL_ROOM_UNDERWATER_POT,
    RC_GREAT_BAY_TEMPLE_SF_COMPASS_ROOM_POT,
    RC_GREAT_BAY_TEMPLE_SF_GREEN_PIPE_3_BARREL,
    RC_GREAT_BAY_TEMPLE_SF_MAP_ROOM_POT,
    RC_GREAT_BAY_TEMPLE_SF_PRE_BOSS_ABOVE_WATER,
    RC_GREAT_BAY_TEMPLE_SF_PRE_BOSS_UNDERWATER,
    RC_GREAT_BAY_TEMPLE_SF_WATER_WHEEL_PLATFORM,
    RC_GREAT_BAY_TEMPLE_SF_WATER_WHEEL_SKULLTULA,
    RC_IKANA_GRAVEYARD_CAPTAIN_MASK,
    RC_IKANA_GRAVEYARD_GROTTO,
    RC_IKANA_VALLEY_GROTTO,
    RC_IKANA_VALLEY_SCRUB_HP,
    RC_LONE_PEAK_SHRINE_BOULDER_CHEST,
    RC_LONE_PEAK_SHRINE_INVISIBLE_CHEST,
    RC_LONE_PEAK_SHRINE_LENS_CHEST,
    RC_MOON_TRIAL_DEKU_HP,
    RC_MOON_TRIAL_GORON_HP,
    RC_MOON_TRIAL_LINK_GARO_MASTER_CHEST,
    RC_MOON_TRIAL_LINK_HP,
    RC_MOON_TRIAL_LINK_IRON_KNUCKLE_CHEST,
    RC_MOON_TRIAL_ZORA_HP,
    RC_MOUNTAIN_VILLAGE_TUNNEL_GROTTO,
    RC_MOUNTAIN_VILLAGE_WATERFALL_CHEST,
    RC_OCEAN_SKULLTULA_2ND_ROOM_BEHIND_SKULL_1,
    RC_OCEAN_SKULLTULA_2ND_ROOM_BEHIND_SKULL_2,
    RC_OCEAN_SKULLTULA_2ND_ROOM_CEILING_EDGE,
    RC_OCEAN_SKULLTULA_2ND_ROOM_CEILING_PLANK,
    RC_OCEAN_SKULLTULA_2ND_ROOM_JAR,
    RC_OCEAN_SKULLTULA_2ND_ROOM_LOWER_POT,
    RC_OCEAN_SKULLTULA_2ND_ROOM_UPPER_POT,
    RC_OCEAN_SKULLTULA_2ND_ROOM_WEBBED_HOLE,
    RC_OCEAN_SKULLTULA_2ND_ROOM_WEBBED_POT,
    RC_OCEAN_SKULLTULA_COLORED_SKULLS_BEHIND_PICTURE,
    RC_OCEAN_SKULLTULA_COLORED_SKULLS_CEILING_EDGE,
    RC_OCEAN_SKULLTULA_COLORED_SKULLS_CHANDELIER_1,
    RC_OCEAN_SKULLTULA_COLORED_SKULLS_CHANDELIER_2,
    RC_OCEAN_SKULLTULA_COLORED_SKULLS_CHANDELIER_3,
    RC_OCEAN_SKULLTULA_COLORED_SKULLS_POT,
    RC_OCEAN_SKULLTULA_ENTRANCE_LEFT_WALL,
    RC_OCEAN_SKULLTULA_ENTRANCE_RIGHT_WALL,
    RC_OCEAN_SKULLTULA_ENTRANCE_WEB,
    RC_OCEAN_SKULLTULA_LIBRARY_BEHIND_BOOKCASE_1,
    RC_OCEAN_SKULLTULA_LIBRARY_BEHIND_BOOKCASE_2,
    RC_OCEAN_SKULLTULA_LIBRARY_BEHIND_PICTURE,
    RC_OCEAN_SKULLTULA_LIBRARY_CEILING_EDGE,
    RC_OCEAN_SKULLTULA_LIBRARY_HOLE_BEHIND_CABINET,
    RC_OCEAN_SKULLTULA_LIBRARY_HOLE_BEHIND_PICTURE,
    RC_OCEAN_SKULLTULA_LIBRARY_ON_CORNER_BOOKSHELF,
    RC_OCEAN_SKULLTULA_STORAGE_ROOM_BEHIND_BOAT,
    RC_OCEAN_SKULLTULA_STORAGE_ROOM_BEHIND_CRATE,
    RC_OCEAN_SKULLTULA_STORAGE_ROOM_CEILING_WEB,
    RC_OCEAN_SKULLTULA_STORAGE_ROOM_CRATE,
    RC_OCEAN_SKULLTULA_STORAGE_ROOM_JAR,
    RC_OCEAN_SPIDER_HOUSE_CHEST_HP,
    RC_PATH_TO_SNOWHEAD_GROTTO,
    RC_PATH_TO_SNOWHEAD_HP,
    RC_PINNACLE_ROCK_CHEST_1,
    RC_PINNACLE_ROCK_CHEST_2,
    RC_PIRATE_FORTRESS_ENTRANCE_CHEST_1,
    RC_PIRATE_FORTRESS_ENTRANCE_CHEST_2,
    RC_PIRATE_FORTRESS_ENTRANCE_CHEST_3,
    RC_PIRATE_FORTRESS_INTERIOR_AQUARIUM,
    RC_PIRATE_FORTRESS_INTERIOR_HEART_1,
    RC_PIRATE_FORTRESS_INTERIOR_HEART_2,
    RC_PIRATE_FORTRESS_INTERIOR_HEART_3,
    RC_PIRATE_FORTRESS_INTERIOR_HOOKSHOT,
    RC_PIRATE_FORTRESS_INTERIOR_LOWER_CHEST,
    RC_PIRATE_FORTRESS_INTERIOR_SILVER_RUPEE_CHEST,
    RC_PIRATE_FORTRESS_INTERIOR_UPPER_CHEST,
    RC_PIRATE_FORTRESS_SEWERS_CHEST_1,
    RC_PIRATE_FORTRESS_SEWERS_CHEST_2,
    RC_PIRATE_FORTRESS_SEWERS_CHEST_3,
    RC_PIRATE_FORTRESS_SEWERS_HP,
    RC_ROAD_TO_IKANA_CHEST,
    RC_ROAD_TO_IKANA_GROTTO,
    RC_ROAD_TO_SOUTHERN_SWAMP_GROTTO,
    RC_ROAD_TO_SOUTHERN_SWAMP_HP,
    RC_ROMANI_RANCH_BARN_COW_LEFT,
    RC_ROMANI_RANCH_BARN_COW_RIGHT_BACK,
    RC_ROMANI_RANCH_BARN_COW_RIGHT_FRONT,
    RC_SECRET_SHRINE_DINALFOS_CHEST,
    RC_SECRET_SHRINE_GARO_MASTER_CHEST,
    RC_SECRET_SHRINE_HP_CHEST,
    RC_SECRET_SHRINE_WART_CHEST,
    RC_SECRET_SHRINE_WIZZROBE_CHEST,
    RC_SOUTHERN_SWAMP_GROTTO,
    RC_SOUTHERN_SWAMP_HP,
    RC_STONE_TOWER_COMPASS,
    RC_STONE_TOWER_ENTRANCE_CHEST,
    RC_STONE_TOWER_ENTRANCE_SWITCH_CHEST,
    RC_STONE_TOWER_INVERTED_BOSS_KEY,
    RC_STONE_TOWER_INVERTED_CHEST_1,
    RC_STONE_TOWER_INVERTED_CHEST_2,
    RC_STONE_TOWER_INVERTED_CHEST_3,
    RC_STONE_TOWER_INVERTED_DEATH_ARMOS_CHEST,
    RC_STONE_TOWER_INVERTED_EAST_LOWER_CHEST,
    RC_STONE_TOWER_INVERTED_EAST_MIDDLE_CHEST,
    RC_STONE_TOWER_INVERTED_EAST_UPPER_CHEST,
    RC_STONE_TOWER_INVERTED_ENTRANCE_CHEST,
    RC_STONE_TOWER_INVERTED_GIANT_MASK,
    RC_STONE_TOWER_INVERTED_WIZZROBE_CHEST,
    RC_STONE_TOWER_LIGHT_ARROW,
    RC_STONE_TOWER_MAP,
    RC_STONE_TOWER_MIRRORS_ROOM_CENTER_CHEST,
    RC_STONE_TOWER_MIRRORS_ROOM_RIGHT_CHEST,
    RC_STONE_TOWER_TEMPLE_BEFORE_WATER_BRIDGE_CHEST,
    RC_STONE_TOWER_TEMPLE_CENTER_ACROSS_WATER_CHEST,
    RC_STONE_TOWER_TEMPLE_CENTER_SUN_BLOCK_CHEST,
    RC_STONE_TOWER_TEMPLE_INVERTED_BOSS_HC,
    RC_STONE_TOWER_UNDER_WEST_GARDEN_LAVA_CHEST,
    RC_STONE_TOWER_UNDER_WEST_GARDEN_LEDGE_CHEST,
    RC_STONE_TOWER_WATER_BRIDGE_CHEST,
    RC_STONE_TOWER_WATER_SUN_SWITCH_CHEST,
    RC_STONE_TOWER_WIND_ROOM_JAIL_CHEST,
    RC_STONE_TOWER_WIND_ROOM_LEDGE_CHEST,
    RC_SWAMP_SKULLTULA_GOLD_ROOM_HIVE,
    RC_SWAMP_SKULLTULA_GOLD_ROOM_NEAR_CEILING,
    RC_SWAMP_SKULLTULA_GOLD_ROOM_PILLAR,
    RC_SWAMP_SKULLTULA_GOLD_ROOM_WALL,
    RC_SWAMP_SKULLTULA_MAIN_ROOM_JAR,
    RC_SWAMP_SKULLTULA_MAIN_ROOM_LOWER_LEFT_SOFT_SOIL,
    RC_SWAMP_SKULLTULA_MAIN_ROOM_LOWER_RIGHT_SOFT_SOIL,
    RC_SWAMP_SKULLTULA_MAIN_ROOM_NEAR_CEILING,
    RC_SWAMP_SKULLTULA_MAIN_ROOM_PILLAR,
    RC_SWAMP_SKULLTULA_MAIN_ROOM_UPPER_PILLAR,
    RC_SWAMP_SKULLTULA_MAIN_ROOM_UPPER_SOFT_SOIL,
    RC_SWAMP_SKULLTULA_MAIN_ROOM_WATER,
    RC_SWAMP_SKULLTULA_MONUMENT_ROOM_CRATE_1,
    RC_SWAMP_SKULLTULA_MONUMENT_ROOM_CRATE_2,
    RC_SWAMP_SKULLTULA_MONUMENT_ROOM_LOWER_WALL,
    RC_SWAMP_SKULLTULA_MONUMENT_ROOM_ON_MONUMENT,
    RC_SWAMP_SKULLTULA_MONUMENT_ROOM_TORCH,
    RC_SWAMP_SKULLTULA_POT_ROOM_BEHIND_VINES,
    RC_SWAMP_SKULLTULA_POT_ROOM_HIVE_1,
    RC_SWAMP_SKULLTULA_POT_ROOM_HIVE_2,
    RC_SWAMP_SKULLTULA_POT_ROOM_JAR,
    RC_SWAMP_SKULLTULA_POT_ROOM_POT_1,
    RC_SWAMP_SKULLTULA_POT_ROOM_POT_2,
    RC_SWAMP_SKULLTULA_POT_ROOM_WALL,
    RC_SWAMP_SKULLTULA_TREE_ROOM_GRASS_1,
    RC_SWAMP_SKULLTULA_TREE_ROOM_GRASS_2,
    RC_SWAMP_SKULLTULA_TREE_ROOM_HIVE,
    RC_SWAMP_SKULLTULA_TREE_ROOM_TREE_1,
    RC_SWAMP_SKULLTULA_TREE_ROOM_TREE_2,
    RC_SWAMP_SKULLTULA_TREE_ROOM_TREE_3,
    RC_TERMINA_FIELD_BIO_BABA_GROTTO,
    RC_TERMINA_FIELD_COW_BACK,
    RC_TERMINA_FIELD_COW_FRONT,
    RC_TERMINA_FIELD_DODONGO_GROTTO,
    RC_TERMINA_FIELD_PEAHAT_GROTTO,
    RC_TERMINA_FIELD_PILLAR_GROTTO,
    RC_TERMINA_FIELD_TALL_GRASS_CHEST,
    RC_TERMINA_FIELD_TALL_GRASS_GROTTO,
    RC_TERMINA_FIELD_TREE_STUMP_CHEST,
    RC_TERMINA_FIELD_WATER_CHEST,
    RC_TWIN_ISLANDS_FROZEN_GROTTO_CHEST,
    RC_TWIN_ISLANDS_RAMP_GROTTO_CHEST,
    RC_TWIN_ISLANDS_UNDERWATER_CHEST_1,
    RC_TWIN_ISLANDS_UNDERWATER_CHEST_2,
    RC_WOODFALL_BOSS_KEY_CHEST,
    RC_WOODFALL_BOW,
    RC_WOODFALL_COMPASS,
    RC_WOODFALL_DARK_CHEST,
    RC_WOODFALL_ENTRANCE_CHEST,
    RC_WOODFALL_HP_CHEST,
    RC_WOODFALL_MAP,
    RC_WOODFALL_NEAR_OWL_CHEST,
    RC_WOODFALL_TEMPLE_BOSS_CONTAINER,
    RC_WOODFALL_TEMPLE_CENTER_CHEST,
    RC_WOODFALL_TEMPLE_ENTRANCE_CHEST,
    RC_WOODFALL_TEMPLE_SF_ENTRANCE,
    RC_WOODFALL_TEMPLE_SF_MAIN_BUBBLE,
    RC_WOODFALL_TEMPLE_SF_MAIN_DEKU_BABA,
    RC_WOODFALL_TEMPLE_SF_MAIN_POT,
    RC_WOODFALL_TEMPLE_SF_MAZE_BEEHIVE,
    RC_WOODFALL_TEMPLE_SF_MAZE_BUBBLE,
    RC_WOODFALL_TEMPLE_SF_MAZE_SKULLTULA,
    RC_WOODFALL_TEMPLE_SF_PRE_BOSS_BOTTOM_RIGHT,
    RC_WOODFALL_TEMPLE_SF_PRE_BOSS_LEFT,
    RC_WOODFALL_TEMPLE_SF_PRE_BOSS_PILLAR,
    RC_WOODFALL_TEMPLE_SF_PRE_BOSS_TOP_RIGHT,
    RC_WOODFALL_TEMPLE_SF_WATER_ROOM_BEEHIVE,
    RC_WOODFALL_TEMPLE_WATER_CHEST,
    RC_WOODS_OF_MYSTERY_GROTTO,
    RC_ZORA_CAPE_GROTTO,
    RC_ZORA_CAPE_LEDGE_CHEST_1,
    RC_ZORA_CAPE_LEDGE_CHEST_2,
    RC_ZORA_CAPE_UNDERWATER_CHEST,
    RC_ZORA_CAPE_WATERFALL_HP,
    RC_ZORA_HALL_SCRUB_HP,
    RC_MAX,
} RandoCheckId;

typedef enum {
    RI_UNKNOWN,
    RI_ARROW_FIRE,
    RI_ARROW_ICE,
    RI_ARROW_LIGHT,
    RI_ARROWS_30,
    RI_BOMBCHU_10,
    RI_BOMBCHU_5,
    RI_BOMBCHU,
    RI_BOTTLE_EMPTY,
    RI_BOW,
    RI_CLOCK_TOWN_STRAY_FAIRY,
    RI_DOUBLE_DEFENSE,
    RI_GREAT_BAY_BOSS_KEY,
    RI_GREAT_BAY_COMPASS,
    RI_GREAT_BAY_MAP,
    RI_GREAT_BAY_SMALL_KEY,
    RI_GREAT_BAY_STRAY_FAIRY,
    RI_GREAT_FAIRY_SWORD,
    RI_GREAT_SPIN_ATTACK,
    RI_GS_TOKEN_OCEAN,
    RI_GS_TOKEN_SWAMP,
    RI_HEART_CONTAINER,
    RI_HEART_PIECE,
    RI_HOOKSHOT,
    RI_LENS,
    RI_LETTER_TO_KAFEI,
    RI_MAGIC_BEAN,
    RI_MASK_BREMEN,
    RI_MASK_CAPTAIN,
    RI_MASK_GIANT,
    RI_MASK_GREAT_FAIRY,
    RI_MILK,
    RI_PROGRESSIVE_MAGIC,
    RI_RECOVERY_HEART,
    RI_ROOM_KEY,
    RI_RUPE_PURPLE,
    RI_RUPEE_BLUE,
    RI_RUPEE_GREEN,
    RI_RUPEE_PURPLE,
    RI_RUPEE_RED,
    RI_RUPEE_SILVER,
    RI_SHIELD_MIRROR,
    RI_SNOWHEAD_BOSS_KEY,
    RI_SNOWHEAD_COMPASS,
    RI_SNOWHEAD_MAP,
    RI_SNOWHEAD_SMALL_KEY,
    RI_SNOWHEAD_STRAY_FAIRY,
    RI_STONE_TOWER_BOSS_KEY,
    RI_STONE_TOWER_COMPASS,
    RI_STONE_TOWER_MAP,
    RI_STONE_TOWER_SMALL_KEY,
    RI_STONE_TOWER_STRAY_FAIRY,
    RI_SUNS_SONG,
    RI_WOODFALL_BOSS_KEY,
    RI_WOODFALL_COMPASS,
    RI_WOODFALL_MAP,
    RI_WOODFALL_SMALL_KEY,
    RI_WOODFALL_STRAY_FAIRY,
    RI_MAX,
} RandoItemId;

typedef enum {
    RANDO_INF_CLOCK_TOWN_STRAY_FAIRY_COLLECTED,
    RANDO_INF_CLOCK_TOWN_GREAT_FAIRY_ALT,
    RANDO_INF_CLOCK_TOWN_GREAT_FAIRY,
    RANDO_INF_GREAT_BAY_GREAT_FAIRY,
    RANDO_INF_IKANA_GREAT_FAIRY,
    RANDO_INF_SNOWHEAD_GREAT_FAIRY,
    RANDO_INF_WOODFALL_GREAT_FAIRY,
    RANDO_INF_MAX,
} RandoInf;

#endif // RANDO_TYPES_H
