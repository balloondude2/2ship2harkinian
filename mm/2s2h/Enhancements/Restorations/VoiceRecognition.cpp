#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "Enhancements/Enhancements.h"
#include "2s2h/ShipInit.hpp"
#include <vosk_api.h>
#include <SDL.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include <libultraship/log/luslog.h>

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

struct VRUContext {
    VoskModel* modelEN = nullptr;
    VoskModel* modelJP = nullptr;

    VoskRecognizer* recognizerEN = nullptr;
    VoskRecognizer* recognizerJP = nullptr;

    VoskRecognizer* activeRecognizer = nullptr;

    SDL_AudioDeviceID device;

    std::vector<std::string> vocabEN;
    std::vector<std::string> vocabJP;
    std::vector<std::string>* activeVocab = nullptr;

    std::atomic<bool> listening = false;

    std::vector<int16_t> audioBuffer;
    std::mutex audioMutex;

    std::mutex resultMutex;

    int lastMatchIndex = -1;
};

static VRUContext gVRU = {};

// Original words: アトナンジカン, ハイチーズ, オキロー, オスワリ, ミルク, ハイヤー
// Original words adjusted for vosk model = {"後 なん 時間", "ハイ チーズ", "起きろ", "オスワリ", "ミルク", "ハイヤー"};
// Because game is polling for input so frequently, long phrases aren't captured. Set to match the last word
// I had limited success with a phrase buffer that could accomdate the vanilla phrases, but needed a way to clear it more consistently
static std::vector<std::string> ogwordListJP = { "時間", "チーズ", "起きろ", "オスワリ", "ミルク", "ハイヤー" };

// Simple english "translations". These could be adjusted, but currently work with vosk model.
static std::vector<std::string> ogwordList = { "time", "cheese", "wake", "sit", "milk", "go" };

std::string ExtractTextFromJson(const char* jsonStr) {
    if (!jsonStr)
        return "";

    try {
        auto j = json::parse(jsonStr);

        if (j.contains("text"))
            return j["text"].get<std::string>();

        return "";
    } catch (...) { return ""; }
}

void ProcessResult(const char* json) {
    std::string text = ExtractTextFromJson(json);

    if (!gVRU.activeVocab || text.empty())
        return;

    auto& vocab = *gVRU.activeVocab;

    for (size_t i = 0; i < vocab.size(); i++) {
        if (text == vocab[i]) {
            std::lock_guard lock(gVRU.resultMutex);
            gVRU.lastMatchIndex = i;
            gVRU.listening = false;
            break;
        }
    }
}

void AudioCallback(void* userdata, Uint8* stream, int len) {
    VRUContext* VRUctx = (VRUContext*)userdata;

    if (!VRUctx->listening)
        return;

    std::lock_guard lock(VRUctx->audioMutex);

    int16_t* samples = (int16_t*)stream;
    int sampleCount = len / sizeof(int16_t);

    VRUctx->audioBuffer.insert(VRUctx->audioBuffer.end(), samples, samples + sampleCount);
}

// Implementation of VRU functions
int32_t osVoiceInit(OSMesgQueue* mq, OSVoiceHandle* hd, int channel) {
    SPDLOG_INFO("osVoiceInit", 0);

    if (!(SDL_WasInit(SDL_INIT_AUDIO))) {
        SDL_InitSubSystem(SDL_INIT_AUDIO);
    }

    // TODO: Have cmake download these and update file paths accordingly
    gVRU.modelEN = vosk_model_new("C:\\Users\\ballo\\Code\\vosk-model-small-en-us-0.15");
    gVRU.modelJP = vosk_model_new("C:\\Users\\ballo\\Code\\vosk-model-small-ja-0.22");

    gVRU.vocabEN = ogwordList;
    gVRU.vocabJP = ogwordListJP;

    std::string json = "[";
    for (size_t i = 0; i < gVRU.vocabEN.size(); i++) {
        json += "\"" + gVRU.vocabEN[i] + "\"";
        if (i != gVRU.vocabEN.size() - 1)
            json += ",";
    }
    json += "]";

    gVRU.recognizerEN = vosk_recognizer_new_grm(gVRU.modelEN, 16000.0f, json.c_str());

    json = "[";
    for (size_t i = 0; i < gVRU.vocabJP.size(); i++) {
        json += "\"" + gVRU.vocabJP[i] + "\"";
        if (i != gVRU.vocabJP.size() - 1)
            json += ",";
    }
    json += "]";

    gVRU.recognizerJP = vosk_recognizer_new_grm(gVRU.modelJP, 16000.0f, json.c_str());

    if (CVAR == VOICE_RESTORATION_ENGLISH) {
        gVRU.activeRecognizer = gVRU.recognizerEN;
        gVRU.activeVocab = &gVRU.vocabEN;
    } else {
        gVRU.activeRecognizer = gVRU.recognizerJP;
        gVRU.activeVocab = &gVRU.vocabJP;
    }

    SDL_AudioSpec want{}, have{};
    want.freq = 16000;
    want.format = AUDIO_S16SYS;
    want.channels = 1;
    want.samples = 4096;
    want.callback = AudioCallback;
    want.userdata = &gVRU;

    gVRU.device = SDL_OpenAudioDevice(nullptr, SDL_TRUE, &want, &have, 0);

    if (gVRU.device == 0) {
        LUSLOG_DEBUG("SDL_OpenAudioDevice failed: %s\n", SDL_GetError());
        return -1;
    }

    SDL_PauseAudioDevice(gVRU.device, 0);

    return 0;
}

int32_t osVoiceSetWord(OSVoiceHandle* hd, u8* word) {
    // SPDLOG_INFO("osVoiceSetWord", 0);
    // currently hardcoding the word list in osVoiceInit()
    return 0;
}

int32_t osVoiceCheckWord(u8* word) {
    // SPDLOG_INFO("osVoiceCheckWord", 0);
    return 0;
}

int32_t osVoiceStartReadData(OSVoiceHandle* hd) {
    // SPDLOG_INFO("osVoiceStartReadData", 0);
    gVRU.lastMatchIndex = -1;

    vosk_recognizer_reset(gVRU.activeRecognizer);

    gVRU.listening = true;
    return 0;
}

int32_t osVoiceStopReadData(OSVoiceHandle* hd) {
    // SPDLOG_INFO("osVoiceStopReadData", 0);
    return 0;
}

int32_t osVoiceGetReadData(OSVoiceHandle* hd, OSVoiceData* result) {
    // SPDLOG_INFO("osVoiceGetReadData", 0);
    if (gPlayState == nullptr or CVAR == VOICE_RESTORATION_OFF) {
        return 0;
    }

    std::lock_guard audioLock(gVRU.audioMutex);

    if (!gVRU.audioBuffer.empty()) {
        vosk_recognizer_accept_waveform(gVRU.activeRecognizer, (const char*)gVRU.audioBuffer.data(),
                                        gVRU.audioBuffer.size() * sizeof(int16_t));

        gVRU.audioBuffer.clear();

        ProcessResult(vosk_recognizer_result(gVRU.activeRecognizer));
    }

    std::lock_guard lock(gVRU.resultMutex);

    // Arbitraily chose values to satisfy the voice_internal.c checks
    result->warning = 0;
    result->answerNum = 1;
    result->voiceLevel = 1000;
    result->voiceRelLevel = 2500;
    result->distance[0] = 10;
    result->answer[0] = gVRU.lastMatchIndex;
    return 0;
}

int32_t osVoiceClearDictionary(OSVoiceHandle* hd, u8 numWords) {
    // SPDLOG_INFO("ClearDictionary", 0);
    return 0;
}

int32_t osVoiceMaskDictionary(OSVoiceHandle* hd, u8* maskPattern, int size) {
    // SPDLOG_INFO("MaskDictionary", 0);
    return 0;
}

int32_t osVoiceControlGain(OSVoiceHandle* hd, s32 analog, s32 digital) {
    // SPDLOG_INFO("ControlGain", 0);
    return 0;
}

void SwitchLanguageModel() {

    if (CVAR == VOICE_RESTORATION_JAPANESE) {
        if (!gVRU.activeRecognizer || !gVRU.activeVocab) {
            return;
        }
        gVRU.activeRecognizer = gVRU.recognizerJP;
        gVRU.activeVocab = &gVRU.vocabJP;
    } else if (CVAR == VOICE_RESTORATION_ENGLISH) {
        if (!gVRU.activeRecognizer || !gVRU.activeVocab) {
            return;
        }
        gVRU.activeRecognizer = gVRU.recognizerEN;
        gVRU.activeVocab = &gVRU.vocabEN;
    }
}

void RegisterVoiceRecognitionUnit() {

    // Not sure if best way to check if VRU has initialized
    if (CVAR != VOICE_RESTORATION_OFF && !gVRU.activeVocab) {
        OSMesgQueue* serialEventQueue;
        osVoiceInit(serialEventQueue, &gVoiceHandle, 3);
        // Start the VRU loop
        func_801A4EB8();
    }

    // OnSaveLoad is probably terrible. We don't want to do this more than once.
    // COND_HOOK(OnSaveLoad, CVAR, [](s16 fileNum) {
    //     OSMesgQueue* serialEventQueue;
    //     osVoiceInit(serialEventQueue, &gVoiceHandle, 3);
    //     // Start the VRU loop
    //     func_801A4EB8();
    // });

    SwitchLanguageModel();
}

// Need to switch recognizer and activeVocab on CVAR change.
static RegisterShipInitFunc initFunc(RegisterVoiceRecognitionUnit, { CVAR_NAME });