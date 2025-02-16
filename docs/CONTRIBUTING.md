# Modding

> You'd like to contribute? That's great! Read here to learn how we recommend going about it.

## Preface

The information in [MODDING.md](https://github.com/HarbourMasters/Shipwright/blob/develop/docs/MODDING.md) is just as relevant to this. This document aims more at giving you best practices for code format, modification documentation, and repo/branch management. All of the information there will help with this aspect.

## Guiding principles

The main idea behind contributions that get merged into 2ship2harkinian's main repo is well-built, easy to maintain changes that most often modify, extend, or improve built-in functionality. Things like custom items, for example, conflict with vanilla, and thus should remain as custom builds. However, you never know what might be considered for merging, so feel free to PR, or you can talk to devs about your idea on the [Discord](https://discord.gg/shipofharkinian).

## Repo management

In addition to principles outlined in MODDING.md, the importance of maintaining clean copies of the main repos (develop, and develop-{release}) cannot be overstated. It is easier to create new branches and update existing ones with changes from SoH using branches on your fork that are always exact mirrors of the primary branches on HM64's repo. All changes should be committed to new branches on your own repo, and those branches form the base of PRs you then submit to HM64.

## Formatting

While we are still in the process of codifying code formats, whitespacing, etc, generally you can follow the precedents set around where you're making changes for information on how to indent lines, opening bracket positions, capitalization, variable naming, etc. Be specific, be as concisely elaborate/accurate as you can.  For example:

```c++
    if (CVarGetInteger("gCustomizeFrogsOcarinaGame", 0)) {
        this->frogSongTimer = sTimerFrogSong[index] * CVarGetInteger("gFrogsModifyFailTime", 1);
    } else {
        this->frogSongTimer = sTimerFrogSong[index];
    }
```

Here you see we open brackets on the same line, spacing outside the parentheses but not inside, same line else and opening bracket, etc. Also, all if...else blocks should be fully encapsulated with brackets, even if they're single-line blocks (which technically would be legal code).

## Project Stucture

Coming into a large project like this, it can feel overwhelming knowing where to start. Below is a portion of the file structure highlighting the folders that you are most likely to use when first starting to contribute. 

├── mm
│   ├── 2s2h
│   │   ├── BenGui
│   │   ├── Enhancements
│   │   ├── GameInteractor
│   │   └── ...other folders
│   ├── include
│   ├── src
│   │   ├── code
│   │   ├── overlays
│   │   └── ..other folders
│   └── ...other folders
├── libultraship
├── OTRExporter
├── ZAPDTR
└── ...other folders

mm/2s2h is where a lot of the port stuff lives. You'll add enhancements and their corresponding menu options here. 

mm/include will have files containing a lot of the structs and enums that are used in MM. You'll rarely change things here, but these files are handy references.

mm/src contains the code for Majora's Mask. /code blah blah. /overlays/actors will contain the code that each actor runs. 

## Hooks

Hooks provide us a way to execute new code with minimal to no changes to the src/ code. The currently available hooks can be found in `mm/2s2h/GameInteractor/GameInteractor.cpp`. These hooks are placed in the appropriate spots in the src/ code. Then when creating an enhancement, we can register the hook to execute our new code without impacting the orginal src/ code.

For example, let's create an enhancement that gives the payer 5 deku nuts at the beginning of every cycle. There is an existing hook `GameInteractor_ExecuteAfterEndOfCycleSave()` that .In `src/code/z_sram_NES.c` we find the function `Sram_SaveEndOfCycle()`, which is the function that runs when the game saves after playing the Song of Time.

```c
void Sram_SaveEndOfCycle(PlayState* play) {
    GameInteractor_ExecuteBeforeEndOfCycleSave();
    s16 sceneId;

    /* Additional code */

    Horse_ResetHorseData(play);
    GameInteractor_ExecuteAfterEndOfCycleSave();
}
```

There are two hooks in that function: `GameInteractor_ExecuteBeforeEndOfCycleSave()` and `GameInteractor_ExecuteAfterEndOfCycleSave()`. 

## Vanilla Behavior Hooks

Sometimes we want to change a non-general spot or conditional. `GameInteractor_Should(VB_, vanilla condition, {optional args})`. The following conditional determines which reward Cremia will give the player after completing the milk run. 

```c
if ((INV_CONTENT(ITEM_MASK_ROMANI) == ITEM_MASK_ROMANI) &&
    CHECK_WEEKEVENTREG(WEEKEVENTREG_ESCORTED_CREMIA) &&
    (Rand_Next() & 0x80)) {
    EnMaYto_SetupBeginWarmFuzzyFeelingCs(this);
} else {
    EnMaYto_SetupAfterMilkRunInit(this);
}
```

To adjust the condition to eliminate the randomness is an excellent use case for `GameInteractor_Should()`.

```c
if ((INV_CONTENT(ITEM_MASK_ROMANI) == ITEM_MASK_ROMANI) &&
    CHECK_WEEKEVENTREG(WEEKEVENTREG_ESCORTED_CREMIA) &&
    (GameInteractor_Should(VB_PLAY_CREMIA_HUG_CUTSCENE, Rand_Next() & 0x80))) {
    EnMaYto_SetupBeginWarmFuzzyFeelingCs(this);
} else {
    EnMaYto_SetupAfterMilkRunInit(this);
}
```

We took the vanilla `Rand_Next() & 0x80` and put it as the second argument in our `GameInteractor_Should()`. The first arguement is an enum that is defined in `GameInteractor.h` that describes what the vanilla condition does. We don't need to pass any va_args in this example. 

Without doing anything else, `GameInteractor_Should(VB_PLAY_CREMIA_HUG_CUTSCENE, Rand_Next() & 0x80)` will evaluate to `Rand_Next() & 0x80`. We will need to register this hook in our enhancement file to adjust that.

```c++
COND_VB_SHOULD(VB_PLAY_CREMIA_HUG_CUTSCENE, CVAR != CREMIA_REWARD_RANDOM, {
    if (CVAR == CREMIA_REWARD_ALWAYS_HUG) {
        *should = true;
    } else if (CVAR == CREMIA_REWARD_ALWAYS_RUPEE) {
        *should = false;
    }
});
```

At the beginning of the hook, `*should` equals the vanilla condition (`Rand_Next() & 0x80` in this case). Based on the user's configuration, `*should` is updated to the boolean we want `GameInteractor_Should(VB_PLAY_CREMIA_HUG_CUTSCENE, Rand_Next() & 0x80)` to evaulate to. 

## Items to Include

Adding code to the CPP codebase
Adding code in src/
Modifying/Removing code in src/
Commenting practices
Variable & file naming conventions for CPP codebase
Variable naming conventions in src/
When to use hooks, when to use cvars, and when to augment the original source code (and in which cases it's okay)
When is it okay to enable a cvar by default
etc


## Documentation

We want to maintain the readability and integrity of vanilla codeflow as much as possible going forward. To do this, we have decided that we will want any modifications to vanilla code to be well notated and maintained separately from modified sections. Given this original code:

```c++
        this->frogSongTimer = 40;
        this->ocarinaNoteIndex = 0;
        func_8010BD58(play, OCARINA_ACTION_FROGS);
        this->ocarinaNote = EnFr_GetNextNoteFrogSong(this->ocarinaNoteIndex);
        EnFr_CheckOcarinaInputFrogSong(this->ocarinaNote);
        this->actionFunc = EnFr_ContinueFrogSong;
```

If you wanted to modify this block with an enhancement to add difficulty options to the frog ocarina game, the first thing you would do is encapsulate it in a comment region as such:

```c++
        // #region SOH [Enhancement]
        if (CVarGetInteger("gCustomizeFrogsOcarinaGame", 0) == 1) {
            this->frogSongTimer = 40;
            this->ocarinaNoteIndex = 0;
            func_8010BD58(play, OCARINA_ACTION_FROGS);
            this->ocarinaNote = EnFr_GetNextNoteFrogSong(this->ocarinaNoteIndex);
            EnFr_CheckOcarinaInputFrogSong(this->ocarinaNote);
            this->actionFunc = EnFr_ContinueFrogSong;
        // #endregion
        } else {
            this->frogSongTimer = 40;
            this->ocarinaNoteIndex = 0;
            func_8010BD58(play, OCARINA_ACTION_FROGS);
            this->ocarinaNote = EnFr_GetNextNoteFrogSong(this->ocarinaNoteIndex);
            EnFr_CheckOcarinaInputFrogSong(this->ocarinaNote);
            this->actionFunc = EnFr_ContinueFrogSong;
        }
```

Incorporate as much of the section as is applicable in a block behind a CVar, with comment mentioning what type of addition it is (main categories are `Enhancement`, `Cheat`, `Randomizer`, or `Debug`), and put the `// #endregion` comment before the else block. Then, make your changes in the if block. In this instance, it ended up looking like this:

```c++
        if (CVarGetInteger("gCustomizeFrogsOcarinaGame", 0) == 1) {
            this->frogSongTimer = 40 * CVarGetInteger("gFrogsModifyFailTime", 1);
            if (CVarGetInteger("gInstantFrogsGameWin", 0) == 1) {
                this->actor.textId = 0x40AC;
                EnFr_SetupReward(this, play, false);
            } else {
                this->ocarinaNoteIndex = 0;
                func_8010BD58(play, OCARINA_ACTION_FROGS);
                this->ocarinaNote = EnFr_GetNextNoteFrogSong(this->ocarinaNoteIndex);
                EnFr_CheckOcarinaInputFrogSong(this->ocarinaNote);
                this->actionFunc = EnFr_ContinueFrogSong;
            }
```

This does result in some code duplication, but this is just a step to where we want to eventually be where enhancement additions are encapsulated in source files housed elsewhere that are called from within the enhancement CVar check, and is more allowable in a situation like we're in, where preserving vanilla code is important to us. The final block ends up looking as such:

```c++
        // #region SOH [Enhancement]
        if (CVarGetInteger("gCustomizeFrogsOcarinaGame", 0) == 1) {
            this->frogSongTimer = 40 * CVarGetInteger("gFrogsModifyFailTime", 1);
            if (CVarGetInteger("gInstantFrogsGameWin", 0) == 1) {
                this->actor.textId = 0x40AC;
                EnFr_SetupReward(this, play, false);
            } else {
                this->ocarinaNoteIndex = 0;
                func_8010BD58(play, OCARINA_ACTION_FROGS);
                this->ocarinaNote = EnFr_GetNextNoteFrogSong(this->ocarinaNoteIndex);
                EnFr_CheckOcarinaInputFrogSong(this->ocarinaNote);
                this->actionFunc = EnFr_ContinueFrogSong;
            }
        // #endregion
        } else {
            this->frogSongTimer = 40;
            this->ocarinaNoteIndex = 0;
            func_8010BD58(play, OCARINA_ACTION_FROGS);
            this->ocarinaNote = EnFr_GetNextNoteFrogSong(this->ocarinaNoteIndex);
            EnFr_CheckOcarinaInputFrogSong(this->ocarinaNote);
            this->actionFunc = EnFr_ContinueFrogSong;
        }
```

Feel free to add any description you want to the opening `// #region` comment, but none should be necessary with properly named variables and CVars.

In the case of alternative/replacement functions and variables, all that's necessary is making the names of these replacements mirror the names of what they're replacing, with a short addition referring to the type of change. The fact that they're used in a modifying way will be clear enough with the above procedure used where they're actually called.