#ifndef Z_EN_BEN_H
#define Z_EN_BEN_H

#include "global.h"

#include "z64snap.h"

struct Ben;

typedef struct Ben {
    /* 0x000 */ PictoActor picto; //sizes are probably different
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ u8 state;
    /* 0x191 */ u8 framesUntilNextState;
    /* 0x192 */ s16 alpha;
} Ben; // size = 0x194

typedef enum {
    /* 0 */ BEN_PARAM_HUMAN,
    /* 1 */ BEN_PARAM_GORON,
    /* 2 */ BEN_PARAM_ZORA,
    /* 3 */ BEN_PARAM_DEKU,
    /* 4 */ BEN_PARAM_FIERCE_DEITY
} BenParam;

typedef enum {
    /* 0 */ BEN_STATE_INITIALIZED,
    /* 1 */ BEN_STATE_FADING_IN,
    /* 2 */ BEN_STATE_SOLID,
    /* 3 */ BEN_STATE_IDLE,
    /* 4 */ BEN_STATE_DYING
} BenState;

#endif // Z_EN_BEN_H
