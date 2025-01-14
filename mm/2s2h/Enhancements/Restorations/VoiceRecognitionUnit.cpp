#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "Enhancements/Enhancements.h"
#include "2s2h/ShipInit.hpp"
#include <vosk_api.h>
#include <SDL.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

extern "C" {
OSMesgQueue* PadMgr_AcquireSerialEventQueue(void);
void PadMgr_ReleaseSerialEventQueue(OSMesgQueue* serialEventQueue);
// PlayState* gPlayState;

#include "variables.h"
#include "z64voice.h"
void func_801A4EB8(void);
}

using json = nlohmann::json;

#define CVAR_NAME "gEnhancements.Restorations.VoiceRecognitionUnit"
#define CVAR CVarGetInteger(CVAR_NAME, 0)
#define CVAR_NAME_WORD "gEnhancements.Restorations.VoiceRecognitionUnitWord"
#define CVAR_WORD CVarGetInteger(CVAR_NAME_WORD, VOICE_WORD_NONE)

static VoskModel *model;
static VoskRecognizer *recognizer;

typedef VoskModel* (*ptr_vosk_model_new)(const char *model_path);
typedef void (*ptr_vosk_model_free)(VoskModel *model);
typedef VoskRecognizer* (*ptr_vosk_recognizer_new_grm)(VoskModel *model, float sample_rate, const char *grammar);
typedef void (*ptr_vosk_recognizer_free)(VoskRecognizer *recognizer);
typedef int (*ptr_vosk_recognizer_accept_waveform)(VoskRecognizer *recognizer, const char *data, int length);
typedef const char *(*ptr_vosk_recognizer_final_result)(VoskRecognizer *recognizer);
typedef void (*ptr_vosk_set_log_level)(int log_level);
typedef void (*ptr_vosk_recognizer_set_max_alternatives)(VoskRecognizer *recognizer, int max_alternatives);

ptr_vosk_recognizer_accept_waveform VoskAcceptWaveform;
ptr_vosk_recognizer_final_result VoskFinalResult;
ptr_vosk_model_new VoskNewModel;
ptr_vosk_recognizer_new_grm VoskNewRecognizer;
ptr_vosk_model_free VoskFreeModel;
ptr_vosk_recognizer_free VoskFreeRecognizer;
ptr_vosk_set_log_level VoskSetLogLevel;
ptr_vosk_recognizer_set_max_alternatives VoskSetAlternatives;

static int l_TalkingState = 0;


static SDL_AudioDeviceID audio_dev;
static SDL_AudioSpec *hardware_spec;
static SDL_TimerID timer_id;


static int wordListLength;
static int wordListCount;
static std::vector<std::string> wordList;

// Implementation of VRU functions
int32_t osVoiceInit(OSMesgQueue* mq, OSVoiceHandle* hd, int channel) {
    
    // if (voskLib)
    //     return 0;
    // voskLib = new QLibrary((QDir(QCoreApplication::applicationDirPath()).filePath("vosk")));
    // VoskAcceptWaveform = (ptr_vosk_recognizer_accept_waveform) voskLib->resolve("vosk_recognizer_accept_waveform");
    // VoskFinalResult = (ptr_vosk_recognizer_final_result) voskLib->resolve("vosk_recognizer_final_result");
    // VoskNewModel = (ptr_vosk_model_new) voskLib->resolve("vosk_model_new");
    // VoskNewRecognizer = (ptr_vosk_recognizer_new_grm) voskLib->resolve("vosk_recognizer_new_grm");
    // VoskFreeModel = (ptr_vosk_model_free) voskLib->resolve("vosk_model_free");
    // VoskFreeRecognizer = (ptr_vosk_recognizer_free) voskLib->resolve("vosk_recognizer_free");
    // VoskSetLogLevel = (ptr_vosk_set_log_level) voskLib->resolve("vosk_set_log_level");
    // VoskSetAlternatives = (ptr_vosk_recognizer_set_max_alternatives) voskLib->resolve("vosk_recognizer_set_max_alternatives");

    // VoskSetLogLevel(-1);

    // QJsonDocument vruwordjson = QJsonDocument::fromJson(vruwords.toUtf8());
    // vruwordsobject = vruwordjson.object();

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
        LUSLOG_DEBUG("SDL fails to initialize audio subsystem!%s", SDL_GetError());
    else
        LUSLOG_DEBUG("SDL Audio was initialized fine!");
    
    LUSLOG_DEBUG("Number of audio devices: %d", SDL_GetNumAudioDevices(0));

    l_TalkingState = 0;
    SDL_AudioSpec *desired, *obtained;
    if(hardware_spec != NULL) free(hardware_spec);
    desired = (SDL_AudioSpec*)malloc(sizeof(SDL_AudioSpec));
    obtained = (SDL_AudioSpec*)malloc(sizeof(SDL_AudioSpec));
    desired->freq = 44100;
    desired->format = AUDIO_S16SYS;
    desired->channels = 1;
    desired->samples = 1024;
    desired->callback = NULL;
    desired->userdata = NULL;
    audio_dev = SDL_OpenAudioDevice(NULL, 1, desired, obtained, SDL_AUDIO_ALLOW_SAMPLES_CHANGE | SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
    LUSLOG_DEBUG("audio device id: %x", audio_dev);
    free(desired);
    hardware_spec = obtained;
    recognizer = nullptr;
    model = nullptr;
    timer_id = 0;
    // model = vosk_model_new("D:\\Travis\\Downloads\\vosk-model-small-ja-0.22\\vosk-model-small-ja-0.22");
    model = vosk_model_new("D:\\Travis\\Downloads\\vosk-model-small-en-us-0.15\\vosk-model-small-en-us-0.15");
    return 0;
    
    // delete voskLib;
    // voskLib = NULL;

    // return 1;
}

// jikan and okiro- aren't recognized together. Can probably separate them out by character for the grammer stuff
// static std::vector<std::string> ogwordList = {"アト ナン ジ カン", "ハイ チーズ", " オキ ロー", "オスワリ", "ミルク", "ハイヤー"};
static std::vector<std::string> ogwordList = {"time", "cheese", " wake", "sit", "milk", "go"};

int32_t osVoiceSetWord(OSVoiceHandle* hd, u8* word) {
    wordList.push_back(ogwordList[wordListCount]);
    int inModel = vosk_model_find_word(model, ogwordList[wordListCount].c_str());
    LUSLOG_DEBUG("word %d: %d", wordListCount, inModel);
    wordListCount++;

    if (wordListCount == wordListLength) {
        wordList.push_back("[unk]");
        if (recognizer) {
            vosk_recognizer_free(recognizer);
        }
        json jsonArray = wordList;
        // Convert to string
        std::string jsonString = jsonArray.dump();

        // Convert to const char*
        const char* cString = jsonString.c_str();
        recognizer = vosk_recognizer_new_grm(model, (float)hardware_spec->freq, cString);
        vosk_recognizer_set_max_alternatives(recognizer, 3);
    }
    return 0;
}

int32_t osVoiceCheckWord(u8* word) {
    return 0;
}

uint32_t stopTalking(Uint32, void *) {
    l_TalkingState = 0;
    SDL_RemoveTimer(timer_id);
    timer_id = 0;
    LUSLOG_DEBUG("stop Talking", 0);
    return 0;
}

int32_t osVoiceStartReadData(OSVoiceHandle* hd) {
    if (timer_id) {
        return 0;
        // SDL_RemoveTimer(timer_id);
        // timer_id = 0;
    }
    l_TalkingState = 1;

    if (l_TalkingState) {
        LUSLOG_DEBUG("Start Timer", 0);
        timer_id = SDL_AddTimer(2000, stopTalking, NULL);
        SDL_ClearQueuedAudio(audio_dev);
        SDL_PauseAudioDevice(audio_dev, 0);
    }
    else { SDL_PauseAudioDevice(audio_dev, 1); }
    return 0;
}

int32_t osVoiceStopReadData(OSVoiceHandle* hd) {
    return 0;
}

int32_t osVoiceGetReadData(OSVoiceHandle* hd, OSVoiceData* result) {
    
    if (gPlayState == nullptr or !CVAR) {
        return 0;
    }


    SDL_PauseAudioDevice(audio_dev, 1);
    uint16_t match[5] = {0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF};
    // look into this, length seems to be around 5000. Need more than that or get error
    char audioData[20000];
    if (SDL_GetQueuedAudioSize(audio_dev) > 0) {
        u32 audioLength = SDL_DequeueAudio(audio_dev, audioData, SDL_GetQueuedAudioSize(audio_dev));
        LUSLOG_DEBUG("audioLength: %d", audioLength);
        vosk_recognizer_accept_waveform(recognizer, audioData, audioLength);
        const char* voskResult = vosk_recognizer_final_result(recognizer);
        LUSLOG_DEBUG("Result: %s", voskResult);
    }
    
    // json alts = voskResult.at("alternatives");
    


    // pseudo imple

    result->answer[0] = VOICE_WORD_ID_NONE;

    if (CHECK_BTN_ANY(gPlayState->state.input[0].cur.button, BTN_L)) {
        // These values arbitrarily chosen to meet thresholds required in voice_internal.c
        result->warning = 0;
        result->answerNum = 1;
        result->voiceLevel = 1000;
        result->voiceRelLevel = 2500;
        result->distance[0] = 10;

        result->answer[0] = CVAR_WORD;
    }

    return 0;
}

int32_t osVoiceClearDictionary(OSVoiceHandle* hd, u8 numWords) {
    wordListCount = 0;
    wordListLength = numWords;
    wordList.clear();
    if (recognizer) {
        vosk_recognizer_free(recognizer);
    }
    recognizer = nullptr;

    return 0;
}

int32_t osVoiceMaskDictionary(OSVoiceHandle* hd, u8* maskPattern, int size) {
    return 0;
}

int32_t osVoiceControlGain(OSVoiceHandle* hd, s32 analog, s32 digital) {
    return 0;
}

void RegisterVoiceRecognitionUnit() {
    if (CVAR && !model) {
        
        OSMesgQueue* serialEventQueue;
        osVoiceInit(serialEventQueue, &gVoiceHandle, 3);
        // Start the VRU loop
        func_801A4EB8();
    }
}

static RegisterShipInitFunc initFunc(RegisterVoiceRecognitionUnit, { CVAR_NAME });
