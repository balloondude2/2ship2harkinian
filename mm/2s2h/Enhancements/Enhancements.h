#ifndef ENHANCEMENTS_H
#define ENHANCEMENTS_H

#include "Cycle/Cycle.h"
#include "Saving/SavingEnhancements.h"

enum AlwaysWinDoggyRaceOptions {
    ALWAYS_WIN_DOGGY_RACE_OFF,
    ALWAYS_WIN_DOGGY_RACE_MASKOFTRUTH,
    ALWAYS_WIN_DOGGY_RACE_ALWAYS,
};

enum TimeStopOptions {
    TIME_STOP_OFF,
    TIME_STOP_TEMPLES,
    TIME_STOP_TEMPLES_DUNGEONS,
};

enum ClockTypeOptions {
    CLOCK_TYPE_ORIGINAL,
    CLOCK_TYPE_3DS,
    CLOCK_TYPE_TEXT_BASED,
};

enum CremiaRewardsOptions {
    CREMIA_REWARD_RANDOM,
    CREMIA_REWARD_ALWAYS_HUG,
    CREMIA_REWARD_ALWAYS_RUPEE,
};

enum DekuGuardSearchBallsOptions {
    DEKU_GUARD_SEARCH_BALLS_NIGHT_ONLY,
    DEKU_GUARD_SEARCH_BALLS_NEVER,
    DEKU_GUARD_SEARCH_BALLS_ALWAYS,
};

enum VoiceWordOptions {
    VOICE_WORD_TIME,
    VOICE_WORD_PICTURE,
    VOICE_WORD_WAKE,
    VOICE_WORD_SIT,
    VOICE_WORD_MILK,
    VOICE_WORD_EPONA,
    VOICE_WORD_NONE = 0xFFFF,
};

// Old Entry Point
void InitEnhancements();

#endif // ENHANCEMENTS_H
